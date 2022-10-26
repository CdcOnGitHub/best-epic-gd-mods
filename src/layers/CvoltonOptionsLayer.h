#pragma once
#include <gd.h>
#include "CvoltonAlertLayerStub.h"

class CvoltonOptionsLayer : public CvoltonAlertLayerStub {
protected:
    int toggleCount = 0;
public:
    static CvoltonOptionsLayer* create();
    void onClose(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);
    bool init();
    virtual void destroyToggles();
    virtual void drawToggles();
    static cocos2d::CCLabelBMFont* createTextLabel(const std::string text, const cocos2d::CCPoint& position, const float scale, cocos2d::CCNode* menu, const char* font = "bigFont.fnt");
    void createToggle(const char* option, const char* name, const char* desc,bool active);
    void createButtonToggle(const char* option, cocos2d::CCNode* sprite, float x, float y, float scale = 1.f);
    void onInfo(cocos2d::CCObject* sender);
};