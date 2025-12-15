#include "MapScene.h"
#include "StartScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* MapScene::createScene()
{
    return MapScene::create();
}

bool MapScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建地图
    createMap();

    // 返回按钮
    auto returnButton = ui::Button::create("button_normal.png");
    returnButton->setScale(0.3f);
    returnButton->setPosition(Vec2(visibleSize.width - 80, visibleSize.height - 50));
    returnButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            menuReturnCallback(sender);
        }
        });
    this->addChild(returnButton, 100);
    return true;
}

void MapScene::createMap()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 加载TMX地图文件
    _tileMap = TMXTiledMap::create("map2.tmx");

    if (!_tileMap)
    {
        // 如果地图文件不存在，显示错误信息
        auto errorLabel = Label::createWithTTF("地图文件加载失败", "fonts/Marker Felt.ttf", 36);
        errorLabel->setColor(Color3B::RED);
        errorLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(errorLabel, 10);
        return;
    }

    // 获取地图信息
    _mapWidth = static_cast<int>(_tileMap->getMapSize().width);
    _mapHeight = static_cast<int>(_tileMap->getMapSize().height);
    _tileSize = static_cast<int>(_tileMap->getTileSize().width);

    // 计算地图居中需要的偏移量
    float mapTotalWidth = _tileMap->getContentSize().width;
    float mapTotalHeight = _tileMap->getContentSize().height;

    // 计算居中位置
    _mapOffsetX =-750 + origin.x + (visibleSize.width - mapTotalWidth) / 2.0f;
    _mapOffsetY =-750+ origin.y + (visibleSize.height - mapTotalHeight) / 2.0f;
    
    // 设置TileMap的位置使其居中
    _tileMap->setPosition(Vec2(_mapOffsetX, _mapOffsetY));
    _tileMap->setScale(2.0f);
    this->addChild(_tileMap, 0);
}
void MapScene::menuReturnCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, StartScene::createScene()));
}