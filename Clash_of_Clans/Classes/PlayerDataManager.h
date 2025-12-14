#ifndef __PLAYER_DATA_MANAGER_H__
#define __PLAYER_DATA_MANAGER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "TimeTools.h"
#include "cocos2d.h"
class PlayerDataManager : public cocos2d::Ref {
 public:
  // 获取实例（全局只有这一个）
  static PlayerDataManager* getInstance();
  // 销毁单例（程序退出时调用）
  static void destroyInstance();

  /////////////////////////////
  //   获取资源的数量的接口，以及设置资源的接口

  // 获取金币数量
  int getGold() const;
  // 设置金币数量（保存到本地）
  void setGold(int gold);
  // 设置金币上限（保存到本地）
  void setGoldLimit(int limit);

  // 获取圣水数量
  int getElixir() const;
  // 设置圣水数量（保存到本地）
  void setElixir(int elixir);
  // 设置圣水上限（保存到本地）
  void setElixirLimit(int limit);

  // 获取建筑工人数量
  int getBuilder() const;
  // 设置建筑工人数量（保存到本地）
  void setBuilder(int builder);
  // 设置建筑工人上限（保存到本地）
  void setBuilderLimit(int limit);

  // 重置所有数据的数值（创建实例时/更换存档调用）
  void resetAllData();

  // 搭配scheduler进行圣水实时更新
  void updateOnlineElixir(float dt);

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
  const std::string kKeyElixir = "player_elixir";
  const std::string kKeyElixirLimit = "elixir_limit";
  const std::string kKeyBuilder = "player_builder";
  const std::string kKeyBuilderLimit = "builder_limit";
  const std::string kKeyLastRecordTime = "last_record_time";

  // 记录各项数值，存在实例之中，并非本地文件
  int gold_;
  int gold_limit_;
  int elixir_;
  int elixir_limit_;
  int builder_;
  int builder_limit_;
  // 上次记录的时间（毫秒级，用于计算离线时长）用int64_t类型,比longlong兼容性强
  int64_t lastRecordTime_;
  // 圣水自动增加的速率（还需要乘上离线/在线的检测时间，可能是每分钟一次,或者每分钟6次之类）
  const int kElixirGrowthRate = 1;

  // 本地存储工具（cocos2d-x内置）
  cocos2d::UserDefault* userDefault_;
  // 定时器调度器（用于在线增长）
  cocos2d::Scheduler* scheduler_;

  // 初始化数据（程序启动时加载本地数据）
  void initData();

  // 启动在线计时器，自动增长圣水
  void startOnlineElixirTimer();

  //////////////////////
  // 数据同步到本地文件的端口

  void syncGoldToLocal();
  void syncGoldLimitToLocal();
  void syncElixirToLocal();
  void syncElixirLimitToLocal();
  void syncBuilderToLocal();
  void syncBuilderLimitToLocal();
  void syncAllDataToLocal();
  void syncTimeToLocal();
};

#endif  // __PLAYER_DATA_MANAGER_H__