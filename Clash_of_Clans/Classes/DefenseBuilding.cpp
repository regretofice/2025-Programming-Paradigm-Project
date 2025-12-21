
#include "DefenseBuilding.h"

#include "BuildingManager.h"  // 用于将建筑添加到管理器
#include "Soldier.h"

DefenseBuilding* DefenseBuilding::create(
    const std::string& texPath, const std::string& name, CampType camp,
    int level, int maxLevel,  // 新增maxLevel
    int maxHP, float size, int upgradeCost, float upgradeTime, int dps,
    float attackRange, AttackType attackType, float attackCD) {
  DefenseBuilding* building = new (std::nothrow) DefenseBuilding();
  if (building && building->init(texPath, name, camp, level, maxLevel, maxHP,
                                 size, upgradeCost, upgradeTime, dps,
                                 attackRange, attackType, attackCD)) {
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
                           float attackCD) {
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

  CCLOG("DefenseBuilding init success");
  return true;
}

void DefenseBuilding::findTarget() {
  // 这里实现寻找攻击范围内敌人的逻辑（示例）
  // 1. 获取所有敌人（需要敌人管理器，假设存在EnemyManager）
  // 2. 筛选出在_attackRange范围内的敌人
  // 3. 设置第一个符合条件的敌人为_target
}

void DefenseBuilding::attackTarget() {
  if (!_target || _target->isDead()) {
    _target = nullptr;
    return;
  }
  // 对目标造成伤害（假设Enemy有takeDamage方法）
  _target->takeDamage(_dps *
                      _attackCD);  // 简化：每秒伤害*dps，实际需按帧率计算
  CCLOG("防御建筑[%s]对敌人造成伤害，当前目标剩余血量：%d", _name.c_str(),
        _target->getHp());
}

void DefenseBuilding::update(float dt) {
  Building::update(dt);  // 调用父类更新逻辑
  if (_isDestroyed) return;

  // 冷却时间更新
  if (_currentCD > 0) {
    _currentCD -= dt;
  } else {
    // 冷却结束，寻找目标并攻击
    findTarget();
    if (_target) {
      attackTarget();
      _currentCD = _attackCD;  // 重置冷却
    }
  }
}

DefenseBuilding::~DefenseBuilding() {
  // 清理资源（如果需要）
  _target = nullptr;
}
