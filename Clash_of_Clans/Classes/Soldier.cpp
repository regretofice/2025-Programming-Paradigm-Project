#include "Soldier.h"

#include "PlacementManager.h"
bool Soldier::init(int hp, int attack, int attack_range, int attack_CD) {
  // if (!Sprite::init()) {
  //   return false;
  // }
  if (!Sprite::initWithFile("rarbarian_idle_01.png")) return false;
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

  loadAllAnimations();
  // 初始化时启用攻击冷却检测（绑定调度器）
  enableAttackCD(true);
  //  确保ActionManager可用
  scheduleUpdate();
  return true;
}

void Soldier::takeDamage(int damage) {
  if (isDead()) return;  // 死亡了就不计算伤害

  hp_ -= damage;
  if (hp_ <= 0) {
    hp_ = 0;
    enableAttackCD(false);            // 死亡后禁用冷却检测
    changeState(SoldierState::kDie);  // 切换为死亡状态

    // 延迟回收到对象池
    auto delay = DelayTime::create(2.0f);
    auto callback = CallFunc::create(
        [this]() { PlacementManager::getInstance()->removeSoldier(this); });
    runAction(Sequence::create(delay, callback, nullptr));
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
      target->takeDamage(attack_);
      CCLOG("对目标造成%.1f伤害", attack_);
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
        [=](float dt) {
          if (!is_attack_CD_ready_ && attack_CD_remaining_ > 0) {
            // 减少剩余冷却
            attack_CD_remaining_ -= dt;

            if (attack_CD_remaining_ <= 0) {
              is_attack_CD_ready_ = true;
              attack_CD_remaining_ = 0.0;
              CCLOG("当前士兵攻击冷却结束");
            }
          }
        },
        // 定时器回调函数,直接使用lambda表达式，请勿使用bind,会导致unschedule无效
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
void Soldier::attackBuilding(Building* target) {
  if (!target || isDead() || target->isDestroyed() || !is_attack_CD_ready_)
    return;

  // 进入攻击冷却
  is_attack_CD_ready_ = false;
  attack_CD_remaining_ = attack_CD_;
  changeState(SoldierState::kAttack);

  CallFunc* attackHitCallBack = CallFunc::create([=]() {
    if (target && !target->isDestroyed()) {
      target->takeDamage(attack_);
      CCLOG("对建筑造成%d伤害", attack_);

      // 如果建筑被摧毁，回到 idle 状态
      if (target->isDestroyed()) {
        setTargetBuilding(nullptr);
        changeState(SoldierState::kIdle);
      }
    }
  });

  runAction(
      Sequence::create(DelayTime::create(0.5f), attackHitCallBack, nullptr));
}

void Soldier::calculatePath(const Vec2& targetPos) {
  // 简单的直线路径（实际项目中应替换为A*等寻路算法）
  path_points_.clear();
  current_path_index_ = 0;

  // 添加起点和终点
  path_points_.push_back(getPosition());
  path_points_.push_back(targetPos);
}

void Soldier::startAutoAttack(Building* target) {
  if (!target || target->isDestroyed()) return;

  setTargetBuilding(target);
  calculatePath(target->getPosition());
  current_path_index_ = 0;

  // 开始移动到第一个路径点
  if (!path_points_.empty()) {
    moveToNextPathPoint();
  }
}

// 新增：移动到下一个路径点
void Soldier::moveToNextPathPoint() {
  if (current_path_index_ >= path_points_.size() - 1) {
    // 到达最后一个点，开始攻击
    if (target_building_ && !target_building_->isDestroyed()) {
      attackBuilding(target_building_);
    }
    return;
  }

  current_path_index_++;
  Vec2 nextPos = path_points_[current_path_index_];

  // 移动到下一个点，完成后继续移动
  changeState(SoldierState::kMove);

  float distance = getPosition().distance(nextPos);
  float moveTime = distance / speed_;

  MoveTo* moveAction = MoveTo::create(moveTime, nextPos);
  CallFunc* moveEndCallBack =
      CallFunc::create([=]() { moveToNextPathPoint(); });

  runAction(Sequence::create(moveAction, moveEndCallBack, nullptr));
}
void Soldier::setSpeed(float speed) { speed_ = speed; }
void Soldier::createPhysicsBody(float radius) {
  // 仅设置碰撞半径，用于手动检测
  setCollisionRadius(radius);
  scheduleUpdate();  // 开启update检测
}
void Soldier::update(float dt) {
  Node::update(dt);

  // 简化的碰撞分离逻辑
  Vec2 myPos = getPosition();
  float myRadius = getCollisionRadius();

  auto scene = Director::getInstance()->getRunningScene();
  for (Node* child : scene->getChildren()) {
    Soldier* other = dynamic_cast<Soldier*>(child);
    if (other && other != this && !other->isDead()) {
      float dist = myPos.distance(other->getPosition());
      float minDist = myRadius + other->getCollisionRadius();
      if (dist < minDist && dist > 5.0f) {
        Vec2 dir = (myPos - other->getPosition()).getNormalized();
        setPosition(myPos + dir * (minDist - dist) * 0.5f);
      }
    }
  }
}