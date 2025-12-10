// 设置中文能够正常使用
#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#include "HelloWorldScene.h"
#include "PlayerDataManager.h"
#include "cocos2d.h"
USING_NS_CC;

Scene* HelloWorld::createScene() {
	auto scene = Scene::create();
	auto layer = HelloWorld::create();
	scene->addChild(layer);
	return scene;
}

static void problemLoading(const char* filename) {
	printf("Error while loading: %s\n", filename);
	printf(
		"Depending on how you compiled you might have to add 'Resources/' in "
		"front of filenames in HelloWorldScene.cpp\n");
}

bool HelloWorld::init() {
	if (!Layer::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// 地图加载：
	auto tmxMap = TMXTiledMap::create("map1.tmx");
	if (!tmxMap) return false;

	float scale = 1.35f; // 放大倍数
	tmxMap->setScale(scale);

	Size mapSize = tmxMap->getContentSize();
	// 居中：
	tmxMap->setPosition(
		visibleSize.width / 2 - mapSize.width * scale / 2,
		visibleSize.height / 2 - mapSize.height * scale / 2
	);

	this->addChild(tmxMap, 1);

	// 玩家数据
	auto PlayerData = PlayerDataManager::getInstance();
	int currentGold = PlayerData->getGold();
	int currentElixir = PlayerData->getElixir();
	int currentBuilder = PlayerData->getBuilder();
	CCLOG("当前金币：%d,当前圣水：%d,当前建筑工人：%d", currentGold, currentElixir, currentBuilder);

	PlayerData->setGold(currentGold + 50);
	PlayerData->setElixir(currentElixir + 20);
	PlayerData->resetAllData();

	auto goldLabel = Label::createWithSystemFont(
		"金币：" + std::to_string(PlayerData->getGold()), "Microsoft YaHei", 50);
	goldLabel->setPosition(visibleSize.width / 2, visibleSize.height - 50);
	goldLabel->setTextColor(Color4B::YELLOW);
	this->addChild(goldLabel, 2);

	return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender) {
	Director::getInstance()->end();
}