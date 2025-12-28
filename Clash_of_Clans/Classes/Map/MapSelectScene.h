#pragma once
#ifndef __MAP_SELECT_SCENE_H__
#define __MAP_SELECT_SCENE_H__

#include "cocos2d.h"

class MapSelectScene : public cocos2d::Scene {
 public:
  static cocos2d::Scene* createScene();
  virtual bool init();

  void selectMap1Callback(cocos2d::Ref* pSender);
  void selectMap2Callback(cocos2d::Ref* pSender);
  void menuReturnCallback(cocos2d::Ref* pSender);

  CREATE_FUNC(MapSelectScene);
};

#endif  // __MAP_SELECT_SCENE_H__