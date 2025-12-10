// HelloWorldScene.h 关键代码
#include "cocos2d.h"
USING_NS_CC;

class HelloWorld : public cocos2d::Layer { // 必须继承Layer
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void menuCloseCallback(cocos2d::Ref* pSender); // 声明回调函数
    CREATE_FUNC(HelloWorld);
};