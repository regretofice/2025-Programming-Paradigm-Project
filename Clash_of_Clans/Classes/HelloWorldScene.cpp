// 设置中文能够正常使用
#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "HelloWorldScene.h"

#include "PlayerDataManager.h"
USING_NS_CC;

Scene* HelloWorld::createScene() { return HelloWorld::create(); }

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename) {
  printf("Error while loading: %s\n", filename);
  printf(
      "Depending on how you compiled you might have to add 'Resources/' in "
      "front of filenames in HelloWorldScene.cpp\n");
}

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

  // 修改金币（+50）
  PlayerData->setGold(currentGold + 50);
  // 修改圣水（+20）
  PlayerData->setElixir(currentElixir + 20);

  PlayerData->resetAllData();
  // 显示金币
  auto goldLabel = Label::createWithSystemFont(
      "金币：" + std::to_string(PlayerData->getGold()), "Microsoft YaHei", 50);
  goldLabel->setPosition(visibleSize.width / 2, visibleSize.height - 50);
  goldLabel->setTextColor(Color4B::YELLOW);
  this->addChild(goldLabel, 2);
  /////////////////////////////
  // 2. add a menu item with "X" image, which is clicked to quit the program
  //    you may modify it.

  // add a "close" icon to exit the progress. it's an autorelease object
  // auto closeItem =
  //    MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
  //                          CC_CALLBACK_1(HelloWorld::menuCloseCallback,
  //                          this));

  // if (closeItem == nullptr || closeItem->getContentSize().width <= 0 ||
  //     closeItem->getContentSize().height <= 0) {
  //   problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
  // } else {
  //   float x =
  //       origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
  //   float y = origin.y + closeItem->getContentSize().height / 2;
  //   closeItem->setPosition(Vec2(x, y));
  // }

  //// create menu, it's an autorelease object
  // auto menu = Menu::create(closeItem, NULL);
  // menu->setPosition(Vec2::ZERO);
  // this->addChild(menu, 1);

  ///////////////////////////////
  //// 3. add your codes below...

  //// add a label shows "Hello World"
  //// create and initialize a label

  // auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf",
  // 24); if (label == nullptr) {
  //   problemLoading("'fonts/Marker Felt.ttf'");
  // } else {
  //   // position the label on the center of the screen
  //   label->setPosition(
  //       Vec2(origin.x + visibleSize.width / 2,
  //            origin.y + visibleSize.height -
  //            label->getContentSize().height));

  //  // add the label as a child to this layer
  //  this->addChild(label, 1);
  //}

  //// add "HelloWorld" splash screen"
  // auto sprite = Sprite::create("HelloWorld.png");
  // if (sprite == nullptr) {
  //   problemLoading("'HelloWorld.png'");
  // } else {
  //   // position the sprite on the center of the screen
  //   sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x,
  //                            visibleSize.height / 2 + origin.y));

  //  // add the sprite as a child to this layer
  //  this->addChild(sprite, 0);
  //}
  return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender) {
  // Close the cocos2d-x game scene and quit the application
  Director::getInstance()->end();

  /*To navigate back to native iOS screen(if present) without quitting the
   * application  ,do not use Director::getInstance()->end() as given
   * above,instead trigger a custom event created in RootViewController.mm as
   * below*/

  // EventCustom customEndEvent("game_scene_close_event");
  //_eventDispatcher->dispatchEvent(&customEndEvent);
}
