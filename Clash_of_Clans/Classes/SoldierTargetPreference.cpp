#include "SoldierTargetPreference.h"

std::unordered_map<SoldierType, std::vector<BuildingType>>
    SoldierTargetPreference::preferences;

const std::vector<BuildingType>& SoldierTargetPreference::getPreferredTargets(
    SoldierType soldierType) {
  static bool initialized = false;
  if (!initialized) {
    initPreferences();
    initialized = true;
  }

  auto it = preferences.find(soldierType);
  if (it != preferences.end()) {
    return it->second;
  }
  // 默认返回空（找最近的）
  static const std::vector<BuildingType> empty;
  return empty;
}

bool SoldierTargetPreference::isPreferredTarget(SoldierType soldierType,
                                                BuildingType buildingType) {
  auto& prefs = getPreferredTargets(soldierType);
  return std::find(prefs.begin(), prefs.end(), buildingType) != prefs.end();
}

void SoldierTargetPreference::initPreferences() {
  //  野蛮人：优先资源，其次防御 // 金矿、圣水塔
  preferences[SoldierType::kGroundRarbarian] = {BuildingType::RESOURCE};

  // 炸弹人：优先防御（肉弹清塔）// 先炸塔
  preferences[SoldierType::kGroundBomberman] = {BuildingType::DEFENSE};

  //  弓箭手：优先防御（远程攻击塔）
  preferences[SoldierType::kGroundArcher] = {BuildingType::DEFENSE};

  // 巨人：优先防御（扛伤害）
  preferences[SoldierType::kGroundGiant] = {BuildingType::DEFENSE};

  // 气球：优先特殊目标
  preferences[SoldierType::kAirBallonSoldier] = {BuildingType::DEFENSE};
}
