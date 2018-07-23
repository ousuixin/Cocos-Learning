#include "LoginRegisterScene.h"
#include "ui\CocosGUI.h"
#include "json/document.h"
#include "Utils.h"

#include "json/writer.h"
#include "json/stringbuffer.h"
#include "platform/CCFileUtils.h"
using namespace rapidjson;

USING_NS_CC;
using namespace cocos2d::ui;

cocos2d::Scene * LoginRegisterScene::createScene() {
  return LoginRegisterScene::create();
}

bool LoginRegisterScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto loginButton = MenuItemFont::create("Login", CC_CALLBACK_1(LoginRegisterScene::loginButtonCallback, this));
  if (loginButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + loginButton->getContentSize().height / 2;
    loginButton->setPosition(Vec2(x, y));
  }

  auto registerButton = MenuItemFont::create("Register", CC_CALLBACK_1(LoginRegisterScene::registerButtonCallback, this));
  if (registerButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + registerButton->getContentSize().height / 2 + 100;
    registerButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [] (Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(loginButton, registerButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  usernameInput = TextField::create("username", "arial", 24);
  if (usernameInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    usernameInput->setPosition(Vec2(x, y));
    this->addChild(usernameInput, 1);
  }

  passwordInput = TextField::create("password", "arial", 24);
  if (passwordInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 130.0f;
    passwordInput->setPosition(Vec2(x, y));
    this->addChild(passwordInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 200.0f;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void LoginRegisterScene::loginButtonCallback(cocos2d::Ref * pSender) {
  // Your code here
    HttpRequest * req = new HttpRequest();
    req->setUrl("http://127.0.0.1:8000/auth");
    req->setRequestType(HttpRequest::Type::POST);
    req->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRequestCompleted, this, 1));

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    rapidjson::Value string(rapidjson::kStringType);
    // ÕâÀï¾Þ¿Ó£¬²»ÄÜÓÃobject£¬±ØÐëÓÃstringType
    string.SetString(usernameInput->getString().c_str(), allocator);
    doc.AddMember("username", string, allocator);
    string.SetString(passwordInput->getString().c_str(), allocator);
    doc.AddMember("password", string, allocator);
    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);

    req->setRequestData(buffer.GetString(), buffer.GetSize());
    req->setTag("POST AUTH");
    std::string temp =  FileUtils::getInstance()->getWritablePath() + "cookieFile.txt" ;
    cocos2d::network::HttpClient::getInstance()->enableCookies(NULL);

    std::vector<std::string> headers;
    headers.push_back("");
    req->setHeaders(headers);
    cocos2d::network::HttpClient::getInstance()->send(req);
    req->release();
}

void LoginRegisterScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response, int logOrRegister) {
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
    // ×¢²á/µÇÂ½³É¹¦
    std::string temp1;
    if (d.IsObject() && d.HasMember("status") && d["status"] == true) {
        if (logOrRegister) { // µÇÂ½
            temp1 = "Login Ok";
        }
        else {//×¢²á
            temp1 = "Register Ok";
        }
    }
    // ×¢²á/µÇÂ½Ê§°Ü
    else {
        if (logOrRegister) { // µÇÂ½
            temp1 = "Login Failed";
        }
        else {//×¢²á
            temp1 = "Register Failed";
        }
    }
    messageBox->setString(temp1 + "\n" + d["msg"].GetString());
};

void LoginRegisterScene::registerButtonCallback(Ref * pSender) {
  // Your code here
    HttpRequest * req = new HttpRequest();
    req->setUrl("http://127.0.0.1:8000/users");
    req->setRequestType(HttpRequest::Type::POST);
    req->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRequestCompleted, this, 0));

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    rapidjson::Value string(rapidjson::kStringType);
    string.SetString(usernameInput->getString().c_str(), allocator);
    doc.AddMember("username", string, allocator);
    string.SetString(passwordInput->getString().c_str(), allocator);
    doc.AddMember("password", string, allocator);
    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);

    req->setRequestData(buffer.GetString(), buffer.GetSize());
    req->setTag("POST USERS");
    cocos2d::network::HttpClient::getInstance()->send(req);
    req->release();
}
