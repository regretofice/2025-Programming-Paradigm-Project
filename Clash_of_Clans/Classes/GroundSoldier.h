#ifndef __GROUND_SOLDIER_H__
#define __GROUND_SOLDIER_H__

#include "soldier.h"

class GroundSoldier : public Soldier {
 protected:
  GroundSoldier() = default;
  ~GroundSoldier() override = default;

  // 重写移动逻辑：添加建筑碰撞检测,当前注释掉了,而且还要加上自动寻路
  void moveTo(const Vec2& targetPos, int speed) override;
};

#endif  // !__GROUND_SOLDIER_H__
