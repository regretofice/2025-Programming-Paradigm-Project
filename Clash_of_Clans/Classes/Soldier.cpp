#include "Soldier.h"

bool Soldier::init(int hp, int attack, int attack_range, int attack_CD) {
  if (!Sprite::init()) {
    return false;
  }

  // 初始化核心属性
  hp_ = hp;
  attack_ = attack;
  attack_range_ = attack_range;
  current_state_ = SoldierState::kIdle;
  attack_CD_ = attack_CD;
  is_attack_CD_ready_ = true;
  attack_CD_remaining_ = 0.0f;
  original_opacity_ = 255;
  is_preview_ = false;

  // 初始化时启用攻击冷却检测（绑定调度器）
  enableAttackCD(true);
  return true;
}

void Soldier::getDamage(int damage) {
  if (isDead()) return;  // 死亡了就不计算伤害

  hp_ -= damage;
  if (hp_ <= 0) {
    hp_ = 0;
    enableAttackCD(false);            // 死亡后禁用冷却检测
    changeState(SoldierState::kDie);  // 切换为死亡状态
  }
}

void Soldier::changeState(SoldierState newState) {
  if (current_state_ == newState) return;  // 状态一致，无需操作

  if (current_state_ == SoldierState::kDie) return;  // 死了也不操作

  stopAllStateActions();

  current_state_ = newState;

  switch (newState) {
    case SoldierState::kIdle: {
      playIdleAnimation();
      break;
    }
    case SoldierState::kMove: {
      playMoveAnimation();
      break;
    }
    case SoldierState::kAttack: {
      playAttackAnimation();
      break;
    }
    case SoldierState::kDie: {
      playDieAnimation();
      break;
    }
  }
}
void Soldier::moveTo(const Vec2& targetPos, int speed) {
  // 死亡状态不可移动
  if (isDead()) return;

  // 切换到移动状态
  changeState(SoldierState::kMove);

  // 计算移动时间
  float distance = getPosition().distance(targetPos);
  float moveTime = distance / speed;

  // 创建移动动作
  MoveTo* moveAction = MoveTo::create(moveTime, targetPos);
  // 移动完成后切换回站立状态
  CallFunc* moveEndCallBack =
      CallFunc::create([=]() { changeState(SoldierState::kIdle); });

  runAction(Sequence::create(moveAction, moveEndCallBack, nullptr));
}
void Soldier::attackSoldier(Soldier* target) {
  // 判断不攻击或者攻击处于冷却的情况
  if (!target || isDead() || target->isDead() || !is_attack_CD_ready_) return;

  // 进入攻击冷却状态
  is_attack_CD_ready_ = false;
  attack_CD_remaining_ = attack_CD_;
  changeState(SoldierState::kAttack);
  CallFunc* attackHitCallBack = CallFunc::create([=]() {
    if (target && !target->isDead()) {
      target->getDamage(attack_);
      log("对目标造成%.1f伤害", attack_);
    }
    changeState(SoldierState::kIdle);
  });
  runAction(
      Sequence::create(DelayTime::create(0.5f), attackHitCallBack, nullptr));
}
void Soldier::setPreviewMode(bool isPreview) {
  is_preview_ = isPreview;
  if (isPreview) {
    original_opacity_ = getOpacity();
    setOpacity(100);
    stopAllStateActions();
    enableAttackCD(false);  // 预览状态下不进行攻击
  } else {
    setOpacity(original_opacity_);
    enableAttackCD(true);  // 退出预览启用冷却
    changeState(SoldierState::kIdle);
  }
}
void Soldier::enableAttackCD(bool enable) {
  if (enable) {
    this->schedule(
        std::bind(&Soldier::updateAttackCD, this, std::placeholders::_1),
        // 回调函数
        0.0f,               // 定时器执行间隔，每帧检测
        CC_REPEAT_FOREVER,  // 定时器重复执行次数
        0.0f,               // 定时器延迟执行时间（单位：秒）
        kAttackCDSchedulerKey);
    CCLOG("攻击冷却检测已启用");
  } else {
    this->unschedule(kAttackCDSchedulerKey);
    CCLOG("攻击冷却检测已禁用");
  }
}

void Soldier::updateAttackCD(float deltaTime) {
  if (!is_attack_CD_ready_ && attack_CD_remaining_ > 0) {
    // 减少剩余冷却
    attack_CD_remaining_ -= deltaTime;

    if (attack_CD_remaining_ <= 0) {
      is_attack_CD_ready_ = true;
      attack_CD_remaining_ = 0.0;
      CCLOG("当前士兵攻击冷却结束");
    }
  }
}