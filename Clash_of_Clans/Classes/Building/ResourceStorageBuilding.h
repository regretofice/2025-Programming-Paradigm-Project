#ifndef RESOURCE_STORAGE_BUILDING_H
#define RESOURCE_STORAGE_BUILDING_H

#include "Building/Building.h"

class ResourceStorageBuilding : public Building {
 private:
  // 存储建筑专属属性
  ResourceType _resType;    // 存储资源类型（仅GOLD/ELIXIR）
  int _maxStorageCapacity;  // 最大存储容量
  int _currentStored;       // 当前存储量
  float _destroyLossRate;   // 被摧毁时资源丢失率
  ResourceStorageBuilding() = default;

 public:
  // Cocos2d创建函数
  static ResourceStorageBuilding* create(
      const std::string& texPath, const std::string& name, CampType camp,
      int level, int maxLevel, int maxHP, float size, int upgradeCost,
      float upgradeTime, ResourceType resType, int maxStorageCapacity,
      float destroyLossRate = 0.5f);

  // 初始化函数（重写父类）
  virtual bool init(const std::string& texPath, const std::string& name,
                    CampType camp, int level, int maxLevel, int maxHP,
                    float size, int upgradeCost, float upgradeTime,
                    ResourceType resType, int maxStorageCapacity,
                    float destroyLossRate = 0.5f);

  // 专属方法
  bool depositResource(int amount);        // 存入资源（返回是否成功）
  int withdrawResource(int amount);        // 取出资源（返回实际取出量）
  void loseResourceOnDestroy();            // 被摧毁时丢失资源
  virtual bool upgrade() override;         // 重写升级（提升容量+血量）
  virtual void checkDestroyed() override;  // 显式声明重写基类方法
  virtual void update(float dt) override;  // 重写更新（无产出，仅状态检查）

  // Getter/Setter
  int getCurrentStored() const { return _currentStored; }
  int getMaxStorageCapacity() const { return _maxStorageCapacity; }
  ResourceType getResType() const { return _resType; }
  void setMaxStorageCapacity(int newMaxStorageCapacity) {
    _maxStorageCapacity = newMaxStorageCapacity;
  }

  // 析构函数
  virtual ~ResourceStorageBuilding();
};

// 便捷创建函数（语义化封装）
// 创建储金罐
inline ResourceStorageBuilding* createGoldStorage(const std::string& texPath,
                                                  CampType camp, int level) {
  // 不同等级的属性可配置化，此处硬编码为示例
  int maxLevel = 3;
  int maxHP = 800 + (level - 1) * 200;           // 1级800血，每级+200
  float size = 120.0f;                           // 边长
  int upgradeCost = 200 + (level - 1) * 150;     // 1级升级成本200金币
  float upgradeTime = 40.0f + (level - 1) * 10;  // 1级升级时间40秒
  int maxStorage = 5000 + (level - 1) * 1000;    // 1级存储容量5000金币
  return ResourceStorageBuilding::create(
      texPath, "储金罐", camp, level, maxLevel, maxHP, size, upgradeCost,
      upgradeTime, ResourceType::GOLD, maxStorage);
}

// 创建圣水瓶
inline ResourceStorageBuilding* createElixirStorage(const std::string& texPath,
                                                    CampType camp, int level) {
  int maxLevel = 3;
  int maxHP = 700 + (level - 1) * 180;          // 1级700血，每级+180
  float size = 110.0f;                          // 边长
  int upgradeCost = 180 + (level - 1) * 130;    // 1级升级成本180圣水
  float upgradeTime = 35.0f + (level - 1) * 8;  // 1级升级时间35秒
  int maxStorage = 4500 + (level - 1) * 900;    // 1级存储容量4500圣水
  return ResourceStorageBuilding::create(
      texPath, "圣水瓶", camp, level, maxLevel, maxHP, size, upgradeCost,
      upgradeTime, ResourceType::ELIXIR, maxStorage);
}

#endif  // RESOURCE_STORAGE_BUILDING_H