#ifndef BUILDING_ENUMS_H
#define BUILDING_ENUMS_H

// 阵营归属
enum class CampType {
  PLAYER,  // 玩家阵营
  ENEMY    // 敌方阵营
};

// 建筑类型
enum class BuildingType {
  COMMON,    // 普通建筑（城墙等）
  DEFENSE,   // 防御建筑（箭塔、加农炮等）
  RESOURCE,  // 资源建筑（金矿、圣水收集器等）
  STORAGE    // 存储建筑（储金罐、圣水瓶等）
};

// 防御建筑攻击方式
enum class AttackType {
  SINGLE_TARGET,  // 单体攻击
  AREA_DAMAGE,    // 范围伤害
};

// 防御建筑可以攻击的目标类型
enum class TargetType {
	GROUND_ONLY,  // 只能攻击地面目标
	AIR_ONLY,     // 只能攻击空中目标
	BOTH,         // 可以攻击地面和空中目标
	NONE          // 无目标
};

// 资源类型
enum class ResourceType {
  GOLD,     // 金币
  ELIXIR,   // 圣水
  BUILDER,  // 建筑工人
  NORESOURCE
};

#endif