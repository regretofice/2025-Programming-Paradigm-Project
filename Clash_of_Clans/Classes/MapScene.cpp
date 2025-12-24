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

Scene* MapScene::createScene() {
  // 默认使用UserDefault中保存的地图，如果没有则使用map2.tmx
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

  // 创建资源显示条（在创建菜单之前，确保在最上层）
  createResourceBars();

  // 创建放置菜单
  createPlacementMenu();

  // 加载已保存的建筑
  loadSavedBuildings();

  auto touchListener = EventListenerTouchOneByOne::create();
  touchListener->onTouchBegan = CC_CALLBACK_2(MapScene::onTouchBegan, this);
  touchListener->onTouchMoved = CC_CALLBACK_2(MapScene::onTouchMoved, this);
  touchListener->onTouchEnded = CC_CALLBACK_2(MapScene::onTouchEnded, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

  // 返回按钮
  auto returnButton = ui::Button::create("button_normal.png");
  returnButton->setScale(0.3f);
  returnButton->setPosition(
      Vec2(visibleSize.width - 80, visibleSize.height - 50));
  returnButton->addTouchEventListener(
      [this](Ref* sender, ui::Widget::TouchEventType type) {
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

  // 注册每帧更新
  this->scheduleUpdate();

  return true;
}

// 创建资源显示条
void MapScene::createResourceBars() {
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 资源条背景面板
  auto resourcePanel = ui::Layout::create();
  resourcePanel->setContentSize(Size(visibleSize.width, 60));
  resourcePanel->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
  resourcePanel->setBackGroundColor(Color3B(40, 40, 40));
  resourcePanel->setOpacity(200);  // 半透明
  resourcePanel->setPosition(
      Vec2(origin.x, origin.y + visibleSize.height - 60));
  this->addChild(resourcePanel, 100);  // 确保在最上层

  // ========== 金币显示 ==========
  // 金币图标
  auto goldIconDraw = DrawNode::create();
  // 绘制金币图标：黄色圆形
  goldIconDraw->drawSolidCircle(Vec2(0, 0), 15, 0, 16, 1, 1,
                                Color4F(1.0f, 0.84f, 0.0f, 1.0f));  // 金色
  // 在圆形中间添加"$"符号
  auto goldText = Label::createWithTTF("$", "fonts/arial.ttf", 20);
  goldText->setColor(Color3B::BLACK);
  goldText->setPosition(Vec2(0, 0));
  goldIconDraw->addChild(goldText);
  goldIconDraw->setPosition(Vec2(100, 30));
  resourcePanel->addChild(goldIconDraw);
  _goldIcon = goldIconDraw;

  // 金币进度条背景
  auto goldBarBackground = DrawNode::create();
  goldBarBackground->drawSolidRect(
      Vec2(0, 0), Vec2(200, 20),
      Color4F(0.2f, 0.2f, 0.2f, 1.0f));  // 深灰色背景
  goldBarBackground->setPosition(Vec2(150, 20));
  resourcePanel->addChild(goldBarBackground);

  // 金币进度条前景
  _goldBarForeground = DrawNode::create();
  _goldBarForeground->drawSolidRect(
      Vec2(0, 0), Vec2(200, 20), Color4F(1.0f, 0.84f, 0.0f, 1.0f));  // 金色前景
  _goldBarForeground->setPosition(Vec2(150, 20));
  resourcePanel->addChild(_goldBarForeground);

  // 金币标签
  _goldLabel = Label::createWithTTF("0/0", "fonts/arial.ttf", 16);
  _goldLabel->setPosition(Vec2(250, 30));
  _goldLabel->setColor(Color3B::WHITE);
  _goldLabel->enableOutline(Color4B::BLACK, 1);
  resourcePanel->addChild(_goldLabel);

  // ========== 圣水显示 ==========
  // 圣水图标
  auto elixirIconDraw = DrawNode::create();
  // 绘制圣水图标：紫色水滴
  elixirIconDraw->drawSolidCircle(
      Vec2(0, 0), 15, 0, 16, 1, 1,
      Color4F(0.58f, 0.0f, 0.83f, 1.0f));  // 紫色圆形
  auto elixirText = Label::createWithTTF("W", "fonts/arial.ttf", 20);
  elixirText->setColor(Color3B::WHITE);
  elixirText->setPosition(Vec2(0, 0));
  elixirIconDraw->addChild(elixirText);
  elixirIconDraw->setPosition(Vec2(visibleSize.width - 350, 30));
  resourcePanel->addChild(elixirIconDraw);
  _elixirIcon = elixirIconDraw;  // 保存引用

  // 圣水进度条背景
  auto elixirBarBackground = DrawNode::create();
  elixirBarBackground->drawSolidRect(
      Vec2(0, 0), Vec2(200, 20),
      Color4F(0.2f, 0.2f, 0.2f, 1.0f));  // 深灰色背景
  elixirBarBackground->setPosition(Vec2(visibleSize.width - 350, 20));
  resourcePanel->addChild(elixirBarBackground);

  // 圣水进度条前景
  _elixirBarForeground = DrawNode::create();
  _elixirBarForeground->drawSolidRect(
      Vec2(0, 0), Vec2(200, 20),
      Color4F(0.58f, 0.0f, 0.83f, 1.0f));  // 紫色前景
  _elixirBarForeground->setPosition(Vec2(visibleSize.width - 350, 20));
  resourcePanel->addChild(_elixirBarForeground);

  // 圣水标签
  _elixirLabel = Label::createWithTTF("0/0", "fonts/arial.ttf", 16);
  _elixirLabel->setPosition(Vec2(visibleSize.width - 250, 30));
  _elixirLabel->setColor(Color3B::WHITE);
  _elixirLabel->enableOutline(Color4B::BLACK, 1);
  resourcePanel->addChild(_elixirLabel);

  // 初始化资源显示
  updateResourceDisplay();
}

// 更新资源显示
void MapScene::updateResourceDisplay() {
  auto playerData = PlayerDataManager::getInstance();

  // 获取当前资源
  int currentGold = playerData->getGold();
  int maxGold = playerData->getGoldLimit();
  int currentElixir = playerData->getElixir();
  int maxElixir = playerData->getElixirLimit();

  // 计算百分比
  float goldPercent = maxGold > 0 ? (currentGold * 100.0f / maxGold) : 0;
  float elixirPercent =
      maxElixir > 0 ? (currentElixir * 100.0f / maxElixir) : 0;

  // 限制百分比在0-100之间
  goldPercent = std::max(0.0f, std::min(100.0f, goldPercent));
  elixirPercent = std::max(0.0f, std::min(100.0f, elixirPercent));

  // 更新进度条 - 清除旧的绘制并绘制新的
  _goldBarForeground->clear();
  _elixirBarForeground->clear();

  // 绘制新的进度条前景
  float goldBarWidth = 200 * (goldPercent / 100.0f);
  float elixirBarWidth = 200 * (elixirPercent / 100.0f);

  _goldBarForeground->drawSolidRect(Vec2(0, 0), Vec2(goldBarWidth, 20),
                                    Color4F(1.0f, 0.84f, 0.0f, 1.0f));
  _elixirBarForeground->drawSolidRect(Vec2(0, 0), Vec2(elixirBarWidth, 20),
                                      Color4F(0.58f, 0.0f, 0.83f, 1.0f));

  // 更新标签文本
  std::string goldText = StringUtils::format("%d/%d", currentGold, maxGold);
  std::string elixirText =
      StringUtils::format("%d/%d", currentElixir, maxElixir);

  _goldLabel->setString(goldText);
  _elixirLabel->setString(elixirText);

  // 根据资源量改变标签颜色
  if (currentGold < maxGold * 0.2f) {
    _goldLabel->setColor(Color3B::RED);
  } else if (currentGold < maxGold * 0.5f) {
    _goldLabel->setColor(Color3B::YELLOW);
  } else {
    _goldLabel->setColor(Color3B::GREEN);
  }

  if (currentElixir < maxElixir * 0.2f) {
    _elixirLabel->setColor(Color3B::RED);
  } else if (currentElixir < maxElixir * 0.5f) {
    _elixirLabel->setColor(Color3B::YELLOW);
  } else {
    _elixirLabel->setColor(Color3B::GREEN);
  }
}

// 更新函数
void MapScene::update(float dt) {
  Scene::update(dt);

  // 每秒更新一次资源显示
  static float timeAccumulator = 0.0f;
  timeAccumulator += dt;

  if (timeAccumulator >= 0.5f) {  // 每0.5秒更新一次
    updateResourceDisplay();
    timeAccumulator = 0.0f;
  }
}

// 创建放置菜单
void MapScene::createPlacementMenu() {
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 菜单背景 - 调整Y位置，避免与资源条重叠
  auto menuPanel = ui::Layout::create();
  menuPanel->setContentSize(
      Size(120, visibleSize.height - 60));  // 减去资源条高度
  menuPanel->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
  menuPanel->setBackGroundColor(Color3B(50, 50, 50));
  menuPanel->setPosition(Vec2(origin.x, origin.y));
  this->addChild(menuPanel, 90);

  auto menuPanel2 = ui::Layout::create();
  menuPanel2->setContentSize(Size(120, visibleSize.height - 60));
  menuPanel2->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
  menuPanel2->setBackGroundColor(Color3B(50, 50, 50));
  menuPanel2->setPosition(Vec2(origin.x + visibleSize.width - 120, origin.y));
  this->addChild(menuPanel2, 90);

  // 调整按钮位置 - 从资源条下方开始
  float startY = visibleSize.height - 80;

  // 建筑按钮1 - 金矿
  auto goldMineBtn = ui::Button::create("gold_mine_icon.png");
  goldMineBtn->setPosition(Vec2(60, startY - 20));
  goldMineBtn->setScale(0.8f);
  goldMineBtn->addClickEventListener([this](Ref* sender) {
    onBuildingButtonClicked(sender, 1);  // 1代表金矿
  });
  menuPanel->addChild(goldMineBtn);

  // 建筑按钮2 - 防御塔
  auto towerBtn = ui::Button::create("tower_icon.png");
  towerBtn->setPosition(Vec2(60, startY - 100));
  towerBtn->setScale(0.8f);
  towerBtn->addClickEventListener([this](Ref* sender) {
    onBuildingButtonClicked(sender, 2);  // 2代表防御塔
  });
  menuPanel->addChild(towerBtn);

  // 士兵按钮1 - 野蛮人
  auto barbarianBtn = ui::Button::create("rarbarian_icon.png");
  barbarianBtn->setPosition(Vec2(60, startY - 20));
  barbarianBtn->setScale(0.8f);
  barbarianBtn->addClickEventListener([this](Ref* sender) {
    onSoldierButtonClicked(sender, 1);  // 1代表野蛮人
  });
  menuPanel2->addChild(barbarianBtn);

  // 士兵按钮3 - 炸弹人
  auto bombermanBtn = ui::Button::create("bomberman_icon.png");
  bombermanBtn->setPosition(Vec2(60, visibleSize.height - 280));
  bombermanBtn->setScale(0.8f);
  bombermanBtn->addClickEventListener([this](Ref* sender) {
    onSoldierButtonClicked(sender, 3);  // 3代表炸弹人
  });
  menuPanel2->addChild(bombermanBtn);

  // 士兵按钮5 - 气球兵
  auto ballonsoldierBtn = ui::Button::create("ballon_soldier_icon.png");
  ballonsoldierBtn->setPosition(Vec2(60, visibleSize.height - 460));
  ballonsoldierBtn->setScale(0.8f);
  ballonsoldierBtn->addClickEventListener([this](Ref* sender) {
    onSoldierButtonClicked(sender, 5);  // 3代表炸弹人
  });

  menuPanel2->addChild(ballonsoldierBtn);
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

// 触摸结束事件
void MapScene::onTouchEnded(Touch* touch, Event* event) {
  // 先处理原有的放置逻辑
  PlacementManager::getInstance()->onTouchBegan(touch, event, this);

  // 检查是否点击了资源建筑进行收集
  auto buildings = BuildingManager::getInstance()->getAllBuildings();
  for (auto building : buildings) {
    if (building && !building->isDestroyed()) {
      // 获取建筑在世界坐标系中的位置和大小
      Vec2 buildingPos = building->getPosition();
      float buildingSize =
          building->getContentSize().width * building->getScale();

      // 将触摸坐标转换为世界坐标
      Vec2 touchPos = touch->getLocation();

      // 检查是否点击在建筑上
      Rect buildingRect(buildingPos.x - buildingSize / 2,
                        buildingPos.y - buildingSize / 2, buildingSize,
                        buildingSize);

      if (buildingRect.containsPoint(touchPos)) {
        // 如果是资源建筑，收集资源
        auto resourceBuilding = dynamic_cast<ResourceBuilding*>(building);
        if (resourceBuilding) {
          int collectedAmount = resourceBuilding->collectResource();
          if (collectedAmount > 0) {
            // 根据资源类型更新玩家数据
            if (resourceBuilding->getResType() == ResourceType::GOLD) {
            } else {
            }
            // 立即更新资源显示
            updateResourceDisplay();
          }
        }
        break;
      }
    }
  }
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
      this->addChild(building, 5);
      building->showHpBar(true);  // 加载建筑时也显示血条
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

    // 更新资源显示
    updateResourceDisplay();
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