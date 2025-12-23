#include "Soldier.h"

#include "GridPathFinder.h"
#include "PlacementManager.h"
bool Soldier::init(int hp, int attack, int attack_range, int attack_CD) {
  if (!Sprite::initWithFile("rarbarian_icon.png")) {
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
    enableAttackCD(false);  // 死亡后禁用冷却检测
    this->stopAllActions();
    changeState(SoldierState::kDie);  // 切换为死亡状态

    // 告诉活跃对象池，该士兵已死
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
  if (!target || target->isDestroyed() || !is_attack_CD_ready_) return;

  is_attack_CD_ready_ = false;
  attack_CD_remaining_ = attack_CD_;
  changeState(SoldierState::kAttack);

  auto attackHitCallBack = CallFunc::create([this, target]() {
    if (target && !target->isDestroyed()) {
      target->takeDamage(this->attack_);
      // 攻击完后切回 Idle，由 update() 根据 CD 决定何时发起下一次攻击
      this->changeState(SoldierState::kIdle);
    } else {
      // 目标死了，找下一个
      this->startAttack();
    }
  });

  // 假设动画时长 0.5s，伤害发生点也在 0.5s
  this->runAction(
      Sequence::create(DelayTime::create(0.5f), attackHitCallBack, nullptr));
}

void Soldier::recalculatePathTo(Building* target) {
  if (!target) return;

  auto pm = PlacementManager::getInstance();
  Vec2 startTile = pm->worldToTile(getPosition());
  Vec2 goalTile = pm->worldToTile(target->getPosition());

  int sx = std::max(0, std::min((int)startTile.x, pm->getMapWidth() - 1));
  int sy = std::max(0, std::min((int)startTile.y, pm->getMapHeight() - 1));
  int gx = std::max(0, std::min((int)goalTile.x, pm->getMapWidth() - 1));
  int gy = std::max(0, std::min((int)goalTile.y, pm->getMapHeight() - 1));

  int mapW = pm->getMapWidth();
  int mapH = pm->getMapHeight();
  GridPathFinder finder(mapW, mapH);

  // 1. 先假定所有格子都可走
  for (int x = 0; x < mapW; ++x) {
    for (int y = 0; y < mapH; ++y) {
      finder.setWalkable(x, y, true);
    }
  }

  // 2. 遍历场景中的建筑，用碰撞半径“画”出不可走区域
  auto scene = cocos2d::Director::getInstance()->getRunningScene();
  if (scene) {
    float myRadius = getCollisionRadius();

    for (auto child : scene->getChildren()) {
      auto building = dynamic_cast<Building*>(child);
      if (!building || building->isDestroyed()) continue;

      float bRadius = building->getCollisionRadius();
      float blockRadius = bRadius + myRadius + 10.0f;

      // 以建筑为圆心，在一定范围内枚举所有 tile
      cocos2d::Vec2 bWorldPos = building->getPosition();
      cocos2d::Vec2 bTilePos = pm->worldToTile(bWorldPos);
      int bx = (int)bTilePos.x;
      int by = (int)bTilePos.y;

      int maxTileOffset = (int)std::ceil(
          blockRadius /
          pm->getTileSize());  // 需要在PlacementManager里加个getTileSize()

      for (int dx = -maxTileOffset; dx <= maxTileOffset; ++dx) {
        for (int dy = -maxTileOffset; dy <= maxTileOffset; ++dy) {
          int tx = bx + dx;
          int ty = by + dy;
          if ((tx == sx && ty == sy) || (tx == gx && ty == gy)) {
            continue;  // 起点或终点，不标记为障碍
          }
          if (tx < 0 || tx >= mapW || ty < 0 || ty >= mapH) continue;

          // 把 tile 中心转换到世界坐标，检查与建筑碰撞圆是否重叠
          cocos2d::Vec2 tileWorld = pm->tileToWorldCenter((float)tx, (float)ty);
          float dist = tileWorld.distance(bWorldPos);
          if (dist < blockRadius) {
            // 该格子对当前士兵来说是不可走的
            finder.setWalkable(tx, ty, false);
          }
        }
      }
    }
  }
  // 3. 用生成好的阻挡网格跑 A*
  bool ok;
  std::vector<cocos2d::Vec2> tiles;
  std::tuple<bool, std::vector<cocos2d::Vec2>> result =
      finder.findPath(sx, sy, gx, gy);
  ok = std::get<0>(result);
  tiles = std::get<1>(result);
  pathTiles_.clear();
  if (!ok) {
    CCLOG("No path found from (%d,%d) to (%d,%d)", sx, sy, gx, gy);
    return;
  }
  CCLOG("Path tiles size = %d", (int)tiles.size());
  for (size_t i = 0; i < tiles.size(); ++i) {
    CCLOG("  step %d: (%.0f, %.0f)", (int)i, tiles[i].x, tiles[i].y);
  }

  pathTiles_ = tiles;  // tiles 里是 tile 坐标
  currentPathIndex_ = 0;
}

// 新增：移动到下一个路径点
void Soldier::moveToNextPathPoint() {
  // 1. 安全检查：如果自己死了，或者目标建筑已经不在了，重找目标
  if (isDead()) return;

  if (!target_building_ || target_building_->isDestroyed()) {
    CCLOG("Target lost or destroyed, finding next...");
    this->startAttack();  // startAttack 负责寻找新目标并重新开启寻路
    return;
  }

  // 2. 核心逻辑：检查是否进入攻击距离
  // 距离计算：士兵中心到建筑中心的距离
  float dist = getPosition().distance(target_building_->getPosition());

  // 判定：攻击距离 + 建筑半径（建筑是40像素半径）
  // 如果进入射程，停止移动，开启攻击动作
  if (dist <= (this->attack_range_ + 40.0f)) {
    this->stopAllActions();
    this->attackBuilding(target_building_);
    return;
  }

  // 3.
  // 路径点检查：如果已经走完了所有路径点还没到攻击距离，说明路被堵死或目标不可达
  if (pathTiles_.empty() || currentPathIndex_ >= (int)pathTiles_.size()) {
    CCLOG("Path finished but not in range, recalculating...");
    this->recalculatePathTo(target_building_);  // 尝试重新规划一次路径
    return;
  }

  // 4. 执行移动：向下一个 A* 路径点迈进
  auto pm = PlacementManager::getInstance();
  Vec2 nextTile = pathTiles_[currentPathIndex_];
  Vec2 worldPos = pm->tileToWorldCenter(nextTile.x, nextTile.y);

  currentPathIndex_++;  // 索引指向下一个

  changeState(SoldierState::kMove);

  float moveDistance = getPosition().distance(worldPos);
  float moveTime = moveDistance / speed_;

  auto moveAction = MoveTo::create(moveTime, worldPos);
  // 递归调用：到达这个点后，继续检查是否到射程，或者走向下一个点
  auto callback = CallFunc::create([this]() { this->moveToNextPathPoint(); });

  this->runAction(Sequence::create(moveAction, callback, nullptr));
}

void Soldier::setSpeed(float speed) { speed_ = speed; }
void Soldier::createPhysicsBody(float radius) {
  // 仅设置碰撞半径，用于手动检测
  setCollisionRadius(radius);
  scheduleUpdate();  // 开启update检测
}
void Soldier::update(float dt) {
  Node::update(dt);
  if (isDead()) return;

  // 第一步：进行碰撞检测，防止重叠
  Vec2 myPos = this->getPosition();
  float myRadius = this->getCollisionRadius();
  Vec2 totalCorrection(0, 0);

  for (auto child : this->getParent()->getChildren()) {
    if (child == this || !child->isVisible()) continue;

    // 仅保留士兵间的挤开，防止叠罗汉
    auto otherSoldier = dynamic_cast<Soldier*>(child);
    if (otherSoldier && !otherSoldier->isDead()) {
      float dist = myPos.distance(otherSoldier->getPosition());
      float minDist = myRadius + otherSoldier->getCollisionRadius();
      if (dist < minDist && dist > 0.1f) {
        totalCorrection +=
            (myPos - otherSoldier->getPosition()).getNormalized() *
            (minDist - dist) * 0.7f;
      }
    }
  }

  if (totalCorrection != Vec2::ZERO) {
    this->setPosition(myPos + totalCorrection);
  }

  // 第二步：更新 CD 计时器
  if (!is_attack_CD_ready_) {
    attack_CD_remaining_ -= dt;
    if (attack_CD_remaining_ <= 0) {
      is_attack_CD_ready_ = true;
      attack_CD_remaining_ = 0;
    }
  }

  // 第三步：核心决策逻辑（攻击循环）
  //  只有在位移修正完成后，这里的 getPosition() 才是准确的
  if (current_state_ == SoldierState::kIdle && is_attack_CD_ready_ &&
      target_building_) {
    float dist = getPosition().distance(target_building_->getPosition());

    if (dist <= (attack_range_ + Building::getCollisionRadius())) {
      if (!target_building_->isDestroyed()) {
        this->attackBuilding(target_building_);
      } else {
        this->startAttack();
      }
    } else if (current_state_ != SoldierState::kMove) {
      // 如果不在射程，且不在移动中，可能需要重新寻路靠近
      this->recalculatePathTo(target_building_);
    }
  }
}

Building* Soldier::findBestTargetBuilding() {
  // 不要直接用 runningScene，而是搜索士兵所在的父节点（MapScene层）
  auto parent = this->getParent();
  if (!parent) return nullptr;

  Building* best = nullptr;
  float bestDist = FLT_MAX;
  auto myPos = getPosition();

  //  在父节点（MapScene）的子节点中遍历，这样才能找到建筑
  for (auto child : parent->getChildren()) {
    auto b = dynamic_cast<Building*>(child);
    if (!b || b->isDestroyed()) continue;

    float d = myPos.distance(b->getPosition());
    if (d < bestDist) {
      bestDist = d;
      best = b;
    }
  }
  return best;
}
void Soldier::startAttack() {
  if (isDead()) return;

  // 1. 寻找最近的敌方建筑
  Building* nextTarget = this->findBestTargetBuilding();

  if (nextTarget) {
    this->setTargetBuilding(nextTarget);
    // 2. 重新计算路径
    this->recalculatePathTo(nextTarget);
    // 3. 开始移动
    this->moveToNextPathPoint();
  } else {
    // 4. 地面上没有建筑了，进入待机
    this->changeState(SoldierState::kIdle);
    CCLOG("No more targets on map.");
  }
}