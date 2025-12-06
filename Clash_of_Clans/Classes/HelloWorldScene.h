#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene {
 public:
  static cocos2d::Scene* createScene();

  virtual bool init();

  // a selector callback
  void menuCloseCallback(cocos2d::Ref* pSender);

  // implement the "static create()" method manually
  // 原本这里是一个宏定义，现在打开了
  static HelloWorld* create() {
    HelloWorld* pRet = new (std::nothrow) HelloWorld();
    if (pRet && pRet->init()) {
      pRet->autorelease();
      return pRet;
    } else {
      delete pRet;
      pRet = nullptr;
      return nullptr;
    }
  };
};

#endif  // __HELLOWORLD_SCENE_H__
