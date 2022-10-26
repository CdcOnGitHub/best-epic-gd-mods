#include "CvoltonOptionsLayer.h"
#include "../managers/CvoltonManager.h"

#include <cocos2d.h>
#include <gd.h>

using namespace cocos2d;
using namespace gd;

CvoltonOptionsLayer* CvoltonOptionsLayer::create(){
    auto ret = new CvoltonOptionsLayer();
    if (ret && ret->init()) {
        //robert 1 :D
        ret->autorelease();
    } else {
        //robert -1
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void CvoltonOptionsLayer::onClose(cocos2d::CCObject* sender)
{   
    auto CM = CvoltonManager::sharedState();
    CM->save();
    destroyToggles();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

bool CvoltonOptionsLayer::init(){
    bool init = createBasics({300.0f, 200.0f}, menu_selector(CvoltonOptionsLayer::onClose), 0.8f);
    if(!init) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    createTextLabel("Mod Settings", {(winSize.width / 2),(winSize.height / 2) + 78}, 0.7f, m_pLayer, "goldFont.fnt");

    drawToggles();

    return true;
}

CCLabelBMFont* CvoltonOptionsLayer::createTextLabel(const std::string text, const CCPoint& position, const float scale, CCNode* menu, const char* font){
    CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
    bmFont->setPosition(position);
    bmFont->setScale(scale);
    menu->addChild(bmFont);
    return bmFont;
}

void CvoltonOptionsLayer::onToggle(cocos2d::CCObject* sender)
{
    sender->retain();
    auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto CM = CvoltonManager::sharedState();
    CM->toggleOption(static_cast<CCString*>(button->getUserData())->getCString());
    //if (CvoltonManager::sharedState()->getOption("cool_toggle"))
    //{
    //    exit(0);
    //}

    destroyToggles();
    drawToggles();
    sender->release();
}
void CvoltonOptionsLayer::onInfo(cocos2d::CCObject* sender)
{
        auto amogus = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto text = static_cast<CCLabelBMFont*>(amogus->getChildren()->objectAtIndex(1));
        auto texttext = text->getString();
        auto text2 = static_cast<CCLabelBMFont*>(amogus->getChildren()->objectAtIndex(2));
        auto texttext2 = text2->getString();
        std::ostringstream optionInfo;
        optionInfo << "<cy>" << texttext2 << "</c>" << "\n\n" << texttext;
        FLAlertLayer::create(nullptr, "Help", "OK", nullptr, optionInfo.str())->show();

}

void CvoltonOptionsLayer::createToggle(const char* option, const char* name, const char* desc, bool active) {
    auto CM = CvoltonManager::sharedState();

    auto buttonSprite = CCSprite::createWithSpriteFrameName(CM->getOption(option) ? "GJ_checkOn_001.png" : "GJ_checkOff_001.png");
    buttonSprite->setScale(0.8f);
    auto amog = menu_selector(CvoltonOptionsLayer::onToggle);
    if (active)
    {
        auto amog = menu_selector(CvoltonOptionsLayer::onToggle);
    }
    else
    {
        amog = nullptr;
        buttonSprite->setColor({ 200,200,200 });
    }
    auto button = gd::CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        amog
    );
    m_pButtonMenu->addChild(button);
    float y = 45.f - (toggleCount++ * 40.f);
    button->setPosition({-127, y});
    auto optionString = CCString::create(option);
    optionString->retain();
    button->setUserData(optionString);
    button->setSizeMult(1.2f);
    if (desc)
    {
        
        auto spriter = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        auto infosprite = CCMenuItemSpriteExtra::create(
            spriter,
            this,
            menu_selector(CvoltonOptionsLayer::onInfo)
        );
        infosprite->setPosition({ button->getPositionX()-15,button->getPositionY()+20 });
        spriter->setScale(0.5f);
        m_pButtonMenu->addChild(infosprite);
        auto desctext = CCLabelBMFont::create(desc, "chatFont.fnt");
        auto desctext2 = CCLabelBMFont::create(name, "chatFont.fnt");
        desctext->setVisible(false);
        desctext2->setVisible(false);
        infosprite->addChild(desctext);
        infosprite->addChild(desctext2);
    }
    auto label = createTextLabel(name, {-107, y}, 0.5f, m_pButtonMenu);
    label->setAnchorPoint({0,0.5f});
}

void CvoltonOptionsLayer::createButtonToggle(const char* option, CCNode* sprite, float x, float y, float scale){
    auto CM = CvoltonManager::sharedState();

    //auto buttonSprite = CCSprite::createWithSpriteFrameName(sprite);
    sprite->setScale(scale);
    auto button = gd::CCMenuItemSpriteExtra::create(
        sprite,
        this,
        menu_selector(CvoltonOptionsLayer::onToggle)
    );
    
    m_pButtonMenu->addChild(button);
    button->setPosition({x, y});
    if(!CM->getOption(option)) button->setColor({125,125,125});
    auto optionString = CCString::create(option);
    optionString->retain();
    button->setUserData(optionString);
    button->setSizeMult(1.2f);
}

void CvoltonOptionsLayer::destroyToggles(){
    //starting at 1 because 0 is the close button
    unsigned int totalMembers = m_pButtonMenu->getChildrenCount();
    for(unsigned int i = 1; i < totalMembers; i++){
        //static index 1 because we're actively moving the elements
        auto object = static_cast<CCNode*>(m_pButtonMenu->getChildren()->objectAtIndex(1));
        auto userData = object->getUserData();
        if(userData != nullptr) static_cast<CCString*>(userData)->release();
        //m_pButtonMenu->removeChild(object, false);
        object->removeFromParent();
    }
    toggleCount = 0;
}

void CvoltonOptionsLayer::drawToggles(){
    //createToggle("no_update_check", "Disable Update Check (disabled)","<cr>Disables</c> the update check.\n This feature is <cr>deprecated</c> in favor of <cy>auto</c><cj>-</c><cb>update</c>.",false);
    createToggle("no_green_user", "Disable Green Username Fix","<cr>Disables</c> the <cg>green</c> username fix.\nYou can turn this <cr>OFF</c> if you want to.",true);
    createToggle("no_level_info", "Disable Extended Level Info", nullptr,true);
    createToggle("white_id", "White Level ID text", "Makes <cy>level IDs</c> white on <cj>Level Cells</c>",true);
    //createToggle("cool_toggle", "Cool Toggle", "Be <cr>careful</c> who you <cy>trust</c> there might be an <cd>impostor</c> <cr>among us</c>", true);
    
}