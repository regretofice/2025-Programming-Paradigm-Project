#ifndef __UPGRADE_MANAGER_H__
#define __UPGRADE_MANAGER_H__

#include "Building/Building.h"
#include "cocos2d.h"

class UpgradeManager {
 public:
  static UpgradeManager* getInstance();

  // 执行升级逻辑
  bool tryUpgradeBuilding(Building* building);

 private:
  UpgradeManager() = default;
  static UpgradeManager* instance;

  // 获取升级后的新贴图路径
  std::string getUpgradeTexture(Building* building);
};

#endif
