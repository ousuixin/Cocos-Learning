#pragma once
#include <stdio.h>
#include "cocos2d.h"
USING_NS_CC;

class MenuScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    virtual bool onTouchBegan(Touch* touch, Event* e);

    void startGame(Ref* pSender);
    
	// a selector callback
	//void startMenuCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MenuScene);
};
