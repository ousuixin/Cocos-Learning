#include "UsersInfoScene.h"
#include "json\document.h"
#include "Utils.h"

using namespace rapidjson;

cocos2d::Scene * UsersInfoScene::createScene() {
  return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
  if (!Scene::init()) return false;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
  if (getUserButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + getUserButton->getContentSize().height / 2;
    getUserButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(getUserButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  limitInput = TextField::create("limit", "arial", 24);
  if (limitInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    limitInput->setPosition(Vec2(x, y));
    this->addChild(limitInput, 1);
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

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
  // Your code here
    HttpRequest * req = new HttpRequest();
    req->setUrl("http://127.0.0.1:8000/users?limit=" + limitInput->getString());
    req->setRequestType(HttpRequest::Type::GET);
    req->setResponseCallback(CC_CALLBACK_2(UsersInfoScene::onHttpRequestCompleted, this));

    req->setTag("GET");
    cocos2d::network::HttpClient::getInstance()->send(req);
    req->release();
}

void UsersInfoScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
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
    if (d.IsObject() && d.HasMember("status") && d["status"] == false) {
        std::string temp1 = "Get User Failed\n";
        messageBox->setString(temp1 + d["msg"].GetString());
        return;
    }

    if (d.IsObject() && d.HasMember("data") && d["data"].IsArray()) {
        rapidjson::Value& a = d["data"];
        if (!a.IsArray()) return;
        std::string temp1 = "";
        int num = atoi(limitInput->getString().c_str());
        for (int i = 0; i < ( num < a.Size() ? num : a.Size() ); i++) {
            rapidjson::Value &b = a[i];
            temp1 += "Username: ";
            temp1 += b["username"].GetString();
            temp1 += "\nDeck:\n";
            for (int j = 0; j < b["deck"].Size(); j++) {
                temp1 += " ";
                rapidjson::Value &c = b["deck"][j];
                if (!c.IsObject()) { continue; }
                for (auto iter = c.MemberBegin(); iter != c.MemberEnd(); iter ++) {
                    temp1 += iter->name.GetString();
                    temp1 += ": ";
                    int num = iter->value.GetUint();
                    temp1 += std::to_string(num);
                    temp1 += "\n ";
                }
                temp1 += "---\n";
            }
            temp1 += "---\n";
        }
        messageBox->setString(temp1);
    }
};