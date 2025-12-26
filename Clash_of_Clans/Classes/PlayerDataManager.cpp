#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "PlayerDataManager.h"

#include "BuildingManager.h"
#include "ResourceBuilding.h"
#include"ResourceStorageBuilding.h"
#include "BuildingEnums.h"

// 使用别名进行缩写，请勿在.h文件中使用
using PDM = PlayerDataManager;
// 对static变量进行类外初始化
PDM* PDM::instance_ = nullptr;

PDM* PDM::getInstance() {
  if (instance_ == nullptr) {
    // std::nothrow禁用报错，哪怕内存申请失败
    instance_ = new (std::nothrow) PlayerDataManager();
    // 单例模式禁止调用autorelease，可以不手动释放内存，OS会兜底
    instance_->retain();  // 单例手动管理生命周期，避免被autorelease销毁
  }
  return instance_;
}

void PDM::destroyInstance() {
  if (instance_) {
    instance_->release();
    instance_ = nullptr;
  }
}

PDM::PlayerDataManager() {
  // 获取UserDefault实例（全局唯一，cocos2d-x已封装）
  userDefault_ = cocos2d::UserDefault::getInstance();
  // 加载本地数据到内存
  initData();

  // 启动在线时的定时器（每分钟增长资源）
  startOnlineTimer();
}

// 析构时无需手动销毁_userDefault（引擎管理）
PDM::~PlayerDataManager() {
  if (scheduler_) {
    // 停止定时器，避免野指针
    scheduler_->unscheduleAllForTarget(this);
  }
  syncAllDataToLocal();
}

// 初始化数据：从本地加载，无数据则设默认值
void PDM::initData() { loadData(); }

void PDM::setGold(int gold) {
  if (gold < 0) {
    gold = 0;
  } else if (gold > gold_limit_) {
    gold = gold_limit_;
  }
  gold_ = gold;
  syncGoldToLocal();
  CCLOG("金币已更新为：%d", gold_);
}
void PDM::setGoldLimit() {
  gold_limit_ = 100;  // 初始上限为100，每个储存型建筑加一点
  auto buildings = BuildingManager::getInstance()->getAllBuildings();
  for (auto building : buildings) {
    if (building->getType() == BuildingType::STORAGE) {
      auto storage = dynamic_cast<ResourceStorageBuilding*>(building);
      if (storage->getResType() == ResourceType::GOLD)
        gold_limit_ += storage->getMaxStorageCapacity();
    }
  }

  CCLOG("金币上限已更新为：%d", gold_limit_);
}
void PDM::setGoldGrowthRate(int rate) {
  if (rate < 0) {
    rate = 1;
  }
  gold_growth_rate = rate;
  syncGoldGrowRateToLocal();
  CCLOG("金币增长速率已更新为：%d", rate);
}

