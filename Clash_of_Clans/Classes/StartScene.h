#pragma once
#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class StartScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    virtual void onEnter() override;
    virtual void onExit() override;

    void menuStartCallback(cocos2d::Ref* pSender);
    void menuExitCallback(cocos2d::Ref* pSender);
    void menuMuteCallback(cocos2d::Ref* pSender);


    CREATE_FUNC(StartScene);

private:
    cocos2d::ui::Button* _muteButton;
    bool _isMuted;
};

#endif // __START_SCENE_H__