// #include "BuildingManager.h"
#include "GroundSoldier.h"

void GroundSoldier::moveTo(const Vec2& targetPos, int speed) {
  if (isDead()) return;

  // 检测路径是否穿过建筑
  Vec2 startPos = this->getPosition();
  bool isCrossBuilding = checkPathCrossBuilding(startPos, targetPos);

  if (isCrossBuilding) {
    CCLOG("地面单位：路径穿过建筑，计算绕行路径！");
    // 计算绕行路径
    calculateDetourPath(startPos, targetPos);
    if (!path_points_.empty()) {
      current_path_index_ = 0;
      moveToNextPathPoint();
    }
    return;
  }

  // 无建筑阻挡，正常移动
  Soldier::moveTo(targetPos, speed);
}

// 新增：检查路径是否穿过建筑
bool GroundSoldier::checkPathCrossBuilding(const Vec2& start, const Vec2& end) {
  // 实现建筑碰撞检测逻辑
  // 1. 获取所有建筑
  // 2. 检查线段(start, end)是否与任何建筑碰撞
  return false;  // 临时返回false
}

// 新增：计算绕行路径
void GroundSoldier::calculateDetourPath(const Vec2& start, const Vec2& end) {
  // 实现简单的绕行算法
  path_points_.clear();
  path_points_.push_back(start);

  // 可以添加中间点实现简单绕行
  Vec2 midPoint = Vec2((start.x + end.x) / 2, (start.y + end.y) / 2 + 50);
  path_points_.push_back(midPoint);

  path_points_.push_back(end);
}
