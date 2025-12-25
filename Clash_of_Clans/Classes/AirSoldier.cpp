#include "AirSoldier.h"

void AirSoldier::moveTo(const Vec2& targetPos, int speed) {
  if (isDead()) return;

  // 飞行单位不进行建筑物碰撞检测，正常移动（调用基类移动逻辑）
  Soldier::moveTo(targetPos, speed);
}