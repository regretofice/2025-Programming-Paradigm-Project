
#include "PlacementManager.h"

#include "AirBallonSoldier.h"
#include "DefenseBuilding.h"
#include "GroundArcher.h"
#include "GroundGiant.h"
#include "GroundRarbarian.h"
#include "Groundbomberman.h"
#include "PlayerDataManager.h"
#include "ResourceBuilding.h"
#include "ResourceStorageBuilding.h"

USING_NS_CC;

float PlacementManager::getSoldierRadius(int soldierType) const {
  switch (soldierType) {
    case 1:
      return 30.0f;  // GroundRarbarian
    case 2:
      return 25.0f;  // GroundGiant
    case 3:
      return 20.0f;  // GroundBomberman
    case 4:
      return 22.0f;  // GroundArcher
    case 5:
      return 28.0f;  // AirThunderDragon
    case 6:
      return 26.0f;  // AirBallonSoldier
    default:
      return 25.0f;
  }
}

PlacementManager* PlacementManager::s_instance = nullptr;

PlacementManager* PlacementManager::getInstance() {
  if (!s_instance) s_instance = new (std::nothrow) PlacementManager();
  return s_instance;
}
void PlacementManager::destroyInstance() { CC_SAFE_DELETE(s_instance); }

PlacementManager::PlacementManager() {}
PlacementManager::~PlacementManager() {}

void PlacementManager::initGrid(int mapWidth, int mapHeight, float tileSize,
                                float mapOffsetX, float mapOffsetY,
                                float mapScale) {
  _mapWidth = mapWidth;
  _mapHeight = mapHeight;
  _tileSize = tileSize;
  _mapOffsetX = mapOffsetX;
  _mapOffsetY = mapOffsetY;
  _mapScale = mapScale;

  _gridOccupied.assign(_mapWidth, std::vector<bool>(_mapHeight, false));
}

Vec2 PlacementManager::worldToTile(const Vec2& worldPos) const {
  // 正确反缩放
  Vec2 mapPos = (worldPos - Vec2(_mapOffsetX, _mapOffsetY)) / _mapScale;
  float tileX = mapPos.x / _tileSize;
  float tileY = mapPos.y / _tileSize;
  CCLOG("worldToTile: world(%.1f,%.1f) -> tile(%.1f,%.1f)", worldPos.x,
        worldPos.y, tileX, tileY);  // 调试日志
  return Vec2(tileX, tileY);
}

Vec2 PlacementManager::tileToWorldCenter(float tileX, float tileY) const {
  // tile -> map（格子中心）
  Vec2 mapCenter((tileX + 0.5f) * _tileSize, (tileY + 0.5f) * _tileSize);
  // map -> world
  Vec2 worldPos = mapCenter * _mapScale + Vec2(_mapOffsetX, _mapOffsetY);
  return worldPos;
}

bool PlacementManager::canPlaceAt(int baseX, int baseY, int sizeInTiles) const {
  int padding = 0;
  int minX = baseX - padding;
  int minY = baseY - padding;
  int maxX = baseX + sizeInTiles - 1 + padding;
  int maxY = baseY + sizeInTiles - 1 + padding;

  if (minX < 0 || minY < 0 || maxX >= _mapWidth || maxY >= _mapHeight) {
    return false;
  }

  for (int x = minX; x <= maxX; ++x) {
    for (int y = minY; y <= maxY; ++y) {
      if (_gridOccupied[x][y]) return false;
    }
  }
  return true;
}
bool PlacementManager::canPlaceSoldierAt(int tileX, int tileY,
                                         float radius) const {
  Vec2 placePos = tileToWorldCenter(tileX, tileY);

  // 检查与所有活跃士兵的距离
  for (Soldier* soldier : activeSoldiers) {
    if (soldier->isDead()) continue;
    float dist = placePos.distance(soldier->getPosition());
    float minDist = radius + soldier->getCollisionRadius();
    if (dist < minDist) {
      return false;  // 太近
    }
  }
  // 检查和建筑的距离
  auto scene = Director::getInstance()->getRunningScene();
  if (scene) {
    for (Node* child : scene->getChildren()) {
      auto building = dynamic_cast<Building*>(child);
      if (!building || building->isDestroyed()) continue;

      float buildingRadius = Building::getCollisionRadius();
      float dist = placePos.distance(building->getPosition());
      if (dist < radius + buildingRadius) {
        return false;
      }
    }
  }
  // 检查边界
  if (tileX < 0 || tileY < 0 || tileX >= _mapWidth || tileY >= _mapHeight) {
    return false;
  }

  return true;
}

