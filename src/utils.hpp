#pragma once

template <typename R, typename T>
inline R cast(T value) { return reinterpret_cast<R>(value); }

inline auto follow(uintptr_t addr) { return *cast<uintptr_t*>(addr); }
inline auto follow(void* addr) { return *cast<void**>(addr); }

// only use this when necessary
template <typename T, typename U>
T union_cast(U value) {
    union {
        T a;
        U b;
    } u;
    u.b = value;
    return u.a;
}

namespace BetterInfo {
    
    const int mainBtnTag = 863390896;
    const int mainBtnExMarkTag = 863390897;

    cocos2d::CCSprite* createWithBISpriteFrameName(const char* name);
    cocos2d::CCSprite* createBISprite(const char* name);
    cocos2d::CCSprite* createPlaceholder();

    bool isLocal(gd::GJSearchObject* searchObj);
    bool isFalseTotal(gd::GJSearchObject* searchObj);
    int levelsPerPage(gd::GJSearchObject* searchObj);
    const char* rankIcon(int position);
    void showBIExclamationMark(cocos2d::CCLayer* creator);

}