#include "CvoltonManager.h"
#include <gd.h>
#include <random>
#include <filesystem>

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;

bool CvoltonManager::init(){
    bool init = CCNode::init();
    if(!init) return false;

    this->m_sFileName = "CCBetterInfo.dat";

    auto FU = CCFileUtils::sharedFileUtils();

    std::string plistPath(FU->getWritablePath2() + "Resources/BI_destroyedUsers.plist");
    if(FU->isFileExist(plistPath)) {
        nameDict = CCDictionary::createWithContentsOfFile("BI_destroyedUsers.plist");
        plistLoaded = true;
    }
    else {
        nameDict = CCDictionary::create();
    }
    nameDict->retain();

    settingsDict = CCDictionary::create();
    settingsDict->retain();

    this->setup();

    std::cout << "initing" << std::endl;

    return true;
}

void CvoltonManager::doUpdateHttpRequest(){
	CCHttpRequest* request = new CCHttpRequest;
    request->setUrl("https://cvolton.eu/gdmods/api/betterinfo/version/");
    request->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
    request->setResponseCallback(this, httpresponse_selector(CvoltonManager::onUpdateHttpResponse));
    CCHttpClient::getInstance()->send(request);
    request->release();
}

void CvoltonManager::onUpdateHttpResponse(CCHttpClient* client, CCHttpResponse* response){
    if(!(response->isSucceed())) return;

    std::vector<char>* responseData = response->getResponseData();
    std::string responseString(responseData->begin(), responseData->end());
    latestVer = responseString;

    if(isUpToDate()) return;

    std::ostringstream stream;
    stream << modName << " " << responseString;


    AchievementNotifier::sharedState()->notifyAchievement("Update available", stream.str().c_str(), "", false);
    //FLAlertLayer::create(nullptr, "User Info", "OK", nullptr, 300, stream.str().c_str())->show();
}

void CvoltonManager::onChangelogHttpResponse(CCHttpClient* client, CCHttpResponse* response){
    CvoltonUpdateLayer* updateLayer = static_cast<CvoltonUpdateLayer*>(response->getHttpRequest()->getUserData());

    if(!(response->isSucceed())){
        updateLayer->onLoadFailed();
        return;
    }

    std::vector<char>* responseData = response->getResponseData();
    std::string responseString(responseData->begin(), responseData->end());
    changelog = responseString;

    updateLayer->onLoadFinished();
    updateLayer->release();
    //FLAlertLayer::create(nullptr, "User Info", "OK", nullptr, 300, stream.str().c_str())->show();
}

//CvoltonManager* CvoltonManager::sharedState()

CvoltonManager::CvoltonManager(){}

void CvoltonManager::doUpdateCheck(){
    if(getOption("no_update_check")) return;

    forceUpdateCheck();
}

void CvoltonManager::forceUpdateCheck(){
    if(hasDoneUpdateCheck) return;

    doUpdateHttpRequest();
    hasDoneUpdateCheck = true;
}

const char* CvoltonManager::getUserName(int id){
    auto obj = dynamic_cast<CCString*>(nameDict->objectForKey(std::to_string(id)));
    if(obj == nullptr) return "";
    return obj->getCString();
}

void CvoltonManager::downloadChangelog(CvoltonUpdateLayer* updateLayer){
    updateLayer->retain();
    forceUpdateCheck();

    CCHttpRequest* request = new CCHttpRequest;
    request->setUrl("https://cvolton.eu/gdmods/api/betterinfo/changelog/latest/");
    request->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
    request->setUserData(updateLayer);
    request->setResponseCallback(this, httpresponse_selector(CvoltonManager::onChangelogHttpResponse));
    CCHttpClient::getInstance()->send(request);
    //request->release();

}

bool CvoltonManager::isUpToDate(){
    if(latestVer == "") return true;
    return version == latestVer;
}

