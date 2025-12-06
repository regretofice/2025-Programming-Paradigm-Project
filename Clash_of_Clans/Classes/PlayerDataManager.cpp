#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "PlayerDataManager.h"

// 使用别名进行缩写，请勿在.h文件中使用
using PDM = PlayerDataManager;
// 对static变量进行类外初始化
PDM* PDM::instance_ = nullptr;

PDM* PDM::getInstance() {
  if (instance_ == nullptr) {
    // std::nothrow禁用报错，哪怕内存申请失败
    instance_ = new (std::nothrow) PlayerDataManager();
  }
  return instance_;
}

void PDM::destroyInstance() {
  if (instance_) {
    delete instance_;
    instance_ = nullptr;
  }
}

PDM::PlayerDataManager() {
  // 获取UserDefault实例（全局唯一，cocos2d-x已封装）
  userDefault_ = cocos2d::UserDefault::getInstance();
  // 加载本地数据到内存
  initData();
}

// 析构时无需手动销毁_userDefault（引擎管理）
PDM::~PlayerDataManager() {}

// 初始化数据：从本地加载，无数据则设默认值
void PDM::initData() {
  int defaultGold = 0;
  int defaultElixir = 0;
  int defaultBuilder = 0;

  gold_ = userDefault_->getIntegerForKey(kKeyGlod.c_str(), defaultGold);
  elixir_ = userDefault_->getIntegerForKey(kKeyGlod.c_str(), defaultElixir);
  builder_ = userDefault_->getIntegerForKey(kKeyGlod.c_str(), defaultBuilder);

  CCLOG("玩家初始数据:\n金币:%d  圣水:%d  建筑工人:%d", gold_, elixir_,
        builder_);
}

int PDM::getGold() const { return gold_; }
int PDM::getElixir() const { return elixir_; }
int PDM::getBuilder() const { return builder_; }

void PDM::setGold(int gold) {
  if (gold < 0) {
    gold = 0;
  }
  gold_ = gold;
  syncGoldToLocal();
  CCLOG("金币已更新为：%d", gold_);
}
void PDM::setElixir(int elixir) {
  if (elixir < 0) {
    elixir = 0;
  }
  elixir_ = elixir;
  syncElixirToLocal();
  CCLOG("圣水已更新为：%d", elixir_);
}
void PDM::setBuilder(int builder) {
  if (builder < 0) {
    builder = 0;
  }
  builder_ = builder;
  syncBuilderToLocal();
  CCLOG("建筑工人已更新为：%d", builder_);
}

// 每次更新完数据之后要调用flush来保存数据，否则会丢失
void PDM::syncGoldToLocal() {
  userDefault_->setIntegerForKey(kKeyGlod.c_str(), gold_);
  userDefault_->flush();
}
void PDM::syncElixirToLocal() {
  userDefault_->setIntegerForKey(kKeyElixir.c_str(), elixir_);
  userDefault_->flush();
}
void PDM::syncBuilderToLocal() {
  userDefault_->setIntegerForKey(kKeyBuilder.c_str(), builder_);
  userDefault_->flush();
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