
#include "DefenseBuilding.h"

#include "BuildingManager.h"  // 用于将建筑添加到管理器
#include "PlacementManager.h"
#include "Soldier.h"

DefenseBuilding* DefenseBuilding::create(
    const std::string& texPath, const std::string& name, CampType camp,
    int level, int maxLevel,  // 新增maxLevel
    int maxHP, float size, int upgradeCost, float upgradeTime, int dps,
    float attackRange, AttackType attackType, float attackCD,
    TargetType targetType) {
  DefenseBuilding* building = new (std::nothrow) DefenseBuilding();
  if (building && building->init(texPath, name, camp, level, maxLevel, maxHP,
                                 size, upgradeCost, upgradeTime, dps,
                                 attackRange, attackType, attackCD, targetType)) {
    building->autorelease();  // Cocos2d-x自动内存管理
    BuildingManager::getInstance()->addBuilding(building);  // 添加到建筑管理器
    return building;
  }
  CC_SAFE_DELETE(building);
  return nullptr;
}

bool DefenseBuilding::init(const std::string& texPath, const std::string& name,
                           CampType camp, int level, int maxLevel, int maxHP,
                           float size, int upgradeCost, float upgradeTime,
                           int dps, float attackRange, AttackType attackType,
                           float attackCD, TargetType targetType) {
  // 先调用父类Building的init方法（注意父类需要BuildingType参数，防御建筑类型为DEFENSE）
  if (!Building::init(texPath, name, camp, level, maxLevel, maxHP, size,
                      upgradeCost, upgradeTime, BuildingType::DEFENSE,
                      size / 2)) {  // 碰撞半径暂时设为size/2

    CCLOG("DefenseBuilding init failed: Building init failed");
    return false;
  }

  // 初始化防御建筑特有的成员变量
  _dps = dps;
  _attackRange = attackRange;
  _attackType = attackType;
  _attackCD = attackCD;
  _currentCD = 0.0f;  // 初始无冷却
  _target = nullptr;  // 初始无目标
  _targetType = targetType;  // 设置可以攻击的目标类型

  // 启用update调度器，以便定期检查和攻击目标
  this->scheduleUpdate();

  CCLOG("DefenseBuilding init success");
  return true;
}

void DefenseBuilding::findTarget() {
  // 获取PlacementManager实例
  auto pm = PlacementManager::getInstance();
  if (!pm) {
    return;
  }
  
  // 获取所有活跃士兵
  const auto& soldiers = pm->getSoldiers();
  
  // 清除当前目标（如果已死亡或不存在）
  if (_target && _target->isDead()) {
    _target = nullptr;
  }
  
  // 如果已经有有效目标，不需要重新寻找
  if (_target) {
    return;
  }
  
  // 寻找最近的敌人
  float minDistance = _attackRange;
  Soldier* nearestSoldier = nullptr;
  
  for (auto soldier : soldiers) {
    // 跳过已死亡的士兵
    if (!soldier || soldier->isDead()) {
      continue;
    }
    
    // 检查是否是敌方士兵（根据阵营判断）
    // 假设防御建筑属于玩家阵营，攻击敌方士兵
    // 这里需要根据实际游戏逻辑调整阵营判断
    
    
    // 根据防御建筑的目标类型筛选士兵
    SoldierMoveType soldierMoveType = soldier->getSoldierMoveType();
    bool canAttack = false;
    
    switch (_targetType) {
      case TargetType::GROUND_ONLY:
        canAttack = (soldierMoveType == SoldierMoveType::kGround);
        break;
      case TargetType::AIR_ONLY:
        canAttack = (soldierMoveType == SoldierMoveType::kAir);
        break;
      case TargetType::BOTH:
        canAttack = (soldierMoveType == SoldierMoveType::kGround || 
                    soldierMoveType == SoldierMoveType::kAir);
        break;
    }
    
    if (!canAttack) continue;  // 跳过不能攻击的目标类型
    
    // 计算与士兵的距离
    float distance = this->getPosition().distance(soldier->getPosition());
    
    // 如果在攻击范围内且距离更近，则更新目标
    if (distance <= minDistance) {
      minDistance = distance;
      nearestSoldier = soldier;
    }
  }
  
  // 设置找到的目标
  _target = nearestSoldier;
}

void DefenseBuilding::attackTarget() {
  // 检查是否有有效目标
  if (!_target || _target->isDead()) {
    _target = nullptr;
    return;
  }
  
  // 检查目标是否仍在攻击范围内
  float distance = this->getPosition().distance(_target->getPosition());
  if (distance > _attackRange) {
    _target = nullptr;
    return;
  }
  
  // 检查攻击冷却
  if (_currentCD > 0) {
    return;  // 攻击冷却中
  }
  
  // 执行攻击
  _currentCD = _attackCD;  // 设置攻击冷却
  
  // 对目标造成伤害
  int damage = static_cast<int>(_dps * _attackCD);  // 计算本次攻击伤害
  _target->takeDamage(damage);
  
  // 创建攻击效果（可选）
  // 这里可以添加攻击特效、音效等
  
  // 检查目标是否死亡
  if (_target->isDead()) {
    _target = nullptr;
  }
}

void DefenseBuilding::enableAttackCD(bool enable) {
    if (enable) {
        this->schedule(
            [=](float dt) {
                if (!is_attack_CD_ready_ && attack_CD_remaining_ > 0) {
                    // 减少剩余冷却
                    attack_CD_remaining_ -= dt;

                    if (attack_CD_remaining_ <= 0) {
                        is_attack_CD_ready_ = true;
                        attack_CD_remaining_ = 0.0;
                        CCLOG("当前攻击冷却结束");
                    }
                }
            },
            // 定时器回调函数,直接使用lambda表达式，请勿使用bind,会导致unschedule无效
            0.0f,               // 定时器执行间隔，每帧检测
            CC_REPEAT_FOREVER,  // 定时器重复执行次数
            0.0f,               // 定时器延迟执行时间（单位：秒）
            AttackCDSchedulerKey);
        CCLOG("攻击冷却检测已启用");
    }
    else {
        this->unschedule(AttackCDSchedulerKey);
        CCLOG("攻击冷却检测已禁用");
    }
}

void DefenseBuilding::update(float dt) {
  // 先调用父类的更新方法
  Building::update(dt);
  
  // 如果建筑已被摧毁，不执行攻击逻辑
  if (_isDestroyed) {
    return;
  }
  
  // 更新攻击冷却
  if (_currentCD > 0) {
    _currentCD -= dt;
    if (_currentCD < 0) {
      _currentCD = 0;
    }
  }
  
  // 如果没有目标，寻找新目标
  if (!_target) {
    findTarget();
  }
  
  // 如果有目标，尝试攻击
  if (_target) {
    attackTarget();
  }
}

DefenseBuilding::~DefenseBuilding() {
  // 清理资源（如果需要）
  _target = nullptr;
  
  // 停止update调度器
  this->unscheduleUpdate();
}
