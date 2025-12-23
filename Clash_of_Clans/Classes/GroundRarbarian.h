#ifndef __GROUND_RARBARIAN_H__
#define __GROUND_RARBARIAN_H__

#include "GroundSoldier.h"

class Rarbarian : public GroundSoldier {
 public:
  static Rarbarian* create(int hp, int attack, int attack_range, int attack_CD);
  bool init(int hp, int attack, int attack_range, int attack_CD);

  // 纯虚接口实现
  void loadAllAnimations() override;  // 改为“绑定全局动画”，而非创建
  SoldierType getSoldierType() const override {
    return SoldierType::kGroundRarbarian;
  }
  SoldierMoveType getSoldierMoveType() const override {
      return SoldierMoveType::kGround;
  }
  // 职业逻辑
  void takeDamage(int damage);
  void attackSoldier(Soldier* target) override;

  ~Rarbarian() override;  // 子类析构不再释放动画（全局缓存统一管理）
 protected:
  Rarbarian() = default;

  // 动画播放实现（直接使用全局动画）
  void playIdleAnimation() override;
  void playMoveAnimation() override;
  void playAttackAnimation() override;
  void playDieAnimation() override;

  // 动画key常量（统一命名，避免硬编码）
  const std::string kAnimIdleKey = "rarbarian_idle";
  const std::string kAnimMoveKey = "rarbarian_move";
  const std::string kAnimAttackKey = "rarbarian_attack";
  const std::string kAnimDieKey = "rarbarian_die";
};

#endif  // __GROUND_RARBARIAN_H__