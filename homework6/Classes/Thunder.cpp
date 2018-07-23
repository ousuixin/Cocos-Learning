#include "Thunder.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Thunder::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Thunder::init() {
	if (!Layer::init()) {
		return false;
	}
	stoneType = 0;
	isMove = false;  // 是否点击飞船
	visibleSize = Director::getInstance()->getVisibleSize();

	// 创建背景
	auto bgsprite = Sprite::create("bg.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
		visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	// 创建飞船
	player = Sprite::create("player.png");
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPosition(visibleSize.width / 2, player->getContentSize().height);
	player->setName("player");
	this->addChild(player, 1);

	// 显示陨石和子弹数量
	enemysNum = Label::createWithTTF("enemys: 0", "fonts/arial.TTF", 20);
	enemysNum->setColor(Color3B(255, 255, 255));
	enemysNum->setPosition(50, 60);
	this->addChild(enemysNum, 3);
	bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
	bulletsNum->setColor(Color3B(255, 255, 255));
	bulletsNum->setPosition(50, 30);
	this->addChild(bulletsNum, 3);

	addEnemy(5);        // 初始化陨石
	preloadMusic();     // 预加载音乐
	playBgm();          // 播放背景音乐
	explosion();        // 创建爆炸帧动画

	// 添加监听器
	addTouchListener();
	addKeyboardListener();
	addCustomListener();

	// 调度器
	schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);

	return true;
}

//预加载音乐文件
void Thunder::preloadMusic() {
	// Todo
}

//播放背景音乐
void Thunder::playBgm() {
	// Todo
    auto audio = SimpleAudioEngine::getInstance();
    audio->playBackgroundMusic("music/bgm.mp3", true);
}

//初始化陨石
void Thunder::addEnemy(int n) {
	enemys.clear();
	for (unsigned i = 0; i < 3; ++i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - i);
		double width = visibleSize.width / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 1), height);
			enemys.push_back(enemy);
			addChild(enemy, 1);
		}
	}
}

// 陨石向下移动并生成新的一行(加分项)
void Thunder::newEnemy() {
	// Todo
    // 首先把陨石往下移动一行
    int flg = 0;
    for (auto k = enemys.begin(); k != enemys.end(); k++) {
        (*k)->setPositionY((*k)->getPositionY() - 50);

        if ((*k)->getPositionY() < 50) {
            flg = 1;
        }
    }

    if (flg == 1) {
        Sprite* gameOver = Sprite::create("gameOver.png");
        gameOver->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        this->addChild(gameOver, 4);

        // 解除所有的监听
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeAllEventListeners();
        
        // 播放帧动画
        auto animation = Animation::createWithSpriteFrames(explore, 0.1f);
        auto animate = Animate::create(animation);

        auto sq = Sequence::create(animate, CallFunc::create([this]() {
            if (player != NULL) {
                player->removeFromParentAndCleanup(true);
            }
            Director::getInstance()->stopAnimation();
        }), nullptr);

        player->runAction(sq);

        auto audio = SimpleAudioEngine::getInstance();
        audio->playEffect("music/explore.wav", false);
    }

    int i = stoneType % 3;
    stoneType++;
    char enemyPath[20];
    sprintf(enemyPath, "stone%d.png", i + 1);
    double width = visibleSize.width / (5 + 1.0),
        height = visibleSize.height - 50;
    for (int j = 0; j < 5; ++j) {
        auto enemy = Sprite::create(enemyPath);
        enemy->setAnchorPoint(Vec2(0.5, 0.5));
        enemy->setScale(0.5, 0.5);
        enemy->setPosition(width * (j + 1) - 85, height);
        enemys.push_back(enemy);
        addChild(enemy, 1);
    }
}

// 移动飞船
void Thunder::movePlane(char c) {
	// Todo
    if (c == 'A') {
        if (player->getBoundingBox().getMinX() < 30) {
            return;
        }
        auto moveToLeft = MoveBy::create(0.1f, Vec2(-10, 0)); // 用0.2s的时间移动-10个像素
        player->runAction(moveToLeft);
    }
    else if (c == 'D') {
        if (player->getBoundingBox().getMaxX() > visibleSize.width - 30) {
            return;
        }
        auto moveToRight = MoveBy::create(0.1f, Vec2(10, 0)); // 用0.2s的时间移动10个像素
        player->runAction(moveToRight);
    }
} 

//发射子弹
void Thunder::fire() {
	auto bullet = Sprite::create("bullet.png");
	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullets.push_back(bullet);
	bullet->setPosition(player->getPosition());
	addChild(bullet, 1);
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);

	// 移除飞出屏幕外的子弹
	// Todo
    auto moveToUp = MoveTo::create(1.0f, Vec2(player->getPosition().x, visibleSize.height + 50));
    auto sq = Sequence::create(moveToUp, CallFunc::create([bullet, this]() {
        if (bullet == NULL) return;
        this->bullets.remove(bullet);
        bullet->removeFromParentAndCleanup(true);
    }), nullptr);
    bullet->runAction(sq);
}

