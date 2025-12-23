#include "MapScene.h"

#include "BuildingManager.h"
#include "DefenseBuilding.h"
#include "GroundRarbarian.h"
#include "PlacementManager.h"
#include "PlayerDataManager.h"
#include "ResourceBuilding.h"
#include "StartScene.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* MapScene::
    createScene() {  // 默认使用UserDefault中保存的地图，如果没有则使用map2.tmx
  auto userDefault = UserDefault::getInstance();
  std::string mapFile =
      userDefault->getStringForKey("selected_map", "map2.tmx");

  return createSceneWithMap(mapFile);
}

Scene* MapScene::createSceneWithMap(const std::string& mapFile) {
  auto scene = Scene::create();
  auto layer = MapScene::create();
  if (layer) {
    layer->_mapFileName = mapFile;
    scene->addChild(layer);
  }
  return scene;
}
void MapScene::createMap() {
  // 加载TMX地图文件
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 如果_mapFileName为空，则从UserDefault获取
  if (_mapFileName.empty()) {
    auto userDefault = UserDefault::getInstance();
    _mapFileName = userDefault->getStringForKey("selected_map", "map2.tmx");
  }

  // 加载TMX地图文件
  _tileMap = TMXTiledMap::create(_mapFileName);
  // 获取地图信息

  _mapWidth = static_cast<int>(_tileMap->getMapSize().width);
  _mapHeight = static_cast<int>(_tileMap->getMapSize().height);
  _tileSize = static_cast<int>(_tileMap->getTileSize().width);

  // 计算地图居中需要的偏移量
  float mapTotalWidth = _tileMap->getContentSize().width;
  float mapTotalHeight = _tileMap->getContentSize().height;

  // 计算居中位置
  _mapOffsetX = -750 + origin.x + (visibleSize.width - mapTotalWidth) / 2.0f;
  _mapOffsetY = -750 + origin.y + (visibleSize.height - mapTotalHeight) / 2.0f;

  // 设置TileMap的位置使其居中
  _tileMap->setPosition(Vec2(_mapOffsetX, _mapOffsetY));
  _tileMap->setScale(2.0f);
  this->addChild(_tileMap, 0);

  // 初始化放置管理器的网格信息
  PlacementManager::getInstance()->initGrid(_mapWidth, _mapHeight, _tileSize,
                                            _mapOffsetX, _mapOffsetY, 2.0f);
}
void MapScene::menuReturnCallback(Ref* pSender) {
  Director::getInstance()->replaceScene(
      TransitionFade::create(0.5f, StartScene::createScene()));
}

bool MapScene::init() {
  if (!Scene::init()) {
    return false;
  }
  if (!Scene::initWithPhysics()) {
    return false;
  }
  auto physicsWorld = getPhysicsWorld();
  physicsWorld->setGravity(Vec2(0, 0));
  PlacementManager::getInstance()->initGrid(20, 20, 64.0f, 0, 0, 1.0f);

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 创建地图
  createMap();

  // 创建放置菜单
  createPlacementMenu();

  // 加载已保存的建筑
  loadSavedBuildings();

  auto touchListener = EventListenerTouchOneByOne::create();
  touchListener->onTouchBegan = CC_CALLBACK_2(MapScene::onTouchBegan, this);
  touchListener->onTouchMoved = CC_CALLBACK_2(MapScene::onTouchMoved, this);
  touchListener->onTouchEnded = CC_CALLBACK_2(MapScene::onTouchEnded, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

  // 返回按钮（保留原有代码）
  auto returnButton = ui::Button::create("button_normal.png");
  returnButton->setScale(0.3f);
  returnButton->setPosition(
      Vec2(visibleSize.width - 80, visibleSize.height - 50));
  returnButton->addTouchEventListener(
      [&](Ref* sender, ui::Widget::TouchEventType type) {
        auto pm = PlacementManager::getInstance();
        pm->clearAll();
        if (type == ui::Widget::TouchEventType::ENDED) {
          // 保存建筑位置
          saveBuildingsPosition();
          menuReturnCallback(sender);
        }
      });
  this->addChild(returnButton, 100);

  _previewMapPosition = Vec2::ZERO;  // 初始化预览位置

  SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
      "rarbarian_idle.plist");
  CCLOG("Loaded rarbarian.plist");
  return true;
}

