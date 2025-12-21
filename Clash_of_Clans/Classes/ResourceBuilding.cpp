#include "ResourceBuilding.h"

ResourceBuilding::ResourceBuilding()
    : _resType(ResourceType::GOLD),
      _productionPerSec(0),
      _maxCapacity(0),
      _currentRes(0) {}

ResourceBuilding::~ResourceBuilding() {}

ResourceBuilding* ResourceBuilding::create(
    const std::string& texPath, const std::string& name, CampType camp,
    int level, int maxLevel, int maxHP, float size, int upgradeCost,
    float upgradeTime, ResourceType resType, int productionPerSec,
    int maxCapacity) {
  ResourceBuilding* res = new (std::nothrow) ResourceBuilding();
  if (res &&
      res->init(texPath, name, camp, level, maxLevel, maxHP, size, upgradeCost,
                upgradeTime, resType, productionPerSec, maxCapacity)) {
    res->autorelease();
    return res;
  }
  CC_SAFE_DELETE(res);
  return nullptr;
}

bool ResourceBuilding::init(const std::string& texPath, const std::string& name,
                            CampType camp, int level, int maxLevel, int maxHP,
                            float size, int upgradeCost, float upgradeTime,
                            ResourceType resType, int productionPerSec,
                            int maxCapacity) {
  if (!Building::init(texPath, name, camp, level, maxLevel, maxHP, size,
                      upgradeCost, upgradeTime, BuildingType::RESOURCE,
                      size / 2)) {
    CCLOG("ResourceBuilding init failed: Building init failed");
    return false;
  }

  // 初始化资源属性
  _resType = resType;
  _productionPerSec = productionPerSec;
  _maxCapacity = maxCapacity;
  _currentRes = 0;

  CCLOG("ResourceBuilding init success");
  return true;
}

void ResourceBuilding::produceResource(float dt) {
  if (_isDestroyed || _productionPerSec <= 0) return;

  // 计算本次产出（每秒产出 * 时间增量）
  int produce = static_cast<int>(_productionPerSec * dt);
  if (produce <= 0) return;

  // 限制存储容量
  _currentRes += produce;
  _currentRes = std::min(_currentRes, _maxCapacity);

  // 日志输出（示例）
  const char* resName = _resType == ResourceType::GOLD ? "金币" : "圣水";
  CCLOG("[%s]生产%d%s，当前存储：%d/%d", _name.c_str(), produce, resName,
        _currentRes, _maxCapacity);
}

int ResourceBuilding::collectResource() {
  if (_isDestroyed || _currentRes <= 0) return 0;

  // 收集所有资源（实际游戏可扩展为部分收集）
  int collected = _currentRes;
  _currentRes = 0;

  const char* resName = _resType == ResourceType::GOLD ? "金币" : "圣水";
  CCLOG("[%s]收集%d%s", _name.c_str(), collected, resName);
  return collected;
}

void ResourceBuilding::update(float dt) {
  Building::update(dt);

  if (_isDestroyed) return;

  // 每帧生产资源
  produceResource(dt);
}