#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Monster.h"
#pragma execution_character_set("utf-8")
# define database UserDefault::getInstance()

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

void HelloWorld::playAttackAnimate(Object* temp) {
    if (this->myAction != NULL) {
        if (this->myAction->isDone()) {
            this->myAction->release();
        }
        else {
            return;
        }
    }
    auto animation = Animation::createWithSpriteFrames(attack, 0.1f);
    auto animate = Animate::create(animation);
    auto animation1 = Animation::createWithSpriteFrames(idle, 0.1f);
    auto animate1 = Animate::create(animation1);
    myAction = Sequence::create(animate, animate1, nullptr);
    this->myAction->retain();
    player->runAction(myAction);

    // 如果攻击范围有怪物就能够回复血量
    Rect playerRect = player->getBoundingBox();
    Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY(),
        playerRect.getMaxX() - playerRect.getMinX() + 80,
        playerRect.getMaxY() - playerRect.getMinY());
    auto factory = Factory::getInstance();
    Sprite* monster = factory->collider(attackRect);
    while (monster != NULL) {
        // 击倒怪物
        factory->removeMonster(monster);
        // 回复血
        pT->setScaleX(pT->getScaleX() >= 90 ? 90 : pT->getScaleX() + 10);
        // 加分并写入数据库
        killNum++;
        char temp[30] = {};
        sprintf(temp, "Kill Number : %d", killNum);
        Label* Kill = (Label*)this->getChildByTag(5);
        Kill->setString(temp);
        database->setIntegerForKey("killNumer", killNum);
        database->flush();

        monster = factory->collider(attackRect);
    }
}

void HelloWorld::playDieAnimate(Object* temp) {
    if (this->myAction != NULL) {
        if (this->myAction->isDone()) {
            this->myAction->release();
        }
        else {
            return;
        }
    }
    auto animation = Animation::createWithSpriteFrames(dead, 0.1f);
    auto animate = Animate::create(animation);
    auto animation1 = Animation::createWithSpriteFrames(idle, 0.1f);
    auto animate1 = Animate::create(animation1);
    myAction = Sequence::create(animate, animate1, nullptr);
    this->myAction->retain();
    player->runAction(myAction);
}