void PDM::setElixir(int elixir) {
  if (elixir < 0 || elixir > elixir_limit_) {
    CCLOG("警告：圣水输入值异常（%d），即将被修正", elixir);
  }
  if (elixir < 0) {
    elixir = 0;
  } else if (elixir > elixir_limit_) {
    elixir = elixir_limit_;
  }
  elixir_ = elixir;
  syncElixirToLocal();
  CCLOG("圣水已更新为：%d", elixir_);
}
void PDM::setElixirLimit() {
  elixir_limit_ = 100;  // 初始上限为100，每个储存型建筑加一点
  auto buildings = BuildingManager::getInstance()->getAllBuildings();
  for (auto building : buildings) {
    if (building->getType() == BuildingType::STORAGE) {
      auto storage = dynamic_cast<ResourceStorageBuilding*>(building);
      if (storage->getResType() == ResourceType::ELIXIR)
        elixir_limit_ += storage->getMaxStorageCapacity();
    }
  }

  CCLOG("圣水上限已更新为：%d", elixir_limit_);
}
void PDM::setElixirGrowthRate(int rate) {
  if (rate < 0) {
    rate = 1;
  }
  elixir_growth_rate = rate;

  CCLOG("圣水增长速率已更新为：%d", rate);
}
void PDM::setBuilder(int builder) {
  if (builder < 0) {
    builder = 0;
  } else if (builder > builder_limit_) {
    builder = builder_limit_;
  }
  builder_ = builder;
  syncBuilderToLocal();
  CCLOG("建筑工人已更新为：%d", builder_);
}
void PDM::setBuilderLimit() {
  builder_limit_ = 100;  // 初始上限为100，每个储存型建筑加一点
  auto buildings = BuildingManager::getInstance()->getAllBuildings();
  for (auto building : buildings) {
    if (building->getType() == BuildingType::STORAGE) {
      auto storage = dynamic_cast<ResourceBuilding*>(building);
      if (storage->getResType() == ResourceType::BUILDER)
        builder_limit_ += storage->getMaxCapacity();
    }
  }

  CCLOG("建筑工人上限已更新为：%d", builder_limit_);
}
void PDM::setBuilderGrowthRate(int rate) {
  if (rate < 0) {
    rate = 1;
  }
  builder_growth_rate = rate;
  syncBuilderGrowRateToLocal();
  CCLOG("建筑工人增长速率已更新为：%d", rate);
}
// 每次更新完数据之后要调用flush来保存数据，否则会丢失
void PDM::syncGoldToLocal() {
  userDefault_->setIntegerForKey(kKeyGold.c_str(), gold_);
  userDefault_->flush();
}
void PDM::syncGoldGrowRateToLocal() {
  userDefault_->setIntegerForKey(kKeyGoldGrowthRate.c_str(), gold_growth_rate);
  userDefault_->flush();
}
void PDM::syncElixirToLocal() {
  userDefault_->setIntegerForKey(kKeyElixir.c_str(), elixir_);
  userDefault_->flush();
}
void PDM::syncElixirGrowRateToLocal() {
  userDefault_->setIntegerForKey(kKeyElixirGrowthRate.c_str(),
                                 elixir_growth_rate);
  userDefault_->flush();
}
void PDM::syncBuilderToLocal() {
  userDefault_->setIntegerForKey(kKeyBuilder.c_str(), builder_);
  userDefault_->flush();
}
void PDM::syncBuilderGrowRateToLocal() {
  userDefault_->setIntegerForKey(kKeyBuilderGrowthRate.c_str(),
                                 builder_growth_rate);
  userDefault_->flush();
}
void PDM::syncTimeToLocal() {
  userDefault_->setStringForKey(kKeyLastRecordTime.c_str(),
                                std::to_string(lastRecordTime_));
  userDefault_->flush();
}
void PDM::syncBuildingToLocal() {
  // 保存建筑数据
  int buildingCount = buildingDatas_.size();
  userDefault_->setIntegerForKey("buildingCount", buildingCount);

  for (int i = 0; i < buildingCount; ++i) {
    std::string prefix = "building_" + std::to_string(i) + "_";
    userDefault_->setIntegerForKey(
        (prefix + "BuildingType").c_str(),
        static_cast<int>(buildingDatas_[i].buildingType));
    if (buildingDatas_[i].buildingType == BuildingType::RESOURCE ||
        buildingDatas_[i].buildingType == BuildingType::STORAGE) {
      userDefault_->setIntegerForKey(
          (prefix + "ResourceType").c_str(),
          static_cast<int>(buildingDatas_[i].resourceType));
    } else {
      userDefault_->setIntegerForKey(
          (prefix + "ResourceType").c_str(),
          static_cast<int>(ResourceType::NORESOURCE));
    }
    if (buildingDatas_[i].buildingType == BuildingType::DEFENSE) {
        userDefault_->setIntegerForKey(
            (prefix + "targetType").c_str(),
            static_cast<int>(buildingDatas_[i].targetType));
    }
    else {
        userDefault_->setIntegerForKey(
            (prefix + "targetType").c_str(),
            static_cast<int>(TargetType::NONE));
    }

    userDefault_->setFloatForKey((prefix + "x").c_str(),
                                 buildingDatas_[i].positionX);
    userDefault_->setFloatForKey((prefix + "y").c_str(),
                                 buildingDatas_[i].positionY);
    userDefault_->setIntegerForKey((prefix + "level").c_str(),
                                   buildingDatas_[i].level);
    userDefault_->setStringForKey((prefix + "name").c_str(),
                                  buildingDatas_[i].name);
  }

  userDefault_->flush();
}

void PDM::syncAllDataToLocal() {
  syncGoldToLocal();
  syncElixirToLocal();
  syncBuilderToLocal();
  syncTimeToLocal();
  syncBuildingToLocal();
}
void PDM::resetAllData() {
  gold_ = 0;
  elixir_ = 0;
  builder_ = 0;
  syncGoldToLocal();
  syncElixirToLocal();
  syncBuilderToLocal();
  CCLOG("玩家数据已重置");
}

