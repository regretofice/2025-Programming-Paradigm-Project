#ifndef __PLAYER_DATA_MANAGER_H__
#define __PLAYER_DATA_MANAGER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "BuildingEnums.h"
#include "TimeTools.h"
#include "cocos2d.h"

struct BuildingData {
  BuildingType buildingType;
  ResourceType resourceType;
  TargetType targetType;
  float positionX;
  float positionY;
  int level;
  std::string name;
};

class PlayerDataManager : public cocos2d::Ref {
 public:
  // 获取实例（全局只有这一个）
  static PlayerDataManager* getInstance();
  // 销毁单例（程序退出时调用）
  static void destroyInstance();

  /////////////////////////////
  //   获取资源的数量的接口，以及设置资源的接口

  // 获取金币数量
  int getGold() const { return gold_; };
  // 获取金币上限数量
  int getGoldLimit() const { return gold_limit_; };
  // 获取金币增长速率
  int getGoldGrowthRate() const { return gold_growth_rate; };
  // 设置金币数量（保存到本地）
  void setGold(int gold);
  // 设置金币上限
  void setGoldLimit();
  // 设置金币回复速率
  void setGoldGrowthRate(int rate);

  // 获取圣水数量
  int getElixir() const { return elixir_; };
  // 获取圣水上限
  int getElixirLimit() const { return elixir_limit_; };
  // 获取圣水回复速率
  int getElixirGrowthRate() const { return elixir_growth_rate; };
  // 设置圣水数量（保存到本地）
  void setElixir(int elixir);
  // 设置圣水上限
  void setElixirLimit();
  // 设置圣水回复速率
  void setElixirGrowthRate(int rate);

  // 获取建筑工人数量
  int getBuilder() const { return builder_; };
  // 获取建筑工人上限
  int getBuilderLimit() const { return builder_limit_; };
  // 获取建筑工人回复速率
  int getBuilderGrowthRate() const { return builder_growth_rate; };
  // 设置建筑工人数量（保存到本地）
  void setBuilder(int builder);
  // 设置建筑工人上限
  void setBuilderLimit();
  // 设置建筑工人回复速率
  void setBuilderGrowthRate(int rate);

  void updateAllLimits();

  /////////////////////////////
  //   建筑存档相关接口
  void addBuildingData(const BuildingData& data);
  const std::vector<BuildingData>& getAllBuildingData() const;
  void clearBuildingData();

  ///////////////////////////////
  // 保存数据接口
  //  手动保存所有数据到本地
  void saveData();
  // 手动从本地加载所有数据
  void loadData();

  // 重置所有数据的数值（创建实例时/更换存档调用）
  void resetAllData();

 private:
  // 私有构造/析构：禁止外部创建/销毁，强制用单例方法
  PlayerDataManager();
  ~PlayerDataManager();

  // 禁用拷贝/赋值：保证单例唯一性
  PlayerDataManager(const PlayerDataManager&) = delete;
  PlayerDataManager& operator=(const PlayerDataManager&) = delete;

  // 唯一实例的指针，
  static PlayerDataManager* instance_;

  // 本地存储的名称，不使用硬编码，便于后续维护
  const std::string kKeyGold = "player_gold";
  const std::string kKeyGoldLimit = "gold_limit";
  const std::string kKeyGoldGrowthRate = "gold_growth";
  const std::string kKeyElixir = "player_elixir";
  const std::string kKeyElixirLimit = "elixir_limit";
  const std::string kKeyElixirGrowthRate = "elixir_growth";
  const std::string kKeyBuilder = "player_builder";
  const std::string kKeyBuilderLimit = "builder_limit";
  const std::string kKeyBuilderGrowthRate = "builder_growth";
  const std::string kKeyLastRecordTime = "last_record_time";

  // 记录各项数值，存在实例之中，并非本地文件
  int gold_;             // 当前金币数量
  int gold_limit_;       // 金币数量上限
  int gold_growth_rate;  // 金币自动增加的速率

  int elixir_;             // 当前圣水数量
  int elixir_limit_;       // 圣水数量上限
  int elixir_growth_rate;  // 圣水自动增加的速率

  int builder_;             // 当前建筑工人数量
  int builder_limit_;       // 建筑工人数量上限
  int builder_growth_rate;  // 建筑工人自动增加的速率
  // 上次记录的时间（毫秒级，用于计算离线时长）用int64_t类型,比longlong兼容性强
  int64_t lastRecordTime_;

  // 本地存储工具（cocos2d-x内置）
  cocos2d::UserDefault* userDefault_;
  // 定时器调度器（用于在线增长）
  cocos2d::Scheduler* scheduler_;

  // 初始化数据（程序启动时加载本地数据）
  void initData();

  // 启动在线计时器，自动增长圣水/金币/建筑工人
  void startOnlineTimer();

  std::vector<BuildingData> buildingDatas_;  // 建筑数据列表
  //////////////////////
  // 数据同步到本地文件的端口

  void syncGoldToLocal();
  void syncGoldGrowRateToLocal();
  void syncElixirToLocal();
  void syncElixirGrowRateToLocal();
  void syncBuilderToLocal();
  void syncBuilderGrowRateToLocal();
  void syncAllDataToLocal();
  void syncTimeToLocal();
  void syncBuildingToLocal();
};

#endif  // __PLAYER_DATA_MANAGER_H__