// 创建放置菜单
void MapScene::createPlacementMenu() {
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 菜单背景
  auto menuPanel = ui::Layout::create();
  menuPanel->setContentSize(Size(120, visibleSize.height));
  menuPanel->setBackGroundColorType(
      ui::Layout::BackGroundColorType::SOLID);         // 设置背景类型为纯色
  menuPanel->setBackGroundColor(Color3B(50, 50, 50));  // 修正函数名大小写
  menuPanel->setPosition(Vec2(origin.x, origin.y));
  this->addChild(menuPanel, 90);

  auto menuPanel2 = ui::Layout::create();
  menuPanel2->setContentSize(Size(120, visibleSize.height));
  menuPanel2->setBackGroundColorType(
      ui::Layout::BackGroundColorType::SOLID);          // 设置背景类型为纯色
  menuPanel2->setBackGroundColor(Color3B(50, 50, 50));  // 修正函数名大小写
  menuPanel2->setPosition(Vec2(origin.x + 1800, origin.y));
  this->addChild(menuPanel2, 90);
  // 建筑按钮1 - 金矿
  auto goldMineBtn = ui::Button::create("gold_mine_icon.png");
  goldMineBtn->setPosition(Vec2(60, visibleSize.height - 60));
  goldMineBtn->setScale(0.8f);
  goldMineBtn->addClickEventListener([this](Ref* sender) {
    onBuildingButtonClicked(sender, 1);  // 1代表金矿
  });
  menuPanel->addChild(goldMineBtn);

  // 建筑按钮2 - 防御塔
  auto towerBtn = ui::Button::create("tower_icon.png");
  towerBtn->setPosition(Vec2(60, visibleSize.height - 140));
  towerBtn->setScale(0.8f);
  towerBtn->addClickEventListener([this](Ref* sender) {
    onBuildingButtonClicked(sender, 2);  // 2代表防御塔
  });
  menuPanel->addChild(towerBtn);

  // 士兵按钮1 - 野蛮人
  auto rarbarianBtn = ui::Button::create("rarbarian_icon.png");
  rarbarianBtn->setPosition(Vec2(60, visibleSize.height - 220));
  rarbarianBtn->setScale(0.8f);
  rarbarianBtn->addClickEventListener([this](Ref* sender) {
    onSoldierButtonClicked(sender, 1);  // 1代表野蛮人
  });
  menuPanel2->addChild(rarbarianBtn);

  // 取消按钮
  auto cancelBtn = ui::Button::create("cancel_icon.png");
  cancelBtn->setPosition(Vec2(60, 60));
  cancelBtn->setScale(0.8f);
  cancelBtn->addClickEventListener([this](Ref* sender) { cancelPlacement(); });
  menuPanel->addChild(cancelBtn);
}

// 建筑按钮点击事件
void MapScene::onBuildingButtonClicked(Ref* sender, int buildingType) {
  auto pm = PlacementManager::getInstance();
  pm->startPlaceBuilding(buildingType);
  pm->createPreviewSprite(buildingType, 0, this);
}

// 士兵按钮点击事件
void MapScene::onSoldierButtonClicked(Ref* sender, int soldierType) {
  auto pm = PlacementManager::getInstance();
  pm->startPlaceSoldier(soldierType);
  pm->createPreviewSprite(0, soldierType, this);
  CCLOG("Soldier button clicked: %d", soldierType);
}
// 触摸开始事件 - 放置建筑或士兵
bool MapScene::onTouchBegan(Touch* touch, Event* event) {
  // 交给PlacementManager处理开始触摸
  return true;
}

