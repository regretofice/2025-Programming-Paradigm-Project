#ifndef __PLAYER_DATA_MANAGER_H__
#define __PLAYER_DATA_MANAGER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "cocos2d.h"
class PlayerDataManager {
 public:
  // 获取实例（全局只有这一个）
  static PlayerDataManager* getInstance();
  // 销毁单例（程序退出时调用，可选）
  static void destroyInstance();

  /////////////////////////////
  //   获取资源的数量的接口，以及设置资源的接口

  // 获取金币数量
  int getGold() const;
  // 设置金币数量（保存到本地）
  void setGold(int gold);

  // 获取圣水数量
  int getElixir() const;
  // 设置圣水数量（保存到本地）
  void setElixir(int elixir);

  // 获取建筑工人数量
  int getBuilder() const;
  // 设置建筑工人数量（保存到本地）
  void setBuilder(int builder);

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
  const std::string kKeyGlod = "player_gold";
  const std::string kKeyElixir = "player_elixir";
  const std::string kKeyBuilder = "player_builder";

  // 记录各项数值，存在实例之中，并非本地文件
  int gold_;
  int elixir_;
  int builder_;

  // 本地存储工具（cocos2d-x内置）
  cocos2d::UserDefault* userDefault_;

  // 初始化数据（程序启动时加载本地数据）
  void initData();

  //////////////////////
  // 数据同步到本地文件的端口

  void syncGoldToLocal();
  void syncElixirToLocal();
  void syncBuilderToLocal();
};

#endif  // __PLAYER_DATA_MANAGER_H__