void PlacementManager::occupyGrid(int baseX, int baseY, int sizeInTiles) {
  for (int x = baseX; x < baseX + sizeInTiles; ++x) {
    for (int y = baseY; y < baseY + sizeInTiles; ++y) {
      if (x >= 0 && x < _mapWidth && y >= 0 && y < _mapHeight) {
        _gridOccupied[x][y] = true;
      }
    }
  }
}

void PlacementManager::createPreviewSprite(int buildingType, int soldierType,
                                           Node* parentScene) {
  if (_previewSprite) {
    _previewSprite->removeFromParent();
    _previewSprite = nullptr;
  }

  std::string texPath;
  if (_currentType == PlacementType::BUILDING) {
    if (buildingType == 1)
      texPath = "gold_mine_icon_01.png";
    else if (buildingType == 2)
      texPath = "tower_icon_01.png";
    else if (buildingType == 3)
      texPath = "base_camp_01.png";
    else if (buildingType == 4)
      texPath = "elixir_collector_icon_01.png";
    else if (buildingType == 5)
      texPath = "cannon_01.png";
    else if (buildingType == 6)
      texPath = "Gold_Storage_01.png";
    else if (buildingType == 7)
      texPath = "Elixir_Storage_01.png";
    else if (buildingType == 8)
        texPath = "firecrackers_01.png";
  } else if (_currentType == PlacementType::SOLDIER) {
    if (soldierType == 1)
      texPath = "rarbarian_icon.png";
    else if (soldierType == 2)
      texPath = "giant_icon.png";
    else if (soldierType == 3)
      texPath = "bomberman_icon.png";
    else if (soldierType == 4)
      texPath = "archer_icon.png";
    else if (soldierType == 5)
      texPath = "ballon_soldier_icon.png";
  }

  _previewSprite = Sprite::create(texPath);
  if (!_previewSprite) {
    CCLOG("预览图加载失败: %s", texPath.c_str());
    _currentType = PlacementType::NONE;
    return;
  }
  _previewSprite->setOpacity(150);
  _previewSprite->setAnchorPoint(Vec2(0.5f, 0.5f));

  Size texSize = _previewSprite->getContentSize();

  auto vs = Director::getInstance()->getVisibleSize();
  auto origin = Director::getInstance()->getVisibleOrigin();
  _previewSprite->setPosition(origin + Vec2(vs.width / 2, vs.height / 2));

  parentScene->addChild(_previewSprite, 85);
}

void PlacementManager::removePreview(Node* parentScene) {
  if (_previewSprite) {
    _previewSprite->removeFromParent();
    _previewSprite = nullptr;
  }
  _currentType = PlacementType::NONE;
  _currentId = 0;
}

void PlacementManager::startPlaceBuilding(int buildingType) {
  _currentType = PlacementType::BUILDING;
  _currentId = buildingType;
}

void PlacementManager::startPlaceSoldier(int soldierType) {
  _currentType = PlacementType::SOLDIER;
  _currentId = soldierType;
}

void PlacementManager::onTouchMoved(Touch* touch, Event* event,
                                    Node* parentScene) {
  if (_currentType == PlacementType::NONE || !_previewSprite) return;

  Vec2 worldPos = touch->getLocation();
  Vec2 tilePos = worldToTile(worldPos);

  // 鼠标所在格子：
  int tileX = static_cast<int>(tilePos.x);
  int tileY = static_cast<int>(tilePos.y);

  int sizeInTiles = (_currentType == PlacementType::BUILDING) ? 2 : 1;

  // 让鼠标所在格子变成 2×2 的“几何中心”
  // 对 size=2：centerTile = (tileX, tileY)，则左下角 baseX = tileX - 1
  int centerX = tileX;
  int centerY = tileY;
  int baseX = centerX - sizeInTiles / 2;
  int baseY = centerY - sizeInTiles / 2;

  _previewBaseTile = Vec2(baseX, baseY);

  Vec2 snappedWorld = tileToWorldCenter(centerX, centerY);
  _previewSprite->setPosition(snappedWorld);
}

