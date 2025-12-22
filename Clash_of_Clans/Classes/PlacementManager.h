#ifndef __PLACEMENT__MANAGER__H__
#define __PLACEMENT__MANAGER__H__

#pragma once

#include "Building.h"
#include "Soldier.h"
#include "cocos2d.h"

class PlacementManager {
 public:
  enum class PlacementType { NONE = 0, BUILDING = 1, SOLDIER = 2 };

  static PlacementManager* getInstance();
  static void destroyInstance();

  float getTileSize() const { return _tileSize; }
  int getMapWidth() const { return _mapWidth; }
  int getMapHeight() const { return _mapHeight; }

  // 初始化：把地图的网格信息告诉它
  void initGrid(int mapWidth, int mapHeight, float tileSize, float mapOffsetX,
                float mapOffsetY, float mapScale);

  // 进入放置模式
  void startPlaceBuilding(int buildingType);
  void startPlaceSoldier(int soldierType);

  // 触摸事件（由 MapScene 调用）
  void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event,
                    cocos2d::Node* parentScene);
  bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event,
                    cocos2d::Node* parentScene);

  // 取消放置
  void cancelPlacement(cocos2d::Node* parentScene);

  void createPreviewSprite(int buildingType, int soldierType,
                           cocos2d::Node* parentScene);

  void removeSoldier(Soldier* soldier);

  cocos2d::Vec2 worldToTile(const cocos2d::Vec2& worldPos) const;
  cocos2d::Vec2 tileToWorldCenter(float tileX, float tileY) const;

 private:
  PlacementManager();
  ~PlacementManager();

  static PlacementManager* s_instance;

  // 网格相关
  int _mapWidth = 0;
  int _mapHeight = 0;
  float _tileSize = 0.0f;
  float _mapOffsetX = 0.0f;
  float _mapOffsetY = 0.0f;
  float _mapScale = 1.0f;

  // 占用表
  std::vector<std::vector<bool>> _gridOccupied;

  // 当前状态
  cocos2d::Sprite* _previewSprite = nullptr;
  PlacementType _currentType = PlacementType::NONE;
  int _currentId = 0;              // buildingType or soldierType
  cocos2d::Vec2 _previewBaseTile;  // 当前2×2左下角格子坐标

  // 碰撞相关
  std::vector<Soldier*> activeSoldiers;  // 跟踪活跃士兵
  bool canPlaceSoldierAt(int tileX, int tileY, float radius) const;
  float getSoldierRadius(int soldierType) const;

  // 内部工具函数

  bool canPlaceAt(int baseX, int baseY, int sizeInTiles) const;
  void occupyGrid(int baseX, int baseY, int sizeInTiles);

  void removePreview(cocos2d::Node* parentScene);
};

#endif  // !__PLACEMENT__MANAGER__H__