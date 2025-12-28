#include "AirBallonSoldier.h"

#include "Building/BuildingManager.h"
#include "Tools/AnimationManager.h"
#include "cocos2d.h"

using namespace cocos2d;

BallonSoldier* BallonSoldier::create(int hp, int attack, int attackrange,
                                     int attackCD) {
  BallonSoldier* ret = new (std::nothrow) BallonSoldier();
  if (ret && ret->init(hp, attack, attackrange, attackCD)) {
    ret->autorelease();
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

bool BallonSoldier::init(int hp, int attack, int attack_range, int attack_CD) {
  if (!Soldier::init("ballon_soldier_icon.png", hp, attack, attack_range,
                     attack_CD)) {
    return false;
  }

  // 初始化碰撞半径
  createPhysicsBody(35.0f);
  // 设置移动速度
  setSpeed(80.0f);
  // 加载动画配置
  loadAllAnimations();

  return true;
}

void BallonSoldier::loadAllAnimations() {
  SoldierAnimationConfig config;

  // Idle动画
  config.idle.framePrefix = "anim/bomberman_idle_";
  config.idle.frameCount = 3;
  config.idle.delayPerUnit = 0.1f;

  // Move动画
  config.move.framePrefix = "anim/bomberman_move_";
  config.move.frameCount = 1;
  config.move.delayPerUnit = 0.08f;

  // Attack动画
  config.attack.framePrefix = "anim/bomberman_attack_";
  config.attack.frameCount = 3;
  config.attack.delayPerUnit = 0.12f;

  // Die动画
  config.die.framePrefix = "anim/bomberman_die_";
  config.die.frameCount = 1;
  config.die.delayPerUnit = 0.1f;

  AnimationManager::getInstance()->registerAnimationConfig(
      SoldierType::kGroundBomberman, config);
}

void BallonSoldier::playIdleAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "idle");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(RepeatForever::create(animate));
  }
}

void BallonSoldier::playMoveAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "move");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(RepeatForever::create(animate));
  }
}

void BallonSoldier::playAttackAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "attack");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(animate);
  }
}

void BallonSoldier::playDieAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "die");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(animate);
  }
}

void BallonSoldier::takeDamage(int damage) {
  // 第一次死亡会自爆
  if ((!isDead()) && getHp() - damage <= 0) {
    // 死亡时停止所有动作并爆炸
    stopAllActions();
    this->explode();
  }
  Soldier::takeDamage(damage);
  // 可添加受击特效逻辑
}

void BallonSoldier::attackSoldier(Soldier* target) {
  if (!target) return;
  Soldier::attackSoldier(target);
  // 可添加攻击特效逻辑
}

BallonSoldier::~BallonSoldier() {
  // 不需要释放动画，由AnimationManager统一管理
}

void BallonSoldier::attackBuilding(Building* target) {
  if (!target || target->isDestroyed() || !is_attack_CD_ready_ || isDead()) {
    this->startAttack();  //  完全保留原有逻辑
    return;
  }

  is_attack_CD_ready_ = false;
  attack_CD_remaining_ = attack_CD_;

  changeState(SoldierState::kAttack);

  auto bombDropCallback = CallFunc::create([this, target]() {
    if (target && !target->isDestroyed() && !isDead()) {
      this->explode();  // 只替换伤害逻辑
      CCLOG(" Ballon bomb dropped!");
    }

    // 基类逻辑：回到Idle，update中会继续检查攻击
    changeState(SoldierState::kIdle);
  });

  // 完全保留原有的延迟
  runAction(Sequence::create(DelayTime::create(EXPLODE_DELAY), bombDropCallback,
                             nullptr));
}
void BallonSoldier::explode() {
  // 死前还会放一次炸弹
  // if (isDead()) return;

  CCLOG("BOMBERMAN EXPLODES!");

  // 1. 从 BuildingManager 获取所有建筑
  auto buildings = BuildingManager::getInstance()->getAllBuildings();

  // 2. 遍历所有敌方建筑，计算圆形范围内的
  for (auto* building : buildings) {
    if (building == nullptr || building->isDestroyed()) continue;

    float distance = this->getPosition().distance(building->getPosition());
    if (distance <= EXPLODE_RADIUS) {
      // 对范围内的建筑造成大量伤害
      building->takeDamage(EXPLODE_DAMAGE);
      CCLOG("Building hit by explosion: %d damage", EXPLODE_DAMAGE);
    }
  }

  // 3. 气球兵不自杀
}