#ifndef __SOLDIER_H__
#define __SOLDIER_H__
#include <algorithm>  // for std::find
#include <memory>
#include <unordered_map>
#include <vector>

#include "Building.h"
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
  // 调整移动速度
  void setSpeed(float speed);
  // 返回当前血量
  int getHp() const { return hp_; }
  void setHp(int hp) { hp_ = hp; }
  // 返回当前攻击力
  int getAttack() const { return attack_; }
  void setAttack(int attack) { attack_ = attack; }
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

  // 攻击士兵
  virtual void attackSoldier(Soldier* target);

  // 攻击建筑
  virtual void attackBuilding(Building* target);

  // 设置目标建筑
  void setTargetBuilding(Building* building) { target_building_ = building; }

  // 自动寻路到目标并攻击
  virtual void startAutoAttack(Building* target);

  void moveToNextPathPoint();

  ////////////////////
  // 碰撞相关接口
  void setCollisionRadius(float radius) { collision_radius_ = radius; };
  float getCollisionRadius() const { return collision_radius_; };
  void createPhysicsBody(float radius);
  void update(float dt);

  //////////
  // 动画相关接口
  // 加载所有动画资源（只需全局加载一次）（纯虚函数，子类中实现）
  virtual void loadAllAnimations() = 0;

  // 预览模式（降低透明度）
  void setPreviewMode(bool isPreview);

  // 攻击冷却控制,调度器相关
  // 启用/禁用攻击冷却检测
  void enableAttackCD(bool enable);

  ~Soldier() override = default;

 protected:
  Soldier() = default;

  // 士兵数值的初始化
  bool init(int hp, int attack, int attack_range, int attack_CD);

  // 各状态的动画播放逻辑（纯虚函数，需要在子类中实现）
  virtual void playIdleAnimation() = 0;
  virtual void playMoveAnimation() = 0;
  virtual void playAttackAnimation() = 0;
  virtual void playDieAnimation() = 0;

  // 停止当前所有动画和动作
  void stopAllStateActions() { stopAllActions(); };

  // 新增：路径点列表
  std::vector<Vec2> path_points_;
  // 新增：当前路径点索引
  int current_path_index_;
  // 新增：目标建筑
  Building* target_building_;

  // 新增：计算到目标的路径
  virtual void calculatePath(const Vec2& targetPos);

 private:
  int hp_;                      // 士兵血量
  int attack_;                  // 士兵攻击
  int attack_range_;            // 士兵攻击范围
  SoldierState current_state_;  // 士兵当前状态
  float speed_;                 // 士兵移动速度
  float attack_CD_ = 1.0f;      // 攻击冷却总时长（秒）

  // 碰撞半径
  float collision_radius_;

  // 攻击冷却相关
  bool is_attack_CD_ready_;    // 冷却是否就绪
  float attack_CD_remaining_;  // 剩余冷却时间

  // 预览模式相关
  float original_opacity_;
  bool is_preview_;

  // 调度器key（唯一标识，用于精准取消）
  const std::string kAttackCDSchedulerKey = "attackCD";
};

#endif  // __SOLDIER_H__