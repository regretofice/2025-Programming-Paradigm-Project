#ifndef DEFENSE_BUILDING_H
#define DEFENSE_BUILDING_H

#include "Building.h"

// 前向声明敌人类（避免循环引用）
class Soldier;

class DefenseBuilding : public Building {
 private:
  // 防御建筑专属属性
  int _dps;                // 每秒伤害
  float _attackRange;      // 攻击范围（半径）
  AttackType _attackType;  // 攻击方式
  float _attackCD;         // 攻击冷却（秒）
  float _currentCD;        // 当前冷却
  Soldier* _target;        // 当前攻击目标

 public:
  static DefenseBuilding* create(const std::string& texPath,
                                 const std::string& name, CampType camp,
                                 int level, int maxLevel,  // 新增maxLevel
                                 int maxHP, float size, int upgradeCost,
                                 float upgradeTime, int dps, float attackRange,
                                 AttackType attackType, float attackCD);

  virtual bool init(const std::string& texPath, const std::string& name,
                    CampType camp, int level, int maxLevel, int maxHP,
                    float size, int upgradeCost, float upgradeTime, int dps,
                    float attackRange, AttackType attackType, float attackCD);

  // 专属方法
  void findTarget();                       // 寻找攻击范围内的敌人
  void attackTarget();                     // 攻击目标
  virtual void update(float dt) override;  // 重写更新逻辑

  // Getter
  int getDPS() const { return _dps; }
  float getAttackRange() const { return _attackRange; }

  virtual ~DefenseBuilding();
};

#endif  // DEFENSE_BUILDING_H