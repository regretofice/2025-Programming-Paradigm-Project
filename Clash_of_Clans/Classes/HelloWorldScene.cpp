// 设置中文能够正常使用
#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "HelloWorldScene.h"
#include "PlayerDataManager.h"
USING_NS_CC;



Scene* HelloWorld::createScene() { return HelloWorld::create(); }


// on "init" you need to initialize your instance
bool HelloWorld::init() {
  //////////////////////////////
  // 1. super init first
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto PlayerData = PlayerDataManager::getInstance();

  // 读取当前金币/水晶（程序启动时自动加载上次保存的值）

  int currentGold = PlayerData->getGold();
  int currentElixir = PlayerData->getElixir();
  int currentBuilder = PlayerData->getBuilder();
  CCLOG("当前金币：%d,当前圣水：%d,当前建筑工人", currentGold, currentElixir,
        currentBuilder);
  // PlayerData->resetAllData();
  //  修改金币（+50）
  PlayerData->setGold(currentGold + 50);
  // 修改圣水（+20）
  // PlayerData->setElixir(currentElixir + 20);

  // PlayerData->resetAllData();
  //    显示金币
  auto goldLabel = Label::createWithSystemFont(
      "圣水：" + std::to_string(PlayerData->getElixir()), "Microsoft YaHei",
      50);
  goldLabel->setPosition(visibleSize.width / 2, visibleSize.height - 50);
  goldLabel->setTextColor(Color4B::YELLOW);
  this->addChild(goldLabel, 10);

  this->schedule(
      [=](float dt) {
        int currentElixir_ = PlayerDataManager::getInstance()->getElixir();
        goldLabel->setString("圣水：" + std::to_string(currentElixir_));
        CCLOG("UI定时器触发，当前圣水：%d", currentElixir_);  // 新增日志
      },
      0.5f, "elixirUpdate");

  return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender) {
  Director::getInstance()->end();
}
