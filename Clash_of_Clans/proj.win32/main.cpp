#include "main.h"
#include "AppDelegate.h"
#include "cocos2d.h"

USING_NS_CC;

// Windows 入口函数
int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 创建应用程序实例并运行
    AppDelegate app;
    return Application::getInstance()->run();
}