void CvoltonManager::encodeDataTo(DS_Dictionary* data) {
    data->setDictForKey("settings", settingsDict);
    data->setStringForKey("versionString", version);

    std::cout << "encodeDataTo";
}
void CvoltonManager::dataLoaded(DS_Dictionary* data) {
    settingsDict->release();
    settingsDict = static_cast<CCDictionary*>(data->getObjectForKey("settings"));
    settingsDict->retain();

    this->save();
}
void CvoltonManager::firstLoad() {
    std::cout << "firstLoad";

    this->save();
}
bool CvoltonManager::getOption(std::string option){
    return settingsDict->valueForKey(option)->boolValue();
}
void CvoltonManager::toggleOption(std::string option){
    settingsDict->setObject(CCString::createWithFormat("%i", !getOption(option)), option);
}

int CvoltonManager::getOptionInt(std::string option){
    return settingsDict->valueForKey(option)->intValue();
}
void CvoltonManager::setOptionInt(std::string option, int value){
    settingsDict->setObject(CCString::createWithFormat("%i", value), option);
}

void CvoltonManager::loadTextures(){
    CCTextureCache::sharedTextureCache()->addImage("BI_GameSheet.png", 0);
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("BI_GameSheet.plist");
}

int CvoltonManager::randomNumber(int start, int end){
    std::random_device os_seed;
    const unsigned int seed = os_seed();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribute(start, end);

    return distribute(generator);
}

int CvoltonManager::completedLevelsForStars(int stars){
    //TODO: do not iterate this 10 times for values 0-10
    int res = 0;
    auto GLM = GameLevelManager::sharedState();
    auto levels = GLM->m_onlineLevels;
    CCDictElement* obj;
    CCDICT_FOREACH(levels, obj){
        auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
        if(currentLvl != nullptr && currentLvl->stars == stars && currentLvl->normalPercent == 100) res++;
    }

    auto dailyLevels = GLM->m_dailyLevels;
    CCDICT_FOREACH(dailyLevels, obj){
        auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
        if(currentLvl != nullptr && currentLvl->stars == stars && currentLvl->normalPercent == 100) res++;
    }

    auto gauntletLevels = GLM->m_gauntletLevels;
    CCDICT_FOREACH(gauntletLevels, obj){
        auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
        if(currentLvl != nullptr && currentLvl->stars == stars && currentLvl->normalPercent == 100) res++;
    }

    return res;
}

/*
    This is a reimplementation of GameLevelManager::responseToDict
    because I couldn't get it to work. It's not 1:1 with the original
    so that's why it's here, even though it achieves the same purpose.
*/
CCDictionary* CvoltonManager::responseToDict(std::string response){
    auto dict = CCDictionary::create();

    std::stringstream responseStream(response);
    std::string currentKey;
    std::string keyID;

    unsigned int i = 0;
    while(getline(responseStream, currentKey, ':')){

        if(i % 2 == 0) keyID = currentKey;
        else dict->setObject(CCString::create(currentKey.c_str()),keyID);

        i++;
    }

    return dict;
}

void CvoltonManager::missingResourcesError() {
    if(hasDoneHealthCheck) return;

    auto FU = CCFileUtils::sharedFileUtils();
    std::ostringstream stream;
    if(!plistLoaded) stream << "\nBI_destroyedUsers.plist";
    for(auto texture : textures){
        constexpr char* extensions[] = {"", "-hd", "-uhd"};
        for(auto extension : extensions){
            std::string plistPath(FU->getWritablePath2() + "Resources/" + texture + extension + ".plist");
            std::string pngPath(FU->getWritablePath2() + "Resources/" + texture + extension + ".png");
            if(!(FU->isFileExist(plistPath))) stream << "\n" << texture << extension << ".plist";
            if(!(FU->isFileExist(plistPath))) stream << "\n" << texture << extension << ".png";
        }
    }

    std::string file_list(stream.str());
    if(!(file_list.empty())) FLAlertLayer::create(nullptr, modName, "OK", nullptr, 350, "<cr>Error: Missing resources!</c>\n\nUnable to find:<cl>"+file_list+"</c>\n\nPlease refer to the <cg>installation instructions</c>.")->show();

    hasDoneHealthCheck = true;
}