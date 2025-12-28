
#ifndef __PATH_FINDER_H__
#define __PATH_FINDER_H__

#include <limits>
#include <tuple>
#include <vector>

#include "OpenSource/OpenSource_AStar.hpp"
#include "cocos2d.h"

class GridPathFinder {
 public:
  GridPathFinder(int nCols, int nRows);
  void setWalkable(int x, int y, bool walkable);
  // 保持接口返回 std::tuple 不变，方便 Soldier.cpp 调用
  std::tuple<bool, std::vector<cocos2d::Vec2>> findPath(int startX, int startY,
                                                        int goalX, int goalY);

 private:
  AStar::Generator generator;  // 使用新库的生成器
  int m_cols, m_rows;
};

#endif  // __PATH_FINDER_H__