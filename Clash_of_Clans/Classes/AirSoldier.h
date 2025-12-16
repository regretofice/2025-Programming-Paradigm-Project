#ifndef __AIR_SOLDIER_H__
#define __AIR_SOLDIER_H__

#include "soldier.h"

class AirSoldier : public Soldier {
 protected:
  AirSoldier() = default;
  ~AirSoldier() override = default;

  // 重写移动逻辑：飞行单位无需建筑物碰撞检测
  void moveTo(const Vec2& targetPos, int speed) override;
};

#endif  // !__AIR_SOLDIER_H__
