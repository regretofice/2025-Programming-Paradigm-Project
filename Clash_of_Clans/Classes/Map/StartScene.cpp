#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "Map/StartScene.h"

#include "Map/MapScene.h"
#include "Map/MapSelectScene.h"
#include "Tools/AudioManager.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* StartScene::createScene() { return StartScene::create(); }

bool StartScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  _isMuted = false;

  // 背景
  auto background = Sprite::create("start_background.png");
  float bgScaleX = visibleSize.width / background->getContentSize().width;
  float bgScaleY = visibleSize.height / background->getContentSize().height;
  background->setScale(bgScaleX, bgScaleY);
  background->setPosition(Vec2(visibleSize.width / 2 + origin.x,
                               visibleSize.height / 2 + origin.y));
  this->addChild(background, 0);

  // 开始按钮
  auto startButton = ui::Button::create("start_button.png");
  startButton->setScale(0.3f);
  startButton->setPosition(Vec2(visibleSize.width / 2 + origin.x,
                                visibleSize.height / 2 + origin.y + 100));
  startButton->addTouchEventListener(
      [&](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
          menuStartCallback(sender);
        }
      });
  this->addChild(startButton, 1);

  // 退出按钮
  auto exitButton = ui::Button::create("exit_button.png");
  exitButton->setScale(0.8f);
  exitButton->setPosition(Vec2(visibleSize.width / 2 + origin.x,
                               visibleSize.height / 2 + origin.y - 70));
  exitButton->addTouchEventListener(
      [&](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
          menuExitCallback(sender);
        }
      });
  this->addChild(exitButton, 1);

  // 右上角静音按钮
  _muteButton = ui::Button::create("mute_button.png");
  _muteButton->setScale(0.2f);
  _muteButton->setPosition(
      Vec2(visibleSize.width - 50, visibleSize.height - 50));
  _muteButton->addTouchEventListener(
      [&](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
          menuMuteCallback(sender);
        }
      });
  this->addChild(_muteButton, 2);

  return true;
}

void StartScene::onEnter() {
  Scene::onEnter();

  // 进入场景时播放背景音乐
  if (!_isMuted) {
    AudioManager::getInstance()->playBackgroundMusic("start_music.mp3", true);
  }
}

void StartScene::onExit() {
  // 离开场景时停止背景音乐
  AudioManager::getInstance()->stopBackgroundMusic();
  Scene::onExit();
}

void StartScene::menuStartCallback(Ref* pSender) {
  Director::getInstance()->replaceScene(
      TransitionFade::create(0.5f, MapSelectScene::createScene()));
}

void StartScene::menuExitCallback(Ref* pSender) {
  Director::getInstance()->end();
}

void StartScene::menuMuteCallback(Ref* pSender) {
  _isMuted = !_isMuted;

  // 切换静音状态
  AudioManager::getInstance()->setMute(_isMuted);
}
