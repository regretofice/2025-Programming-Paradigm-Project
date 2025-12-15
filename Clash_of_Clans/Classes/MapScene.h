#pragma once
#ifndef __MAP_SCENE_H__
#define __MAP_SCENE_H__

#include "cocos2d.h"

class MapScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    void menuReturnCallback(cocos2d::Ref* pSender);
    //创建地图
    void createMap();
    CREATE_FUNC(MapScene);

private:
    cocos2d::TMXTiledMap* _tileMap;
    int _mapWidth;
    int _mapHeight;
    int _tileSize;
    float _mapOffsetX;  // 地图X轴偏移
    float _mapOffsetY;  // 地图Y轴偏移
};
#endif // __MAP_SCENE_H__