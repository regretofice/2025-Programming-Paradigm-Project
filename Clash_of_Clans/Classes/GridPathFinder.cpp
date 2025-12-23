#include "GridPathFinder.h"

#include <algorithm>
#include <cmath>
#include <queue>

USING_NS_CC;

GridPathFinder::GridPathFinder(int nCols, int nRows)
    : m_cols(nCols), m_rows(nRows), m_blocked(nCols * nRows, false) {}

bool GridPathFinder::inBounds(int x, int y) const {
  return x >= 0 && x < m_cols && y >= 0 && y < m_rows;
}

int GridPathFinder::index(int x, int y) const {
  if (x < 0 || x >= m_cols || y < 0 || y >= m_rows) {
    // 如果越界，返回一个安全的错误值或触发警告而非崩溃
    return 0;
  }
  return y * m_cols + x;
}

void GridPathFinder::setWalkable(int x, int y, bool walkable) {
  if (!inBounds(x, y)) return;
  m_blocked[index(x, y)] = !walkable;
}

std::tuple<bool, std::vector<Vec2>> GridPathFinder::findPath(int startX,
                                                             int startY,
                                                             int goalX,
                                                             int goalY) {
  std::vector<Vec2> emptyResult;

  if (!inBounds(startX, startY) || !inBounds(goalX, goalY)) {
    return {false, emptyResult};
  }
  if (m_blocked[index(goalX, goalY)]) {
    // 目标格子是障碍，直接失败
    return {false, emptyResult};
  }

  const int INF = std::numeric_limits<int>::max();

  // 每个格子的 g / h / f / 父节点
  std::vector<int> gScore(m_cols * m_rows, INF);
  std::vector<int> hScore(m_cols * m_rows, 0);
  std::vector<int> fScore(m_cols * m_rows, INF);
  std::vector<int> parentX(m_cols * m_rows, -1);
  std::vector<int> parentY(m_cols * m_rows, -1);
  std::vector<bool> closed(m_cols * m_rows, false);

  auto heuristic = [&](int x, int y) {
    // 曼哈顿距离
    return std::abs(x - goalX) + std::abs(y - goalY);
  };

  struct PQNode {
    int x;
    int y;
    int f;
  };

  struct CompareF {
    bool operator()(const PQNode& a, const PQNode& b) const {
      return a.f > b.f;  // 小 f 优先
    }
  };

  std::priority_queue<PQNode, std::vector<PQNode>, CompareF> openList;

  int startIdx = index(startX, startY);
  gScore[startIdx] = 0;
  hScore[startIdx] = heuristic(startX, startY);
  fScore[startIdx] = gScore[startIdx] + hScore[startIdx];

  openList.push({startX, startY, fScore[startIdx]});

  // 4 方向移动，也可以扩成 8 方向
  const int dir[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  bool found = false;

  while (!openList.empty()) {
    PQNode current = openList.top();
    openList.pop();

    int cx = current.x;
    int cy = current.y;
    int cidx = index(cx, cy);

    if (closed[cidx]) continue;
    closed[cidx] = true;

    // 到达目标
    if (cx == goalX && cy == goalY) {
      found = true;
      break;
    }

    // 遍历邻居
    for (int k = 0; k < 4; ++k) {
      int nx = cx + dir[k][0];
      int ny = cy + dir[k][1];

      if (!inBounds(nx, ny)) continue;
      int nidx = index(nx, ny);
      if (m_blocked[nidx]) continue;  // 障碍
      if (closed[nidx]) continue;

      int newG = gScore[cidx] + 10;  // 相邻格子固定代价

      if (newG < gScore[nidx]) {
        gScore[nidx] = newG;
        hScore[nidx] = heuristic(nx, ny);
        fScore[nidx] = gScore[nidx] + hScore[nidx];
        parentX[nidx] = cx;
        parentY[nidx] = cy;

        openList.push({nx, ny, fScore[nidx]});
      }
    }
  }

  if (!found) {
    return {false, emptyResult};
  }

  // 回溯构建路径（从 goal 回到 start，再反转）
  std::vector<Vec2> path;
  int tx = goalX;
  int ty = goalY;
  int tidx = index(tx, ty);

  while (!(tx == startX && ty == startY)) {
    path.emplace_back((float)tx, (float)ty);
    int px = parentX[tidx];
    int py = parentY[tidx];
    if (px == -1 && py == -1) {
      // 理论上不该发生，防御一下
      break;
    }
    tx = px;
    ty = py;
    tidx = index(tx, ty);
  }
  // 把起点也加进去
  path.emplace_back((float)startX, (float)startY);

  std::reverse(path.begin(), path.end());

  return {true, path};
}
