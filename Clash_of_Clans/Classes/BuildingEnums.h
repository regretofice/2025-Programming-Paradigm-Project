#ifndef BUILDING_ENUMS_H
#define BUILDING_ENUMS_H

// 阵营归属
enum class CampType {
  PLAYER,  // 玩家阵营
  ENEMY    // 敌方阵营
};

// 建筑类型
enum class BuildingType {
  COMMON,   // 普通建筑（大本营、城墙等）
  DEFENSE,  // 防御建筑（箭塔、加农炮等）
  RESOURCE, // 资源建筑（金矿、圣水收集器等）
  STORAGE   // 存储建筑（储金罐、圣水瓶等）
};

// 防御建筑攻击方式
enum class AttackType {
  SINGLE_TARGET,  // 单体攻击
  AREA_DAMAGE,    // 范围伤害
};

// 资源类型
enum class ResourceType {
  GOLD,    // 金币
  ELIXIR,  // 圣水
  NORESOURCE
};

#endif