// 触摸移动事件 - 更新预览图位置
void MapScene::onTouchMoved(Touch* touch, Event* event) {
  PlacementManager::getInstance()->onTouchMoved(touch, event, this);
}

void MapScene::onTouchEnded(Touch* touch, Event* event) {
  PlacementManager::getInstance()->onTouchBegan(touch, event, this);
}
// 取消放置状态
void MapScene::cancelPlacement() {
  PlacementManager::getInstance()->cancelPlacement(this);
}

// 保存建筑位置到存档
void MapScene::saveBuildingsPosition() {
  auto buildingManager = BuildingManager::getInstance();
  auto allBuildings = buildingManager->getAllBuildings();
  auto playerData = PlayerDataManager::getInstance();

  // 清空之前保存的建筑数据
  playerData->clearBuildingData();

  // 保存每个建筑的数据
  for (auto building : allBuildings) {
    if (!building->isDestroyed()) {
      BuildingData data;
      data.type = (int)building->getType();
      data.positionX = building->getPositionX();
      data.positionY = building->getPositionY();
      data.level = building->getLevel();
      data.name = building->getName();
      playerData->addBuildingData(data);
    }
  }

  // 保存数据
  playerData->saveData();
}

// 加载保存的建筑
void MapScene::loadSavedBuildings() {
  auto playerData = PlayerDataManager::getInstance();
  auto buildingDatas = playerData->getAllBuildingData();

  for (auto& data : buildingDatas) {
    Building* building = nullptr;

    // 根据建筑类型创建对应的建筑
    if (data.type == (int)BuildingType::RESOURCE) {
      building = ResourceBuilding::create(
          "gold_mine_icon.png", data.name, CampType::PLAYER, data.level, 3,
          100 * data.level, 60, 100 * data.level, 5.0f, ResourceType::GOLD,
          10 * data.level, 500 * data.level);
    } else if (data.type == (int)BuildingType::DEFENSE) {
      building = DefenseBuilding::create(
          "tower_icon.png", data.name, CampType::PLAYER, data.level, 3,
          200 * data.level, 50, 150 * data.level, 8.0f, 10 * data.level, 150,
          AttackType::SINGLE_TARGET, 2.0f);
    }

    if (building) {
      building->setPosition(Vec2(data.positionX, data.positionY));
    }
  }
}
// 在指定位置生成士兵
void MapScene::spawnSoldierAtPosition(SoldierType type, const Vec2& pos) {
  // 检查资源是否足够（例如消耗圣水，根据实际需求添加）
  if (!checkResourceEnough(type)) {
    CCLOG("资源不足，无法生成士兵");
    return;
  }

  Soldier* soldier = nullptr;
  switch (type) {
    case SoldierType::kGroundRarbarian:
      soldier = Rarbarian::create(100, 20, 50, 2);
      break;
    // 其他类型以后再加
    default:
      CCLOG("Unknown soldier type");
      return;
  }
  if (soldier) {
    // 设置士兵位置为点击位置
    soldier->setPosition(pos);
    this->addChild(soldier, 10);                // 添加到场景
    soldier->changeState(SoldierState::kIdle);  // 初始化为 idle 状态

    // 扣除资源（根据实际需求添加）
    consumeResource(type);
  }
}

// 检查资源是否足够（示例）
bool MapScene::checkResourceEnough(SoldierType type) {
  // 假设生成野蛮人需要50圣水
  if (type == SoldierType::kGroundRarbarian) {
    return PlayerDataManager::getInstance()->getElixir() >= 50;
  }
  return false;
}

// 消耗资源（示例）
void MapScene::consumeResource(SoldierType type) {
  if (type == SoldierType::kGroundRarbarian) {
    PlayerDataManager::getInstance()->setElixir(
        PlayerDataManager::getInstance()->getElixir() - 50);
  }
}

void MapScene::onEnter() { Scene::onEnter(); }