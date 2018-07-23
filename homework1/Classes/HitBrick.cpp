#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
  srand((unsigned)time(NULL));
  auto scene = Scene::createWithPhysics();

  scene->getPhysicsWorld()->setAutoStep(true);

  // Debug ģʽ
  // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
  scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
  auto layer = HitBrick::create();
  layer->setPhysicsWorld(scene->getPhysicsWorld());
  layer->setJoint();
  scene->addChild(layer);
  return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }
  visibleSize = Director::getInstance()->getVisibleSize();


  auto edgeSp = Sprite::create();  //����һ������
  auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox�ǲ��ܸ�����ײӰ���һ�ָ��壬����������������������ı߽�
  boundBody->setContactTestBitmask(0x00);
  edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //λ����������Ļ����
  edgeSp->setPhysicsBody(boundBody);
  addChild(edgeSp);


  preloadMusic(); // Ԥ������Ч

  addSprite();    // ��ӱ����͸��־���
  addListener();  // ��Ӽ����� 
  addPlayer();    // ��������
  BrickGeneraetd();  // ����ש��


  schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

  onBall = true;
  spHolded = false;
  spBall = 60;

  spFactor = 0;
  return true;
}

// �ؽ����ӣ��̶��������
// Todo
void HitBrick::setJoint() {
    joint1 = PhysicsJointFixed::construct(
        player->getPhysicsBody(), ball->getPhysicsBody(),player->getPosition()
    );
    m_world->addJoint(joint1);
}



// Ԥ������Ч
void HitBrick::preloadMusic() {
    auto sae = SimpleAudioEngine::getInstance();
    sae->preloadEffect("gameover.mp3");
    sae->preloadBackgroundMusic("bgm.mp3");
    sae->playBackgroundMusic("bgm.mp3", true);
}

// ��ӱ����͸��־���
void HitBrick::addSprite() {
    // add background
    auto bgSprite = Sprite::create("bg.png");
    bgSprite->setPosition(visibleSize / 2);
    bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
    this->addChild(bgSprite, 0);


    // add ship
    ship = Sprite::create("ship.png");
    ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
    ship->setPosition(visibleSize.width / 2, 0);
    auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
    shipbody->setCategoryBitmask(0xFFFFFFFF);
    shipbody->setCollisionBitmask(0xFFFFFFFF);
    shipbody->setContactTestBitmask(0xFFFFFFFF);
    shipbody->setTag(1);
    shipbody->setDynamic(false);  // ??????�I?????????, ????????????��??
    ship->setPhysicsBody(shipbody);
    this->addChild(ship, 1);

    // add sun and cloud
    auto sunSprite = Sprite::create("sun.png");
    sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
    this->addChild(sunSprite);
    auto cloudSprite1 = Sprite::create("cloud.png");
    cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
    this->addChild(cloudSprite1);
    auto cloudSprite2 = Sprite::create("cloud.png");
    cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
    this->addChild(cloudSprite2);
}

// ��Ӽ�����
void HitBrick::addListener() {
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// ������ɫ
void HitBrick::addPlayer() {

    player = Sprite::create("bar.png");
    int xpos = visibleSize.width / 2;

    player->setScale(0.1f, 0.1f);
    player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f));
    // ���ð�ĸ�������
    // Todo
    auto physicsBody = PhysicsBody::createBox(player->getContentSize(), PhysicsMaterial(10, 1.0f, 0.5f));
    physicsBody->setDynamic(false);
    physicsBody->setCategoryBitmask(0xff);
    physicsBody->setCollisionBitmask(0xff);
    physicsBody->setContactTestBitmask(0x00);

    player->setAnchorPoint(Vec2(0.5, 1));
    player->addComponent(physicsBody);

    this->addChild(player, 2);
  
    ball = Sprite::create("ball.png");
    ball->setPosition(Vec2(xpos, player->getPosition().y - 2));
    ball->setScale(0.1f, 0.1f);
    // ������ĸ�������
    // Todo
    physicsBody = PhysicsBody::createCircle(ball->getContentSize().width / 2, PhysicsMaterial(0.1f, 1.0f, 0));
    physicsBody->setCategoryBitmask(0xff);
    physicsBody->setCollisionBitmask(0xff);
    physicsBody->setContactTestBitmask(0xff);

    ball->setAnchorPoint(Vec2(0.5f, 0));
    ball->addComponent(physicsBody);

    addChild(ball, 3);
  
}