// 圣水在线增长
void PDM::startOnlineTimer() {
  scheduler_ = cocos2d::Director::getInstance()->getScheduler();
  if (!scheduler_) {
    CCLOG("错误：获取Scheduler失败！");  // 可添加此日志排查
    return;
  }
  // 先取消同名定时器，避免重复调度
  scheduler_->unschedule("growth_timer", this);

  // 注册定时器
  scheduler_->schedule(
      [=](float dt) {
        setGold(gold_ + gold_growth_rate);
        setElixir(elixir_ + elixir_growth_rate);
        setBuilder(builder_ + builder_growth_rate);
        lastRecordTime_ = TimeTools::getCurrentTimeMs();
        syncTimeToLocal();
        CCLOG("资源增长，当前金币：%d，圣水：%d，建筑工人：%d", gold_, elixir_,
              builder_);  // 新增日志
      },
      // 定时器回调函数,直接使用lambda表达式，请勿使用bind
      this,               // 定时器关联的目标对象（PDM单例实例）
      1.0f,               // 定时器执行间隔（单位：秒）
      CC_REPEAT_FOREVER,  // 定时器重复执行次数
      0.0f,               // 定时器延迟执行时间（单位：秒）
      false,              // 定时器是否初始暂停
      "growth_timer"      // 定时器唯一标签（用于后续取消/查找）
  );
  CCLOG("startOnlineTimer：资源增长定时器已启动");  // 新增日志

  ////////////////
  ////调用时请在Scene加入如下语句
  // this->schedule(
  //     [=](float dt) {
  //       int currentElixir_ = PlayerDataManager::getInstance()->getElixir();
  //       goldLabel->setString("圣水：" + std::to_string(currentElixir_));
  //       CCLOG("UI定时器触发，当前圣水：%d", currentElixir_);  // 新增日志
  //     },
  //     0.5f, "elixirUpdate");
}

void PlayerDataManager::addBuildingData(const BuildingData& data) {
  buildingDatas_.push_back(data);
}

// 获取所有建筑数据
const std::vector<BuildingData>& PlayerDataManager::getAllBuildingData() const {
  return buildingDatas_;
}

// 清空建筑数据
void PlayerDataManager::clearBuildingData() { buildingDatas_.clear(); }

void PDM::loadData() {
  // 从本地存储重新加载数据（复用initData中的读取逻辑）
  setGoldLimit();
  setGold(userDefault_->getIntegerForKey(kKeyGold.c_str(), gold_));

  setElixirLimit();
  setElixir(userDefault_->getIntegerForKey(kKeyElixir.c_str(), elixir_));

  setBuilderLimit();
  setBuilder(userDefault_->getIntegerForKey(kKeyBuilder.c_str(), builder_));

  setGoldGrowthRate(userDefault_->getIntegerForKey(kKeyGoldGrowthRate.c_str(),
                                                   gold_growth_rate));
  setElixirGrowthRate(userDefault_->getIntegerForKey(
      kKeyElixirGrowthRate.c_str(), elixir_growth_rate));
  setBuilderGrowthRate(userDefault_->getIntegerForKey(
      kKeyBuilderGrowthRate.c_str(), builder_growth_rate));

  std::string timeStr = userDefault_->getStringForKey(
      kKeyLastRecordTime.c_str(), std::to_string(lastRecordTime_));
  lastRecordTime_ = std::stoll(timeStr);

  buildingDatas_.clear();
  int buildingCount = userDefault_->getIntegerForKey("buildingCount", 0);

  for (int i = 0; i < buildingCount; ++i) {
    std::string prefix = "building_" + std::to_string(i) + "_";
    BuildingData data;
    data.buildingType = static_cast<BuildingType>(
        userDefault_->getIntegerForKey((prefix + "BuildingType").c_str(), 0));
    data.resourceType = static_cast<ResourceType>(
        userDefault_->getIntegerForKey((prefix + "ResourceType").c_str(), 0));
    data.positionX = userDefault_->getFloatForKey((prefix + "x").c_str(), 0);
    data.positionY = userDefault_->getFloatForKey((prefix + "y").c_str(), 0);
    data.level = userDefault_->getIntegerForKey((prefix + "level").c_str(), 1);
    data.name = userDefault_->getStringForKey((prefix + "name").c_str(), "");

    buildingDatas_.push_back(data);
  }
  CCLOG("手动加载玩家数据:\n金币:%d  圣水:%d  建筑工人:%d", gold_, elixir_,
        builder_);
}

void PlayerDataManager::saveData() {
  syncAllDataToLocal();  // 复用已有的全量同步逻辑
  CCLOG("手动触发数据保存完成");
}