void HelloWorld::playRunAnimateW(Object* temp) {
    if (this->myAction != NULL) {
        if (this->myAction->isDone()) {
            this->myAction->release();
        }
        else {
            return;
        }
    }
    auto animation = Animation::createWithSpriteFrames(run, 0.025f);
    auto animate = Animate::create(animation);
    auto animation1 = Animation::createWithSpriteFrames(idle, 0.1f);
    auto animate1 = Animate::create(animation1);
    myAction = Sequence::create(animate, animate1, nullptr);
    this->myAction->retain();
    player->runAction(myAction);

    auto moveTo = MoveTo::create(0.2f, Vec2(player->getPosition().x, player->getPosition().y + 20));
    moveAction = Sequence::create(moveTo, nullptr);
    moveAction->retain();
    player->runAction(moveAction);
}
void HelloWorld::playRunAnimateS(Object* temp) {
    if (this->myAction != NULL) {
        if (this->myAction->isDone()) {
            this->myAction->release();
        }
        else {
            return;
        }
    }
    auto animation = Animation::createWithSpriteFrames(run, 0.025f);
    auto animate = Animate::create(animation);
    auto animation1 = Animation::createWithSpriteFrames(idle, 0.1f);
    auto animate1 = Animate::create(animation1);
    myAction = Sequence::create(animate, animate1, nullptr);
    this->myAction->retain();
    player->runAction(myAction);

    auto moveTo = MoveTo::create(0.2f, Vec2(player->getPosition().x, player->getPosition().y - 20));
    moveAction = Sequence::create(moveTo, nullptr);
    moveAction->retain();
    player->runAction(moveAction);
}
void HelloWorld::playRunAnimateA(Object* temp) {
    if (this->myAction != NULL) {
        if (this->myAction->isDone()) {
            this->myAction->release();
        }
        else {
            return;
        }
    }
    myFlips('a');

    auto animation = Animation::createWithSpriteFrames(run, 0.025f);
    auto animate = Animate::create(animation);
    auto animation1 = Animation::createWithSpriteFrames(idle, 0.1f);
    auto animate1 = Animate::create(animation1);
    myAction = Sequence::create(animate, animate1, nullptr);
    this->myAction->retain();
    player->runAction(myAction);

    auto moveTo = MoveTo::create(0.2f, Vec2(player->getPosition().x - 20, player->getPosition().y));
    moveAction = Sequence::create(moveTo, nullptr);
    moveAction->retain();
    player->runAction(moveAction);
}
void HelloWorld::playRunAnimateD(Object* temp) {
    if (this->myAction != NULL) {
        if (this->myAction->isDone()) {
            this->myAction->release();
        }
        else {
            return;
        }
    }
    myFlips('d');

    auto animation = Animation::createWithSpriteFrames(run, 0.025f);
    auto animate = Animate::create(animation);
    auto animation1 = Animation::createWithSpriteFrames(idle, 0.1f);
    auto animate1 = Animate::create(animation1);
    myAction = Sequence::create(animate, animate1, nullptr);
    this->myAction->retain();
    player->runAction(myAction);

    auto moveTo = MoveTo::create(0.2f, Vec2(player->getPosition().x + 20, player->getPosition().y));
    moveAction = Sequence::create(moveTo, nullptr);
    moveAction->retain();
    player->runAction(moveAction);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    String test = FileUtils::getInstance()->getWritablePath();
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 68, 101)));
    texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3, 0);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0, 2);

    auto menuLabelW = Label::createWithSystemFont("W", "arial.ttf", 36);
    auto itemW = MenuItemLabel::create(menuLabelW, CC_CALLBACK_1(HelloWorld::playRunAnimateW, this));
    itemW->setPosition(Vec2(origin.x + 100, origin.y + 80 + menuLabelW->getHeight()));

    auto menuLabelS = Label::createWithSystemFont("S", "arial.ttf", 36);
    auto itemS = MenuItemLabel::create(menuLabelS, CC_CALLBACK_1(HelloWorld::playRunAnimateS, this));
    itemS->setPosition(Vec2(origin.x + 100, origin.y + 30));

    auto menuLabelA = Label::createWithSystemFont("A", "arial.ttf", 36);
    auto itemA = MenuItemLabel::create(menuLabelA, CC_CALLBACK_1(HelloWorld::playRunAnimateA, this));
    itemA->setPosition(Vec2(origin.x + 50, origin.y + 30));

    auto menuLabelD = Label::createWithSystemFont("D", "arial.ttf", 36);
    auto itemD = MenuItemLabel::create(menuLabelD, CC_CALLBACK_1(HelloWorld::playRunAnimateD, this));
    itemD->setPosition(Vec2(origin.x + 150, origin.y + 30));

    auto menuLabelX = Label::createWithSystemFont("X", "arial.ttf", 36);
    auto itemX = MenuItemLabel::create(menuLabelX, CC_CALLBACK_1(HelloWorld::playAttackAnimate, this));
    itemX->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + 30));

    auto menuLabelY = Label::createWithSystemFont("Y", "arial.ttf", 36);
    auto itemY = MenuItemLabel::create(menuLabelY, CC_CALLBACK_1(HelloWorld::playDieAnimate, this));
    itemY->setPosition(Vec2(origin.x + visibleSize.width - 50, origin.y + 30));

    auto menu = Menu::create(itemW, itemS, itemA, itemD, itemX, itemY, NULL);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 0, 3);

    dtime = 10800;
    char temp[30] = {};
    sprintf(temp, "Time Left : %d", dtime/60);
    time = Label::createWithSystemFont(temp, "arial.ttf", 36);
    time->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height - 40));
    addChild(time, 0, 4);

    killNum = 0;
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "Kill Number : %d", killNum);
    Label* KillNum = Label::createWithSystemFont(temp, "arial.ttf", 36);
    KillNum->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 70));
    addChild(KillNum, 0, 5);

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i - 12, 0, 113, 113)));
		attack.pushBack(frame);
	}

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	// Todo
    for (int i = 0; i < 22; i++) {
        auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
        dead.pushBack(frame);
    }

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
    for (int i = 0; i < 8; i++) {
        auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, i == 7 ? 0 : i, 68, 101)));
        run.pushBack(frame);
    }
	// Todo

    // ----------------------------------------------------------------------------------------
    // 以下是12周添加部分

    // 每间隔5s产生一波怪物
    addMonster(2.0f);
    schedule(schedule_selector(HelloWorld::addMonster), 5, kRepeatForever, 0);
    // 检测怪物是否碰到人
    schedule(schedule_selector(HelloWorld::hitByMonster), 0.1f, kRepeatForever, 0);
    //添加背景图片
    TMXTiledMap* tmx = TMXTiledMap::create("background.tmx");
    tmx->setPosition( visibleSize.width / 2, visibleSize.height / 2 );
    tmx->setAnchorPoint( Vec2( 0.5f, 0.5f ) );
    tmx->setScale(Director::getInstance()->getContentScaleFactor());
    this->addChild(tmx, -1);
    // ----------------------------------------------------------------------------------------

    scheduleUpdate();
    return true;
}