// 切割爆炸动画帧
void Thunder::explosion() {
	// Todo
    auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
    explore.reserve(9);
    for (int i = 0; i < 5; i++) {
        auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(191 * i + 2, 0, 191, 192)));
        explore.pushBack(frame);
    }
    for (int i = 0; i < 4; i++) {
        auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(191 * i + 2, 192, 191, 192)));
        explore.pushBack(frame);
    }
}

void Thunder::update(float f) {

	// 实时更新页面内陨石和子弹数量(不得删除)
	// 要求数量显示正确(加分项)
	char str[15];
	sprintf(str, "enemys: %d", enemys.size());
	enemysNum->setString(str);
	sprintf(str, "bullets: %d", bullets.size());
	bulletsNum->setString(str);

	// 飞船移动
	if (isMove)
		this->movePlane(movekey);

	static int ct = 0;
	static int dir = 4;
	++ct;
	if (ct == 120)
		ct = 40, dir = -dir;
	else if (ct == 80) {
		dir = -dir;
		newEnemy();  // 陨石向下移动并生成新的一行(加分项)
	}
	else if (ct == 20)
		ct = 40, dir = -dir;

	//陨石左右移动
	for (Sprite* s : enemys) {
		if (s != NULL) {
			s->setPosition(s->getPosition() + Vec2(dir, 0));
		}
	}

	// 分发自定义事件
	EventCustom e("meet");
	_eventDispatcher->dispatchEvent(&e);

}

// 自定义碰撞事件
void Thunder::meet(EventCustom * event) {
	// 判断子弹是否打中陨石并执行对应操作
	// Todo
    for (auto i = bullets.begin(); i != bullets.end();) {
        int flag = 0;
        Vec2 pos;
        Sprite* temp1;
        Sprite* temp2;
        for (auto j = enemys.begin(); j != enemys.end();) {
            if ((*i)->getBoundingBox().containsPoint((*j)->getPosition())) {
                flag = 1;
                pos = Vec2((*i)->getPosition());

                temp1 = *i;
                temp2 = *j;

                bullets.remove(*(i++));
                enemys.remove(*j);

                break;
            }
            else {
                j++;
            }
        }

        if (flag == 0) {
            i++;
        }
        else {
            // 播放帧动画
            temp2->setPosition(pos);
            auto animation = Animation::createWithSpriteFrames(explore, 0.1f);
            auto animate = Animate::create(animation);
            
            auto sq = Sequence::create(animate, [temp2]() {
                if (temp2 != NULL) {
                    temp2->removeFromParentAndCleanup(true);
                }
            }, nullptr);

            temp2->runAction(sq);


            if (temp1 != NULL) {
                temp1->removeFromParentAndCleanup(true);
            }

            auto audio = SimpleAudioEngine::getInstance();
            audio->playEffect("music/explore.wav", false);
        }
    }
    // 判断陨石是否碰到飞机
    for (auto k = enemys.begin(); k != enemys.end(); k++) {
        if ((*k)->getBoundingBox().containsPoint(player->getPosition())) {
            Sprite* gameOver = Sprite::create("gameOver.png");
            gameOver->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
            this->addChild(gameOver, 4);

            // 解除所有的监听
            auto dispatcher = Director::getInstance()->getEventDispatcher();
            dispatcher->removeAllEventListeners();

            // 播放帧动画
            auto animation = Animation::createWithSpriteFrames(explore, 0.1f);
            auto animate = Animate::create(animation);

            auto sq = Sequence::create(animate, CallFunc::create([this]() {
                if (player != NULL) {
                    player->removeFromParentAndCleanup(true);
                }
                Director::getInstance()->stopAnimation();
            }), nullptr);

            player->runAction(sq);

            auto audio = SimpleAudioEngine::getInstance();
            audio->playEffect("music/explore.wav", false);
            break;
        }
    }
}

void Thunder::stopAc() {
    // Todo
}

// 添加自定义监听器
void Thunder::addCustomListener() {
	// Todo
    auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithFixedPriority(meetListener, 1);// 意思是优先级比其他事件高一些
}

// 添加键盘事件监听器
void Thunder::addKeyboardListener() {
	// Todo
    auto keyBoardListener = EventListenerKeyboard::create();
    keyBoardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
    keyBoardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(keyBoardListener, player);
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;
	}
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		isMove = false;
		break;
	}
}

// 添加触摸事件监听器
void Thunder::addTouchListener() {
	// Todo
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(touchListener, player);
}

// 鼠标点击发射炮弹
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	if (touch->getLocation().getDistance(player->getPosition()) <= 30)
        isClick = true;
    // Todo
    if (isClick == true) {
        fire();
    }
    return isClick;
}

void Thunder::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;
}

// 当鼠标按住飞船后可控制飞船移动 (加分项)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
	// Todo
    if (isClick == true) {
        if (touch->getLocation().x < visibleSize.width - 30 && touch->getLocation().x > 30 && touch->getLocation().y < visibleSize.height - 30 && touch->getLocation().y > 30)
        player->setPosition(touch->getLocation());
    }
}
