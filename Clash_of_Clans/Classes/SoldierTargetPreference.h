#ifndef __SOLDIER_TARGET_PREFERENCE_H__
#define __SOLDIER_TARGET_PREFERENCE_H__

#include <unordered_map>
#include <vector>

#include "BuildingEnums.h"
#include "Soldier.h"

class SoldierTargetPreference {
 public:
  // 每种士兵的优先目标类型列表（第一优先在前）
  static const std::vector<BuildingType>& getPreferredTargets(
      SoldierType soldierType);

  // 判断建筑物类型是否是士兵的优先目标之一
  static bool isPreferredTarget(SoldierType soldierType,
                                BuildingType buildingType);

 private:
  static std::unordered_map<SoldierType, std::vector<BuildingType>> preferences;
  static void initPreferences();
};

#endif
