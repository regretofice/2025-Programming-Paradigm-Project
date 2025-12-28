#include "Building/BuildingManager.h"

#include <algorithm>

#include "cocos2d.h"

using namespace cocos2d;

BuildingManager* BuildingManager::_instance = nullptr;
std::vector<Building*> BuildingManager::_buildings = {};

BuildingManager* BuildingManager::getInstance() {
  if (!_instance) {
    _instance = new (std::nothrow) BuildingManager();
  }
  return _instance;
}

void BuildingManager::destroyInstance() {
  if (_instance) {
    _instance->clearAllBuildings();
    delete _instance;
    _instance = nullptr;
  }
}

BuildingManager::~BuildingManager() { clearAllBuildings(); }

void BuildingManager::addBuilding(Building* building) {
  if (!building) return;

  // 避免重复添加
  auto it = std::find(_buildings.begin(), _buildings.end(), building);
  if (it == _buildings.end()) {
    _buildings.push_back(building);
  }
}

void BuildingManager::removeBuilding(Building* building) {
  if (!building) return;

  auto it = std::find(_buildings.begin(), _buildings.end(), building);
  if (it != _buildings.end()) {
    _buildings.erase(it);
  }
}

void BuildingManager::destroyBuilding(Building* building) {
  if (!building) return;

  // 从管理器中移除
  removeBuilding(building);

  // 从场景中移除并释放内存
  building->removeFromParent();
  delete building;
}

const std::vector<Building*>& BuildingManager::getAllBuildings() const {
  return _buildings;
}

std::vector<Building*> BuildingManager::getBuildingsByCamp(
    CampType camp) const {
  std::vector<Building*> result;
  for (auto building : _buildings) {
    if (building && !building->isDestroyed() && building->getCamp() == camp) {
      result.push_back(building);
    }
  }
  return result;
}

std::vector<Building*> BuildingManager::getBuildingsByType(
    BuildingType type) const {
  std::vector<Building*> result;
  for (auto building : _buildings) {
    if (building && !building->isDestroyed() && building->getType() == type) {
      result.push_back(building);
    }
  }
  return result;
}

std::vector<Building*> BuildingManager::getBuildingsByCampAndType(
    CampType camp, BuildingType type) const {
  std::vector<Building*> result;
  for (auto building : _buildings) {
    if (building && !building->isDestroyed() && building->getCamp() == camp &&
        building->getType() == type) {
      result.push_back(building);
    }
  }
  return result;
}

std::vector<Building*> BuildingManager::getBuildingsInRange(const Vec2& pos,
                                                            float range) const {
  std::vector<Building*> result;
  float rangeSquared = range * range;  // 用平方比较提高效率

  for (auto building : _buildings) {
    if (building && !building->isDestroyed()) {
      float distanceSquared = building->getPosition().distanceSquared(pos);
      if (distanceSquared <= rangeSquared) {
        result.push_back(building);
      }
    }
  }
  return result;
}

void BuildingManager::clearAllBuildings() {
  // 仅仅清空列表即可，因为建筑作为 MapScene 的子节点，
  // 在场景切换时会被 Cocos2dx 引擎自动销毁。
  _buildings.clear();
}

void BuildingManager::update(float dt) {
  // 清理已销毁的建筑
  auto it = _buildings.begin();
  while (it != _buildings.end()) {
    Building* building = *it;
    if (building->isDestroyed()) {
      building->removeFromParent();
      delete building;
      it = _buildings.erase(it);
    } else {
      ++it;
    }
  }
}
