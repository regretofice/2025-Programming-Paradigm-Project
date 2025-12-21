
#ifndef __PATH_FINDER_H__
#define __PATH_FINDER_H__

#include <vector>

#include "cocos2d.h"

class PathFinder {
 public:
  static PathFinder* getInstance();

  // 计算从起点到终点的路径
  std::vector<cocos2d::Vec2> findPath(const cocos2d::Vec2& start,
                                      const cocos2d::Vec2& end,
                                      bool isGroundUnit);

 private:
  PathFinder() = default;
  ~PathFinder() = default;

  // A*算法实现细节
  // ...
};

#endif  // __PATH_FINDER_H__