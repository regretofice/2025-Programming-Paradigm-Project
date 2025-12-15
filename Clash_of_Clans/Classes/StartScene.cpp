#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "StartScene.h"
#include "MapScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* StartScene::createScene()
{
    return StartScene::create();
}

bool StartScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 背景
    auto background = Sprite::create("start_background.png"); 
    float bgScaleX = visibleSize.width / background->getContentSize().width;
    float bgScaleY = visibleSize.height / background->getContentSize().height;
    background->setScale(bgScaleX, bgScaleY);
    background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(background, 0);

    // 开始按钮 
    auto startButton = ui::Button::create("start_button.png");
    startButton->setScale(0.3f);
    startButton->setPosition(Vec2(visibleSize.width / 2 + origin.x,
        visibleSize.height / 2 + origin.y + 100)); // 调整位置，让两个按钮分开
    startButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            menuStartCallback(sender);
        }
        });
    this->addChild(startButton, 1);

    // 退出按钮
    auto exitButton = ui::Button::create("exit_button.png");
    exitButton->setScale(0.8f);
    exitButton->setPosition(Vec2(visibleSize.width / 2 + origin.x,
        visibleSize.height / 2 + origin.y - 70)); 
    exitButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            menuExitCallback(sender);
        }
        });
    this->addChild(exitButton, 1);
    return true;
}

void StartScene::menuStartCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MapScene::createScene()));
}

void StartScene::menuExitCallback(Ref* pSender)
{
    Director::getInstance()->end();
}