// ʵ�ּ򵥵�����Ч��
// Todo
void HitBrick::update(float dt) {
    if (player->getBoundingBox().getMinX() <= 0 || player->getBoundingBox().getMaxX() >= visibleSize.width) {
        spFactor = 0;
        if (player->getBoundingBox().getMinX() <= 0)
            player->setPositionX(player->getPositionX() + 1);
        if (player->getBoundingBox().getMaxX() >= visibleSize.width)
            player->setPositionX(player->getPositionX() - 1);
    }
    player->getPhysicsBody()->setVelocity(Vec2(spFactor*3, 0));

    if (onBall) {
        if (spHolded) {
            spBall++;
        }
        else if (spHolded == false && spBall > 60) {
            m_world->removeJoint(joint1);
            ball->getPhysicsBody()->setVelocity(Vec2(0, spBall*10));
            onBall = false;
        }
    }

}




// Todo
void HitBrick::BrickGeneraetd() {

    for (int i = 0; i < 3; i++) {
	    int cw = 0;
	    while (cw <= visibleSize.width) {
		    auto box = Sprite::create("box.png");
            box->setPosition(Vec2(cw + box->getContentSize().width/2 - 5, visibleSize.height - i*box->getContentSize().height - box->getContentSize().height/2));
            cw += box->getContentSize().width;
		    // Ϊש�����ø�������
		    // Todo
            auto physicsBody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(0.1, 1.0f, 0.5f));
            physicsBody->setDynamic(false);
            physicsBody->setCategoryBitmask(0xff);
            physicsBody->setCollisionBitmask(0xff);
            physicsBody->setContactTestBitmask(0xff);

            box->addComponent(physicsBody);
            box->setName("box");
            this->addChild(box, 2);
	    }

     }

}


// ����
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
      if (player->getBoundingBox().getMinX() > 0) {
          spFactor = -200;
      }
      else {
          spFactor = 0;
      }
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // �����ƶ�
    // Todo
      if (player->getBoundingBox().getMaxX() < visibleSize.width) {
          spFactor = 200;
      }
      else {
          spFactor = 0;
      }

    break;

  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // ��ʼ����
      spHolded = true;
      break;
  default:
    break;
  }
}

// �ͷŰ���
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
      spFactor = 0;
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // ֹͣ�˶�
    // Todo
      spFactor = 0;
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   // ����������С����
      spHolded = false;
    break;

  default:
    break;
  }
}

// ��ײ���
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
  auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
  auto sp1 = (Sprite*)c1->getBody()->getNode(), sp2 = (Sprite*)c2->getBody()->getNode();
  if (sp1 == ball) {
      if (sp2 == ship) {
          GameOver();
          spFactor = 0;
          ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
      }
      else if (sp2->getName() == "box") {
          ParticleSun* fireworks;
          auto tempAni = MoveTo::create(0.02, sp2->getPosition());
          Vec2 posTemp = sp2->getPosition();
          auto sq = Sequence::create(tempAni, CallFunc::create([sp2]() {
              sp2->removeFromParentAndCleanup(true);
          }), CallFunc::create([this, &fireworks, posTemp]() {
              fireworks = ParticleSun::create();
              addChild(fireworks, 5);
              fireworks->setPosition(posTemp);
              auto tempAni1 = MoveTo::create(0.5f, posTemp);
              auto sq1 = Sequence::create(tempAni1, CallFunc::create([fireworks]() {
                        fireworks->removeFromParentAndCleanup(true);
                    }), nullptr
              );
              fireworks->runAction(sq1);
          }), nullptr);
          sp2->runAction(sq);
      } 
  }
  return true;
}


void HitBrick::GameOver() {

	_eventDispatcher->removeAllEventListeners();
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
  SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
  SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

  auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
  label1->setColor(Color3B(0, 0, 0));
  label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  this->addChild(label1);

  auto label2 = Label::createWithTTF("����", "fonts/STXINWEI.TTF", 40);
  label2->setColor(Color3B(0, 0, 0));
  auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  auto label3 = Label::createWithTTF("�˳�", "fonts/STXINWEI.TTF", 40);
  label3->setColor(Color3B(0, 0, 0));
  auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);
}

// ���������水ť��Ӧ����
void HitBrick::replayCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(HitBrick::createScene());
}

// �˳�
void HitBrick::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}
