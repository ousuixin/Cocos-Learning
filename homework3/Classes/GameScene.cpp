#include "GameScene.h"
#include <ctime>
USING_NS_CC;

Scene* GameSence::createScene()
{
	return GameSence::create();
}

// on "init" you need to initialize your instance
bool GameSence::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

    auto visibleSize1 = Director::getInstance()->getVisibleSize();
    Vec2 origin1 = Director::getInstance()->getVisibleOrigin();

    auto bg = Sprite::create("level-background-0.jpg");
    bg->setPosition(Vec2(visibleSize1.width / 2 + origin1.x, visibleSize1.height / 2 + origin1.y));
    this->addChild(bg, 0, 0);

    auto stoneLayer = Label::create();
    stoneLayer->setPosition(Vec2(origin1.x, origin1.y));
    auto stone = Sprite::create("stone.png");
    stone->setPosition(Vec2(560, 480));
    stoneLayer->addChild(stone, 0, 0);
    this->addChild(stoneLayer, 0, 1);

    auto mouseLayer = Label::create();
    mouseLayer->setPosition(Vec2(origin1.x, origin1.y + visibleSize1.height/2));
    this->addChild(mouseLayer, 0, 2);
    auto mouse = Sprite::create();
    Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
    mouse->runAction(RepeatForever::create(mouseAnimate));
    mouse->setPosition(visibleSize1.width/2, 0);
    mouseLayer->addChild(mouse, 0, 0);

    auto diamondLayer = Label::create();
    diamondLayer->setPosition(Vec2(origin1.x, origin1.y));
    this->addChild(diamondLayer, 0, 3);

    auto shoot = Label::createWithSystemFont("shoot", "fonts/arial.ttf", 55);
    shoot->setPosition(Vec2(visibleSize1.width / 2 + origin1.x + 300, visibleSize1.height / 2 + origin1.y + 160));
    this->addChild(shoot, 0, 4);

    auto cheeseLayer = Label::create();
    cheeseLayer->setPosition(origin1.x, origin1.y);
    this->addChild(cheeseLayer, 0, 5);

    //add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {
    if (this->myAction != NULL) {
        if (this->myAction->isDone()) {
            this->myAction->release();
        }
        else {
            return true;
        }
    }
    if (this->getChildByTag(4) && this->getChildByTag(4)->getBoundingBox().containsPoint(touch->getLocation())) {
        // 获取老鼠的位置（发射的石头的终点）
        Vec2 loc0 = this->getChildByTag(2)->getPosition();
        Vec2 loc = this->getChildByTag(2)->getChildByTag(0)->getPosition();
        Vec2 worldLoc = Vec2(loc.x + loc0.x, loc.y + loc0.y);

        // 将老鼠移动到随机的一个位置
        srand((unsigned)time(NULL));
        srand((unsigned)time(NULL) + rand());
        int x = rand()*10 % (int)(Director::getInstance()->getVisibleSize().width - 40);
        srand((unsigned)time(NULL) + rand());
        int y = rand()*7 % (int)Director::getInstance()->getVisibleSize().height - (int)Director::getInstance()->getVisibleSize().height / 2;
        auto moveMouse = MoveTo::create(1.0f, Vec2(x+20, y));
        this->myAction = Sequence::create(moveMouse, nullptr);
        this->myAction->retain();
        this->getChildByTag(2)->getChildByTag(0)->runAction(this->myAction);
        
        // 留下一块钻石
        Sprite* diamond = Sprite::create("diamond.png");
        Animate* diamondAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("diamondAnimation"));
        diamond->runAction(RepeatForever::create(diamondAnimate));
        diamond->setPosition(worldLoc);
        this->getChildByTag(3)->addChild(diamond, 1);

        // 创建石头并添加到stoneLayer层
        auto stoneTemp = Sprite::create("stone.png");
        stoneTemp->setPosition(560, 480);
        this->getChildByTag(1)->removeChildByTag(1);
        this->getChildByTag(1)->addChild(stoneTemp, 1, 1);
        // 移动到老鼠位置(经过1s时间)
        auto moveTo = MoveTo::create(1.0f, worldLoc);
        // 石头淡出
        auto fadeOut = FadeOut::create(0.5f);
        // 构造队列并运行队列
        auto seq = Sequence::create(moveTo, fadeOut, nullptr);
        stoneTemp->runAction(seq);
    }
    // 如果没有点击发射石头，就会在点击的地方放上一块奶酪
    else {
        // 留下奶酪
        Sprite* cheese = Sprite::create("cheese.png");
        cheese->setPosition(touch->getLocation());
        this->getChildByTag(5)->addChild(cheese, 0);
        // 奶酪逐渐消失
        auto moveTo = MoveTo::create(2.0f, touch->getLocation());
        auto fadeOut = FadeOut::create(3.0f);
        auto seq = Sequence::create(moveTo, fadeOut, nullptr);
        cheese->runAction(seq);

        // 老鼠移动到奶酪
        auto moveMouse = MoveTo::create(2.0f, Vec2(touch->getLocation().x, touch->getLocation().y - Director::getInstance()->getVisibleSize().height/2));
        this->myAction = Sequence::create(moveMouse, nullptr);
        this->myAction->retain();
        this->getChildByTag(2)->getChildByTag(0)->runAction(this->myAction);
    }

	return true;
}
