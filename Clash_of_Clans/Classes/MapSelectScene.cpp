#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "MapSelectScene.h"
#include "StartScene.h"
#include "MapScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* MapSelectScene::createScene()
{
    return MapSelectScene::create();
}

bool MapSelectScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 背景
    auto background = Sprite::create("select_background.png");
    if (!background) {
        background = Sprite::create("start_background.png"); // 如果没有选择背景，用开始背景
    }

    float bgScaleX = visibleSize.width / background->getContentSize().width;
    float bgScaleY = visibleSize.height / background->getContentSize().height;
    background->setScale(bgScaleX, bgScaleY);
    background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(background, 0);

    // 标题
    auto titleLabel = Label::createWithSystemFont("选择地图", "Microsoft YaHei", 72);
    titleLabel->setTextColor(Color4B::YELLOW);
    titleLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x,
        visibleSize.height - 100));
    this->addChild(titleLabel, 1);

    // 地图1选择按钮
    auto map1Button = ui::Button::create("preview1.png");
    if (!map1Button) {
        map1Button = ui::Button::create("button_normal.png");
        auto map1Label = Label::createWithSystemFont("地图1", "Microsoft YaHei", 36);
        map1Label->setPosition(map1Button->getContentSize() / 2);
        map1Button->addChild(map1Label);
    }

    map1Button->setScale(0.3f);
    map1Button->setPosition(Vec2(visibleSize.width / 2 - 200, visibleSize.height / 2 + 50));
    map1Button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            selectMap1Callback(sender);
        }
        });
    this->addChild(map1Button, 1);

    // 地图1名称标签
    auto map1Name = Label::createWithSystemFont("沙漠地图", "Microsoft YaHei", 36);
    map1Name->setPosition(Vec2(visibleSize.width / 2 - 200, visibleSize.height / 2 - 100));
    this->addChild(map1Name, 1);

    // 地图2选择按钮
    auto map2Button = ui::Button::create("preview2.png");
    if (!map2Button) {
        map2Button = ui::Button::create("button_normal.png");
        auto map2Label = Label::createWithSystemFont("地图2", "Microsoft YaHei", 36);
        map2Label->setPosition(map2Button->getContentSize() / 2);
        map2Button->addChild(map2Label);
    }

    map2Button->setScale(0.3f);
    map2Button->setPosition(Vec2(visibleSize.width / 2 + 200, visibleSize.height / 2 + 50));
    map2Button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            selectMap2Callback(sender);
        }
        });
    this->addChild(map2Button, 1);

    // 地图2名称标签
    auto map2Name = Label::createWithSystemFont("丛林地图", "Microsoft YaHei", 36);
    map2Name->setPosition(Vec2(visibleSize.width / 2 + 200, visibleSize.height / 2 - 100));
    this->addChild(map2Name, 1);

    // 返回按钮
    auto returnButton = ui::Button::create("return_button.png");
    if (!returnButton) {
        returnButton = ui::Button::create("button_normal.png");
        auto returnLabel = Label::createWithSystemFont("返回", "Microsoft YaHei", 24);
        returnLabel->setPosition(returnButton->getContentSize() / 2);
        returnButton->addChild(returnLabel);
    }

    returnButton->setScale(0.3f);
    returnButton->setPosition(Vec2(100, 100));
    returnButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            menuReturnCallback(sender);
        }
        });
    this->addChild(returnButton, 1);

    return true;
}

void MapSelectScene::selectMap1Callback(Ref* pSender)
{
    auto userDefault = UserDefault::getInstance();
    userDefault->setStringForKey("selected_map", "map1.tmx"); 
    userDefault->flush();

    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MapScene::createScene()));
}

void MapSelectScene::selectMap2Callback(Ref* pSender)
{
    auto userDefault = UserDefault::getInstance();
    userDefault->setStringForKey("selected_map", "map2.tmx"); 
    userDefault->flush();

    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MapScene::createScene()));
}

void MapSelectScene::menuReturnCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, StartScene::createScene()));
}