#pragma once
#ifndef __MAP_SCENE_H__
#define __MAP_SCENE_H__

#include "AudioManager.h"
#include "Soldier.h"
#include "cocos2d.h"
class MapScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void update(float dt) override;
	void menuReturnCallback(cocos2d::Ref* pSender);
	// 创建地图
	void createMap();

	// 菜单和放置相关函数
	void createPlacementMenu();
	void createResourceBars();
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
	static cocos2d::Scene* createSceneWithMap(const std::string& mapFile);
	CREATE_FUNC(MapScene);
	virtual void onEnter() override;
	// 添加资源显示相关函数
	void updateResourceDisplay();
	void checkBaseCampStatus();
	void gameOver(bool isVictory);
private:
	cocos2d::TMXTiledMap* _tileMap;
	std::string _mapFileName;  // 存储地图文件名
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
	bool _isUpgradeMode = false;        // 是否处于升级模式
	SoldierType currentSoldierType;     // 当前要放置的士兵类型

	bool _isMap1 = false;           
	int _countdown = 60;            // 倒计时秒数
	cocos2d::Label* _timerLabel;    // 计时器标签
	bool _isGameOver = false;       // 游戏是否结束
	int64_t _startTimeMs = 0;       // 计时开始时间

	// 添加资源显示相关成员变量
	cocos2d::Label* _goldLabel;
	cocos2d::Label* _elixirLabel;
	cocos2d::Label* _builderLabel;  // 建筑工人标签
	cocos2d::DrawNode* _goldIcon;
	cocos2d::DrawNode* _elixirIcon;
	cocos2d::DrawNode* _builderIcon;  // 建筑工人图标
	cocos2d::DrawNode* _goldBarForeground;
	cocos2d::DrawNode* _elixirBarForeground;
	cocos2d::DrawNode* _builderBarForeground;  // 建筑工人进度条前景
}; 
#endif  // __MAP_SCENE_H__