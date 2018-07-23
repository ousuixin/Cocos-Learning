#include "MenuScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool MenuScene::onTouchBegan(Touch *touch, Event *e) {
    if (this->getChildByTag(6) && this->getChildByTag(6)->getBoundingBox().containsPoint(touch->getLocation())) {
        // °ÑÍ¼Æ¬½øÐÐÌæ»»
        auto start = Sprite::create("start-1.png");
        start->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2 + Director::getInstance()->getVisibleOrigin().x + 300, Director::getInstance()->getVisibleSize().height / 2 + Director::getInstance()->getVisibleOrigin().y - 120));
        this->addChild(start, 4, 7);

        Director::getInstance()->replaceScene(TransitionFade::create(1, GameSence::createScene()));
        return true;
    }
    return false;
}

void MenuScene::startGame(Ref* pSender) {
    
};

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0, 1);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1, 2);

	auto leg = Sprite::create();
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1, 3);

    auto gameName = Sprite::create("gold-miner-text.png");
    gameName->setPosition(Vec2(visibleSize.width/2 + origin.x + 30 , visibleSize.height/2 + origin.y + 170));
    this->addChild(gameName, 2, 4);

    auto menuStartGold = Sprite::create("menu-start-gold.png");
    menuStartGold->setPosition(Vec2(visibleSize.width/2 + origin.x + 300, visibleSize.height/2 + origin.y - 170));
    this->addChild(menuStartGold, 2, 5);


    auto start = Sprite::create("start-0.png");
    start->setPosition(Vec2(visibleSize.width/2 + origin.x + 300, visibleSize.height/2 + origin.y - 120));
    this->addChild(start, 3, 6);

    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(MenuScene::onTouchBegan, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


    return true;
}



