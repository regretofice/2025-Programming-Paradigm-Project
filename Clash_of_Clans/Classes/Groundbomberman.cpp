#include "Groundbomberman.h"

#include "AnimationManager.h"
#include "BuildingManager.h"
#include "cocos2d.h"

using namespace cocos2d;

Bomberman* Bomberman::create(int hp, int attack, int attackrange,
                             int attackCD) {
  Bomberman* ret = new (std::nothrow) Bomberman();
  if (ret && ret->init(hp, attack, attackrange, attackCD)) {
    ret->autorelease();
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

bool Bomberman::init(int hp, int attack, int attack_range, int attack_CD) {
  if (!Soldier::init("bomberman_icon.png", hp, attack, attack_range,
                     attack_CD)) {
    return false;
  }

  // 初始化碰撞半径
  createPhysicsBody(15.0f);
  // 设置移动速度
  setSpeed(120.0f);
  // 加载动画配置
  loadAllAnimations();

  return true;
}

void Bomberman::loadAllAnimations() {
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

void Bomberman::playIdleAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "idle");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(RepeatForever::create(animate));
  }
}

void Bomberman::playMoveAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "move");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(RepeatForever::create(animate));
  }
}

void Bomberman::playAttackAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "attack");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(animate);
  }
}

void Bomberman::playDieAnimation() {
  auto anim =
      AnimationManager::getInstance()->getAnimation(getSoldierType(), "die");
  if (anim) {
    auto animate = Animate::create(anim);
    runAction(animate);
  }
}

void Bomberman::takeDamage(int damage) {
  Soldier::takeDamage(damage);
  // 可添加受击特效逻辑
}

void Bomberman::attackSoldier(Soldier* target) {
  if (!target) return;
  Soldier::attackSoldier(target);
  // 可添加攻击特效逻辑
}

Bomberman::~Bomberman() {
  // 不需要释放动画，由AnimationManager统一管理
}

void Bomberman::attackBuilding(Building* target) {
  if (!target || isDead() || target->isDestroyed()) {
    // 目标无效，直接找下一个
    this->startAttack();
    return;
  }

  //  停止移动，进入自爆准备状态
  this->stopAllActions();
  changeState(SoldierState::kAttack);

  // 2秒延迟后执行爆炸
  auto explodeCallback = CallFunc::create([this, target]() {
    if (isDead()) return;  // 路上被打死了就不炸了

    // 执行爆炸逻辑：范围伤害 + 自杀
    this->explode();
  });

  this->runAction(Sequence::create(DelayTime::create(EXPLODE_DELAY),
                                   explodeCallback, nullptr));
}
void Bomberman::explode() {
  if (isDead()) return;

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

  // 3. 自杀（触发死亡动画和移除）
  this->takeDamage(500);  // 确保必死
}