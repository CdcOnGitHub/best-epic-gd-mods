#pragma once
#include <gd.h>

class UnregisteredProfileLayer : public gd::FLAlertLayer {
    gd::GJUserScore* score;
public:
    static UnregisteredProfileLayer* create(gd::GJUserScore* score);
    void onClose(cocos2d::CCObject* sender);
    static void displayProfile(int userID, std::string userName);
    static void doSearch(int userID);
    void onMyLevels(CCObject* sender);
    void onCommentHistory(CCObject* sender);
    bool init();
};