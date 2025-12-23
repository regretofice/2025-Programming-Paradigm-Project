#include "GroundRarbarian.h"

#include "AnimationManager.h"
#include "cocos2d.h"

using namespace cocos2d;

Rarbarian* Rarbarian::create(int hp, int attack, int attackrange,
                             int attackCD) {
  Rarbarian* ret = new (std::nothrow) Rarbarian();
  if (ret && ret->init(hp, attack, attackrange, attackCD)) {
    ret->autorelease();
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

bool Rarbarian::init(int hp, int attack, int attack_range, int attack_CD) {
  if (!Soldier::init("rarbarian_icon.png", hp, attack, attack_range,
                     attack_CD)) {
    return false;
  }

  // 初始化碰撞半径
  setCollisionRadius(30.0f);
  createPhysicsBody(getCollisionRadius());
  // 设置移动速度
  setSpeed(120.0f);
  // 加载动画配置
  loadAllAnimations();

  return true;
}

void Rarbarian::loadAllAnimations() {
  SoldierAnimationConfig config;

  // Idle动画
  config.idle.framePrefix = "anim/rarbarian_idle_";
  config.idle.frameCount = 3;  // rarbarian_idle_01.png
  config.idle.delayPerUnit = 0.1f;

  // Move动画
  config.move.framePrefix = "anim/rarbarian_move_";
  config.move.frameCount = 1;
  config.move.delayPerUnit = 0.08f;

  // Attack动画
  config.attack.framePrefix = "anim/rarbarian_attack_";
  config.attack.frameCount = 3;
  config.attack.delayPerUnit = 0.12f;

  // Die动画
  config.die.framePrefix = "anim/rarbarian_die_";
  config.die.frameCount = 1;
  config.die.delayPerUnit = 0.1f;

  AnimationManager::getInstance()->registerAnimationConfig(
      SoldierType::kGroundRarbarian, config);
}

void Rarbarian::playIdleAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "idle");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(RepeatForever::create(animate));
  }
}

void Rarbarian::playMoveAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "move");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(RepeatForever::create(animate));
  }
}

void Rarbarian::playAttackAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "attack");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(animate);
  }
}

void Rarbarian::playDieAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "die");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(animate);
  }
}

void Rarbarian::takeDamage(int damage) {
  Soldier::takeDamage(damage);
  // 可添加受击特效逻辑
}

void Rarbarian::attackSoldier(Soldier* target) {
  if (!target) return;
  Soldier::attackSoldier(target);
  // 可添加攻击特效逻辑
}

Rarbarian::~Rarbarian() {
  // 不需要释放动画，由AnimationManager统一管理
}
