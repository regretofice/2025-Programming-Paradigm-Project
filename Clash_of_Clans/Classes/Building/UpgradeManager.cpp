#include "Building/UpgradeManager.h"

#include "Building/DefenseBuilding.h"
#include "Building/ResourceBuilding.h"
#include "Building/ResourceStorageBuilding.h"
#include "Tools/PlayerDataManager.h"

UpgradeManager* UpgradeManager::instance = nullptr;

UpgradeManager* UpgradeManager::getInstance() {
  if (!instance) instance = new UpgradeManager();
  return instance;
}

std::string UpgradeManager::getUpgradeTexture(Building* building) {
  // 根据类型和等级返回新的贴图名
  // 已经升级了
  if (building->getType() == BuildingType::RESOURCE) {
    auto subBuilding = dynamic_cast<ResourceBuilding*>(building);
    subBuilding->setMaxCapacity(subBuilding->getMaxCapacity() + 100);
    subBuilding->setProductionPerSec(subBuilding->getProductionPerSec() + 5);
    if (subBuilding->getResType() == ResourceType::GOLD) {
      return "gold_mine_icon_0" + std::to_string(subBuilding->getLevel()) +
             ".png";
    } else if (subBuilding->getResType() == ResourceType::ELIXIR) {
      return "elixir_collector_icon_0" +
             std::to_string(subBuilding->getLevel()) + ".png";
    } else {
      return "base_camp_0" + std::to_string(subBuilding->getLevel()) + ".png";
    }
  } else if (building->getType() == BuildingType::DEFENSE) {
    auto subBuilding = dynamic_cast<DefenseBuilding*>(building);
    subBuilding->setDPS(subBuilding->getDPS() + 5);
    if (subBuilding->getTargetType() == TargetType::AIR_ONLY) {
      return "firecrackers_0" + std::to_string(subBuilding->getLevel()) +
             ".png";
    } else if (subBuilding->getTargetType() == TargetType::GROUND_ONLY) {
      return "cannon_0" + std::to_string(subBuilding->getLevel()) + ".png";
    } else {
      return "tower_icon_0" + std::to_string(subBuilding->getLevel()) + ".png";
    }

  } else if (building->getType() == BuildingType::STORAGE) {
    auto subBuilding = dynamic_cast<ResourceStorageBuilding*>(building);
    subBuilding->setMaxStorageCapacity(subBuilding->getMaxStorageCapacity() +
                                       400);
    if (subBuilding->getResType() == ResourceType::GOLD) {
      return "Gold_Storage_0" + std::to_string(subBuilding->getLevel()) +
             ".png";
    } else if (subBuilding->getResType() == ResourceType::ELIXIR) {
      return "Elixir_Storage_0" + std::to_string(subBuilding->getLevel()) +
             ".png";
    } else {
      return "base_camp_0" + std::to_string(subBuilding->getLevel()) + ".png";
    }
  }
}

bool UpgradeManager::tryUpgradeBuilding(Building* building) {
  if (!building || building->isDestroyed()) return false;

  int cost = building->getUpGradeCost();
  auto pdm = PlayerDataManager::getInstance();

  if (pdm->getGold() >= cost) {
    if (!building->setLevel(building->getLevel() + 1)) {
      CCLOG("无法升为当前等级");
      return false;
    }
    // 1. 扣除资源
    pdm->setGold(pdm->getGold() - cost);

    // 2.增加血量上限
    building->setMaxHp(building->getMaxHp() + 100);
    building->setHp(building->getMaxHp());  // 升级后血量回满

    // 3. 更换贴图
    std::string newTex = getUpgradeTexture(building);
    building->setTexture(newTex);

    CCLOG("%s 升级成功！当前等级：%d", building->getName().c_str(),
          building->getLevel());
    return true;
  } else {
    CCLOG("金币不足，需要 %d", cost);
    return false;
  }
}
