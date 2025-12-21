#pragma once
#ifndef __MAP_SCENE_H__
#define __MAP_SCENE_H__

#include "Soldier.h"
#include "cocos2d.h"

class MapScene : public cocos2d::Scene {
 public:
  static cocos2d::Scene* createScene();

  virtual bool init();
  void menuReturnCallback(cocos2d::Ref* pSender);
  // 创建地图
  void createMap();

  // 菜单和放置相关函数
  void createPlacementMenu();
  void onBuildingButtonClicked(cocos2d::Ref* sender, int buildingType);
  void onSoldierButtonClicked(cocos2d::Ref* sender, int soldierType);
  bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
  void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
  void onTouchEnded(Touch* touch, Event* event);
  void cancelPlacement();
  void saveBuildingsPosition();
  void loadSavedBuildings();

  void spawnSoldierAtPosition(SoldierType type, const Vec2& pos);
  bool checkResourceEnough(SoldierType type);
  void consumeResource(SoldierType type);

  CREATE_FUNC(MapScene);
  virtual void onEnter() override;

 private:
  cocos2d::TMXTiledMap* _tileMap;
  int _mapWidth;
  int _mapHeight;
  int _tileSize;
  float _mapOffsetX;  // 地图X轴偏移
  float _mapOffsetY;  // 地图Y轴偏移

  cocos2d::Sprite* _previewSprite;    // 预览图
  cocos2d::Vec2 _previewMapPosition;  // 预览图对应的地图位置
  int _currentPlacementType;          // 当前放置类型：0-无，1-建筑，2-士兵
  int _currentTypeId;                 // 具体类型ID
  bool _isPlacing;                    // 是否正在放置状态
  bool isPlacingSoldier;              // 是否处于放置士兵模式
  SoldierType currentSoldierType;     // 当前要放置的士兵类型
};
#endif  // __MAP_SCENE_H__