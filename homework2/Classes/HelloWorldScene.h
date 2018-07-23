#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    virtual void update(float dt);
    // ---------------------------------------
    // 12周新增函数
    void HelloWorld::addMonster(float dt);
    void HelloWorld::hitByMonster(float dt);
    void HelloWorld::myFlips(char cid);
    // ---------------------------------------

    void HelloWorld::playAttackAnimate(Object* temp);

    void HelloWorld::playDieAnimate(Object* temp);

    void HelloWorld::playRunAnimateW(Object* temp);
    void HelloWorld::playRunAnimateS(Object* temp);
    void HelloWorld::playRunAnimateA(Object* temp);
    void HelloWorld::playRunAnimateD(Object* temp);
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime;
	cocos2d::ProgressTimer* pT;

    Sequence* myAction = NULL;
    Sequence* moveAction = NULL;

    // ---------------------------------------
    // 12周新增变量
    int delayTime = 0;
    char lastCid = 'D';
    int killNum = 0;
    // ---------------------------------------
};
