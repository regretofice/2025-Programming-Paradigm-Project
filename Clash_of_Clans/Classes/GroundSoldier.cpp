// #include "BuildingManager.h"
#include "GroundSoldier.h"

void GroundSoldier::moveTo(const Vec2& targetPos, int speed) {
  if (isDead()) return;

  // 核心检测：当前位置到目标位置的直线是否穿过建筑
  // Vec2 startPos = this->getPosition();  // 地面单位当前位置（世界坐标系）
  // bool isCrossBuilding =…………

  // if (isCrossBuilding) {
  //   CCLOG("地面单位：路径穿过建筑，无法移动！");
  //   changeState(UnitState::IDLE);  // 保持站立状态
  //   return;
  // }

  // 无建筑阻挡，正常移动（调用基类移动逻辑）
  Soldier::moveTo(targetPos, speed);
}