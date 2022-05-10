#pragma once

#include <gd.h>

class LeaderboardListView : public gd::CustomListView {
protected:
    void setupList() override;
    gd::TableViewCell* getListCell(const char* key) override;
    void loadCell(gd::TableViewCell* cell, unsigned int index) override;
    
public:
    static LeaderboardListView* create(cocos2d::CCArray* scores, float width, float height);
};