bool PlacementManager::onTouchBegan(Touch* touch, Event* event,
                                    Node* parentScene) {
  CCLOG("onTouchBegan in PlacementManager, type=%d", (int)_currentType);

  Vec2 worldPos = touch->getLocation();

  if (_currentType == PlacementType::NONE || !_previewSprite) {
    CCLOG("no currentType or no preview");
    return false;
  }
  int sizeInTiles = (_currentType == PlacementType::BUILDING) ? 2 : 1;
  int baseX = static_cast<int>(_previewBaseTile.x);
  int baseY = static_cast<int>(_previewBaseTile.y);

  int centerX = baseX + sizeInTiles / 2;
  int centerY = baseY + sizeInTiles / 2;

  // 放置前检查
  if (_currentType == PlacementType::BUILDING &&
      !canPlaceAt(baseX, baseY, sizeInTiles)) {
    CCLOG("格子被占用或离其他建筑太近");
    return false;
  }

  Vec2 centerTile(baseX + sizeInTiles / 2.0f, baseY + sizeInTiles / 2.0f);
  Vec2 snappedWorld = tileToWorldCenter(centerX, centerY);
  auto pdm = PlayerDataManager::getInstance();
  if (_currentType == PlacementType::BUILDING) {
    Building* newBuilding = nullptr;
    if (_currentId == 1) {
      newBuilding = ResourceBuilding::create(
          "gold_mine_icon_01.png", "金矿", CampType::PLAYER, 1, 3, 100, 60, 100,
          5.0f, ResourceType::GOLD, 10, 50);
      pdm->setGoldGrowthRate(pdm->getGoldGrowthRate() + 10);
    } else if (_currentId == 2) {
      newBuilding = DefenseBuilding::create(
          "tower_icon_01.png", "防御塔", CampType::PLAYER, 1, 3, 200, 50, 150,
          8.0f, 10, 200, AttackType::SINGLE_TARGET, 2.0f);
    } else if (_currentId == 3) {
      newBuilding = ResourceBuilding::create(
          "base_camp_01.png", "大本营", CampType::PLAYER, 1, 3, 200, 50, 150,
          8.0f, ResourceType::BUILDER, 0, 0);
    } else if (_currentId == 4) {
      newBuilding = ResourceBuilding::create(
          "elixir_collector_icon_01.png", "圣水收集器", CampType::PLAYER, 1, 3,
          100, 60, 100, 5.0f, ResourceType::ELIXIR, 10, 50);
      pdm->setElixirGrowthRate(pdm->getElixirGrowthRate() + 10);
    } else if (_currentId == 5) {
      newBuilding = DefenseBuilding::create(
          "cannon_01.png", "加农炮", CampType::PLAYER, 1, 3, 200, 50, 150, 8.0f,
          10, 220, AttackType::SINGLE_TARGET, 2.0f, TargetType::GROUND_ONLY);
    } else if (_currentId == 6) {
      newBuilding = ResourceStorageBuilding::create(
          "Gold_Storage_01.png", "储金罐", CampType::PLAYER, 1, 3, 400, 50, 100,
          5.0f, ResourceType::GOLD, 1000, 0.5f);
      pdm->setGoldLimit();
    } else if (_currentId == 7) {
      newBuilding = ResourceStorageBuilding::create(
          "Elixir_Storage_01.png", "圣水瓶", CampType::PLAYER, 1, 3, 400, 50,
          100, 5.0f, ResourceType::ELIXIR, 1000, 0.5f);
      pdm->setElixirLimit();
    } else if (_currentId == 8) {
      newBuilding = DefenseBuilding::create(
            "firecrackers_01.png", "防空火箭", CampType::PLAYER, 1, 3, 200, 50, 150, 8.0f,
            10, 200, AttackType::SINGLE_TARGET, 2.0f, TargetType::AIR_ONLY);
    }
    if (newBuilding) {
      CCLOG("addChild building");
      newBuilding->setPosition(snappedWorld);
      parentScene->addChild(newBuilding, 5);
      occupyGrid(baseX, baseY, sizeInTiles);
    }
    // 放置建筑后立即显示血条
    newBuilding->showHpBar(true);

  } else if (_currentType == PlacementType::SOLDIER) {
    Vec2 tilePos = worldToTile(worldPos);
    int tileX = static_cast<int>(tilePos.x);
    int tileY = static_cast<int>(tilePos.y);
    float soldierRadius = getSoldierRadius(_currentId);

    // 关键：预检查碰撞
    if (!canPlaceSoldierAt(tileX, tileY, soldierRadius)) {
      CCLOG("Cannot place soldier: collision detected");
      return false;
    }

    Vec2 placePos = tileToWorldCenter(tileX, tileY);
    CCLOG(">>> Try place soldier, type=%d", _currentId);
    Soldier* soldier = nullptr;
    if (_currentId == 1) {
      soldier = Rarbarian::create(50, 10, 50, 1);
    } else if (_currentId == 2) {
      soldier = Giant::create(300, 10, 50, 2);
    } else if (_currentId == 3) {
      soldier = Bomberman::create(1, 30, 10, 2);
    } else if (_currentId == 4) {
      soldier = Archer::create(20, 30, 180, 1);
    } else if (_currentId == 5) {
      soldier = BallonSoldier::create(50, 30, 10, 2);
    }

    CCLOG(">>> soldier created: %p", soldier);

    if (soldier) {
      // 设置位置、碰撞半径、激活检测
      soldier->setPosition(placePos);
      soldier->setCollisionRadius(soldierRadius);
      soldier->createPhysicsBody(soldierRadius);  // 激活update碰撞检测

      parentScene->addChild(soldier, 5);
      soldier->changeState(SoldierState::kIdle);

      // 注册到活跃士兵列表
      activeSoldiers.push_back(soldier);
      // 放下后立即开始自动寻路 + 移动
      CCLOG(">>> RUNNING PATHFINDING FOR SOLDIER");
      soldier->startAttack();

      CCLOG("Soldier placed at tile (%d,%d), radius=%.1f", tileX, tileY,
            soldierRadius);
    } else {
      CCLOG("Failed to get soldier from pool");
    }
  }

  removePreview(parentScene);
  return true;
}

