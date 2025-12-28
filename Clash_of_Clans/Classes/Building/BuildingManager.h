#ifndef BUILDING_MANAGER_H
#define BUILDING_MANAGER_H

#include <memory>
#include <vector>

#include "Building/Building.h"
#include "Building/BuildingEnums.h"
#include "cocos2d.h"

// 建筑管理器：单例模式，负责管理游戏中所有建筑的生命周期和查询
class BuildingManager {
 public:
  // 获取单例实例
  static BuildingManager* getInstance();

  // 销毁单例实例
  static void destroyInstance();

  // 添加建筑到管理器
  static void addBuilding(Building* building);

  // 从管理器中移除建筑（不负责销毁建筑本身）
  static void removeBuilding(Building* building);

  // 销毁并移除建筑（同时释放内存）
  void destroyBuilding(Building* building);

  // 获取所有建筑
  const std::vector<Building*>& getAllBuildings() const;

  // 根据阵营获取建筑
  std::vector<Building*> getBuildingsByCamp(CampType camp) const;

  // 根据建筑类型获取建筑
  std::vector<Building*> getBuildingsByType(BuildingType type) const;

  // 根据阵营和类型获取建筑
  std::vector<Building*> getBuildingsByCampAndType(CampType camp,
                                                   BuildingType type) const;

  // 获取指定位置附近的建筑（范围内）
  std::vector<Building*> getBuildingsInRange(const cocos2d::Vec2& pos,
                                             float range) const;

  // 清理所有建筑（游戏结束时调用）
  void clearAllBuildings();

  // 每帧更新（处理建筑状态变化）
  void update(float dt);

 private:
  // 单例模式：私有构造/析构
  BuildingManager() = default;
  ~BuildingManager();

  // 禁止拷贝
  BuildingManager(const BuildingManager&) = delete;
  BuildingManager& operator=(const BuildingManager&) = delete;

  // 存储所有建筑的容器
  static std::vector<Building*> _buildings;

  // 单例实例
  static BuildingManager* _instance;
};

#endif  // BUILDING_MANAGER_H