void HelloWorld::update(float dt) {
    
    dtime = dtime - 1;
    if (dtime != 0 && dtime%60 == 0) {
        char temp[30] = {};
        sprintf(temp, "Time Left : %d", dtime / 60);
        time->setString(temp);
    }

    if ((player->getPosition().x > visibleSize.width + origin.x - 50 || player->getPosition().y > visibleSize.height + origin.y - 50
        || player->getPosition().x < origin.x + 50 || player->getPosition().y < origin.y + 50) && moveAction != NULL) {
        moveAction->stop();
        if (player->getPosition().x > visibleSize.width + origin.x - 50) {
            player->setPosition(player->getPosition().x - 5, player->getPosition().y);
        }
        if (player->getPosition().y > visibleSize.height + origin.y - 50) {
            player->setPosition(player->getPosition().x, player->getPosition().y - 5);
        }
        if (player->getPosition().x < origin.x + 50) {
            player->setPosition(player->getPosition().x + 5, player->getPosition().y);
        }
        if (player->getPosition().y < origin.y + 50) {
            player->setPosition(player->getPosition().x, player->getPosition().y + 5);
        }
        // moveAction->release(); //为什么不可以呢?
    }
}

void HelloWorld::addMonster(float dt) {
    auto factory = Factory::getInstance();
    auto monster = factory->createMonster();
    float x = random(origin.x, origin.x + visibleSize.width);
    float y = random(origin.y, origin.y + visibleSize.height);
    monster->setPosition(x, y);
    this->addChild(monster, 3);
    // 怪物向角色靠近
    factory->moveMonster(player->getPosition(), 1);
}

void HelloWorld::hitByMonster(float dt) {
    auto factory = Factory::getInstance();
    Sprite* collision = factory->collider(player->getBoundingBox());
    if (collision != NULL) {
        // 怪物碰到角色后，角色掉血,并让怪物消失
        
        if (pT->getScaleX() <= 0) {
            if (delayTime == 0) {
                // 如果角色血扣完了，就播放死亡动画
                auto animation = Animation::createWithSpriteFrames(dead, 0.1f);
                auto animate = Animate::create(animation);
                player->runAction(animate);
            }
            delayTime++;
            if (delayTime > 20) {
                // 游戏停止
                time->setString("Game Over");
                Director::getInstance()->stopAnimation();
            }
        }
        else {
            // 角色掉血
            pT->setScaleX(pT->getScaleX() - 10);
            // 怪物消失
            factory->removeMonster(collision);
        }
    }
}

void HelloWorld::myFlips(char cid) {
    switch (cid)
    {
    case 'w':
        break;
    case 'a':
        if (lastCid != 'a')
        {
            player->setFlippedX(true);
        }
        lastCid = 'a';
    case 's':
        break;
    case 'd':
        if (lastCid != 'd')
        {
            player->setFlippedX(false);
        }
        lastCid = 'd';
    }
}