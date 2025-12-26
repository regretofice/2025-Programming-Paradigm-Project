#ifndef __GROUND_BOMBERMAN_H__
#define __GROUND_BOMBERMAN_H__

#include "Soldier.h"

class Bomberman : public Soldier {
 public:
  static Bomberman* create(int hp, int attack, int attack_range, int attack_CD);
  bool init(int hp, int attack, int attack_range, int attack_CD);

  // 纯虚接口实现
  void loadAllAnimations() override;  // 改为“绑定全局动画”，而非创建
  SoldierType getSoldierType() const override {
    return SoldierType::kGroundBomberman;
  }
  SoldierMoveType getSoldierMoveType() const override {
    return SoldierMoveType::kGround;
  }
  // 职业逻辑
  void takeDamage(int damage);
  void attackSoldier(Soldier* target) override;
  void attackBuilding(Building* target) override;

  ~Bomberman() override;  // 子类析构不再释放动画（全局缓存统一管理）
 protected:
  Bomberman() = default;

  // 爆炸（进行攻击）
  void explode();

  // 动画播放实现（直接使用全局动画）
  void playIdleAnimation() override;
  void playMoveAnimation() override;
  void playAttackAnimation() override;
  void playDieAnimation() override;

  // 动画key常量（统一命名，避免硬编码）
  const std::string kAnimIdleKey = "bomberman_idle";
  const std::string kAnimMoveKey = "bomberman_move";
  const std::string kAnimAttackKey = "bomberman_attack";
  const std::string kAnimDieKey = "bomberman_die";

  const float EXPLODE_DELAY = 2.0f;  // 爆炸延迟
  const int EXPLODE_RADIUS = 200;    // 爆炸半径
  const int EXPLODE_DAMAGE = 100;    // 爆炸伤害
};

#endif  // __GROUND_BOMBERMAN_H__