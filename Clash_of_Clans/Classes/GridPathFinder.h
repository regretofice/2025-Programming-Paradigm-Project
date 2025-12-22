
#ifndef __PATH_FINDER_H__
#define __PATH_FINDER_H__

#include <limits>
#include <tuple>
#include <vector>

#include "cocos2d.h"

struct GridNode {
  int x = 0;
  int y = 0;
  int g = 0;  // 从起点到当前点的代价
  int h = 0;  // 估价（启发式：到终点的曼哈顿距离）
  int f = 0;  // f = g + h
  int parentX = -1;
  int parentY = -1;
};

class GridPathFinder {
 public:
  // nCols: 地图宽度（x方向格子数）
  // nRows: 地图高度（y方向格子数）
  GridPathFinder(int nCols, int nRows);

  // 设置是否可走：true = 可以走，false = 障碍
  void setWalkable(int x, int y, bool walkable);

  // A* 寻路：传入起点/终点的 tile 坐标
  // 返回：(是否找到路径, 路径上的 tile 坐标，从起点到终点)
  std::tuple<bool, std::vector<cocos2d::Vec2>> findPath(int startX, int startY,
                                                        int goalX, int goalY);

 private:
  bool inBounds(int x, int y) const;
  int index(int x, int y) const;

  int m_cols;
  int m_rows;
  std::vector<bool> m_blocked;  // 一维存储：blocked[index(x,y)] = true 表示障碍
};

#endif  // __PATH_FINDER_H__