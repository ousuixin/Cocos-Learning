#include "ModifyUserScene.h"
#include "Utils.h"
#include "json/document.h"

#include "json/writer.h"
#include "json/stringbuffer.h"
#include "json/document.h"

cocos2d::Scene * ModifyUserScene::createScene() {
  return ModifyUserScene::create();
}

bool ModifyUserScene::init() {
  if (!Scene::init()) return false;
  
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto postDeckButton = MenuItemFont::create("Post Deck", CC_CALLBACK_1(ModifyUserScene::putDeckButtonCallback, this));
  if (postDeckButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + postDeckButton->getContentSize().height / 2;
    postDeckButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(postDeckButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  deckInput = TextField::create("Deck json here", "arial", 24);
  if (deckInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    deckInput->setPosition(Vec2(x, y));
    this->addChild(deckInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void ModifyUserScene::putDeckButtonCallback(Ref * pSender) {
  // Your code here
    HttpRequest * req = new HttpRequest();
    req->setUrl("http://127.0.0.1:8000/users");
    req->setRequestType(HttpRequest::Type::PUT);
    req->setResponseCallback(CC_CALLBACK_2(ModifyUserScene::onHttpRequestCompleted, this));

    std::string temp = "{\"deck\":" + deckInput->getString() + "}";
    req->setRequestData(temp.c_str(), temp.size());
    req->setTag("PUT USERS");

    std::vector<std::string> headers;
    headers.push_back("");
    req->setHeaders(headers);
    cocos2d::network::HttpClient::getInstance()->send(req);
    req->release();
}

void ModifyUserScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
    if (!response) {
        return;
    }
    if (!response->isSucceed()) {
        log("response failed");
        log("error buffer: %s", response->getErrorBuffer());
        return;
    }
    std::vector<char> *buffer = response->getResponseData();
    std::string temp;
    temp.assign(buffer->begin(), buffer->end());

    rapidjson::Document d;
    d.Parse<0>(temp.c_str());
    if (d.HasParseError()) {
        log("Parse failed %s", d.GetParseError());
        return;
    }
    std::string temp1;
    if (d.IsObject() && d.HasMember("status") && d["status"] == true) {
        temp1 = "Put Ok";
    }
    else {
        temp1 = "Put Failed";
    }
    messageBox->setString(temp1 + "\n" + d["msg"].GetString());
};