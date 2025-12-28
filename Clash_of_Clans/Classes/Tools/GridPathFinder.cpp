#include "Tools/GridPathFinder.h"

#include <algorithm>
#include <cmath>
#include <queue>

USING_NS_CC;

GridPathFinder::GridPathFinder(int nCols, int nRows)
    : m_cols(nCols), m_rows(nRows) {
  generator.setWorldSize({nCols, nRows});
  generator.setHeuristic(AStar::Heuristic::manhattan);  // 设置启发函数
  generator.setDiagonalMovement(false);                 //  强制关闭对角线走法
}

void GridPathFinder::setWalkable(int x, int y, bool walkable) {
  if (walkable) {
    generator.removeCollision({x, y});
  } else {
    generator.addCollision({x, y});
  }
}

std::tuple<bool, std::vector<cocos2d::Vec2>> GridPathFinder::findPath(
    int startX, int startY, int goalX, int goalY) {
  auto path = generator.findPath({startX, startY}, {goalX, goalY});

  std::vector<cocos2d::Vec2> resultPath;
  if (path.empty()) {
    return std::make_tuple(false, resultPath);
  }
  CCLOG("size of path:%d", path.size());
  for (auto& coord : path) {
    // 转换回 Cocos2d 的 Vec2 格式
    resultPath.push_back(cocos2d::Vec2(coord.x, coord.y));
  }

  return std::make_tuple(true, resultPath);
}
