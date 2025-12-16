#include <memory>
#include <vector>

#include "cocos2d.h"
USING_NS_CC;
// 士兵状态（用于界定当前状态）
enum class SoldierState {
  kIdle,    // 站立/悬停
  kMove,    // 移动/飞行
  kAttack,  // 攻击
  kDie      // 死亡
};
// 士兵类型（用于区分天空/地面+职业）
enum class SoldierType {
  kGroundRarbarian,   // 地面-野蛮人
  kGroundGiant,       // 地面-巨人
  kGroundBomberman,   // 地面-炸弹人
  kGroundArcher,      // 地面-弓箭手
  kAirThunderDragon,  // 天空-雷电飞龙
  kAirBallonSoldier   // 天空-气球兵
};
class Soldier : public Sprite {
 public:
  ///////////////////
  // 通用属性操作属性操作

  // 受到伤害，数值为damage
  void takeDamage(int damage);
  // 返回当前血量
  int getHp() const { return hp_; }
  // 返回当前攻击力
  int getAttack() const { return attack_; }
  // 返回当前是否死亡
  bool isDead() const { return hp_ <= 0; }
  // 纯虚函数：获取士兵类型
  virtual SoldierType getSoldierType() const = 0;

  ////////////////
  // 状态机相关操作

  // 变更为新状态
  void changeState(SoldierState newState);
  // 返回当前状态
  SoldierState getCurrentState() { return current_state_; }

  ////////////
  // 行为接口

  // 移动到对应位置
  virtual void moveTo(const Vec2& targetPos, int speed);
  // 攻击建筑
  // void attackBuildings(const Building& building);

  // 攻击士兵
  virtual void attackSoldier(Soldier* target);

  //////////
  // 动画相关接口
  // 加载所有动画资源（只需全局加载一次）（纯虚函数，子类中实现）
  virtual void loadAllAnimations() = 0;

  // 预览模式（降低透明度）
  void setPreviewMode(bool isPreview);

  // 攻击冷却控制,调度器相关
  // 启用/禁用攻击冷却检测
  void enableAttackCD(bool enable);

 protected:
  Soldier() = default;
  ~Soldier() = default;

  // 士兵数值的初始化
  bool init(int hp, int attack, int attack_range, int attack_CD);

  // 各状态的动画播放逻辑（纯虚函数，需要在子类中实现）
  virtual void playIdleAnimation() = 0;
  virtual void playMoveAnimation() = 0;
  virtual void playAttackAnimation() = 0;
  virtual void playDieAnimation() = 0;

  // 停止当前所有动画和动作
  void stopAllStateActions() { stopAllActions(); };

 private:
  int hp_;                      // 士兵血量
  int attack_;                  // 士兵攻击
  int attack_range_;            // 士兵攻击范围
  SoldierState current_state_;  // 士兵当前状态
  float attack_CD_ = 1.0f;      // 攻击冷却总时长（秒）

  // 攻击冷却相关
  bool is_attack_CD_ready_;    // 冷却是否就绪
  float attack_CD_remaining_;  // 剩余冷却时间

  // 预览模式相关
  float original_opacity_;
  bool is_preview_;

  // 调度器key（唯一标识，用于精准取消）
  const std::string kAttackCDSchedulerKey = "attackCD";
};