void PlacementManager::cancelPlacement(Node* parentScene) {
  removePreview(parentScene);
}
void PlacementManager::removeSoldier(Soldier* soldier) {
  auto it = std::find(activeSoldiers.begin(), activeSoldiers.end(), soldier);
  if (it != activeSoldiers.end()) {
    activeSoldiers.erase(it);
    CCLOG("Soldier removed from active list");
  }
}
void PlacementManager::clearAll() {
  CCLOG("PlacementManager: Clearing all references and states.");

  // 1. 清空士兵和建筑的指针列表
  // 注意：这里不需要手动 delete 士兵/建筑，因为它们作为场景节点
  // 会在 replaceScene 时由 Cocos2d-x 的引用计数机制自动销毁。
  activeSoldiers.clear();
  // 2. 重置放置状态
  _currentType = PlacementType::NONE;
  if (_previewSprite) {
    // tempSprite 是单例持有的临时显示对象，场景销毁时它可能还没被移除
    _previewSprite->removeFromParent();
    _previewSprite = nullptr;
  }

  // 3. 停止所有正在进行的调度器（如果有）
  // 如果你在 PlacementManager 里使用了 scheduleUpdate，必须停止它
  Director::getInstance()->getScheduler()->unscheduleUpdate(this);

  // 4. 重置网格占用地图
  // 确保下次进入场景时，网格是干净的
  for (int i = 0; i < _mapWidth; ++i) {
    for (int j = 0; j < _mapHeight; ++j) {
      _gridOccupied[i][j] = 0;
    }
  }
}
