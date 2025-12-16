#include "Building.h"

Building::Building()
    : _level(1), _maxLevel(2), _maxHP(0), _currentHP(0), _size(0.0f),
    _camp(CampType::PLAYER), _type(BuildingType::COMMON),
    _isDestroyed(false), _upgradeCost(0), _upgradeTime(0.0f), _name("") {
}

Building::~Building() {}

Building* Building::create(const std::string& texPath, const std::string& name,
    CampType camp, int level, int maxLevel, int maxHP, float size,
    int upgradeCost, float upgradeTime, BuildingType type) {
    Building* building = new (std::nothrow) Building();
    if (building && building->init(texPath, name, camp, level, maxLevel, maxHP, size, upgradeCost, upgradeTime, type)) {
        building->autorelease(); // Cocos2d自动内存管理
        return building;
    }
    CC_SAFE_DELETE(building);
    return nullptr;
}

bool Building::init(const std::string& texPath, const std::string& name,
    CampType camp, int level, int maxLevel, int maxHP, float size,
    int upgradeCost, float upgradeTime, BuildingType type) {
    // 初始化Sprite（Cocos2d渲染基础）
    if (!Sprite::initWithFile(texPath)) return false;

    // 初始化通用属性
    _name = name;
    _camp = camp;
    _level = level;
    _maxHP = maxHP;
    _currentHP = maxHP; // 初始血量满
    _size = size;
    _upgradeCost = upgradeCost;
    _upgradeTime = upgradeTime;
    _type = type;
    _isDestroyed = false;

    // 设置建筑大小和锚点
    this->setContentSize(Size(_size, _size));
    this->setAnchorPoint(Vec2(0.5f, 0.5f));

    // 注册每帧更新
    this->scheduleUpdate();

    return true;
}

void Building::checkDestroyed() {
    if (_currentHP <= 0 && !_isDestroyed) {
        _isDestroyed = true;
        this->setVisible(false); // 隐藏模型
        CCLOG("建筑[%s]被摧毁！", _name.c_str());
        // 可扩展：播放摧毁动画、通知游戏管理器等
    }
}

void Building::takeDamage(int damage) {
    if (_isDestroyed) return;

    _currentHP -= damage;
    _currentHP = std::max(_currentHP, 0); // 血量不低于0
    checkDestroyed();
}

bool Building::upgrade() {
    if (_isDestroyed) {
        CCLOG("无法升级已摧毁的建筑：%s", _name.c_str());
        return false;
    }

    if (_level == _maxLevel) {
        CCLOG("无法升级满级建筑：%s", _name.c_str());
        return false;
    }

    // 升级逻辑：等级+1，血量提升20%，升级成本提升50%，升级时间延长30%
    _level++;
    _maxHP = static_cast<int>(_maxHP * 1.2f);
    _currentHP = _maxHP; // 升级后血量回满
    _upgradeCost = static_cast<int>(_upgradeCost * 1.5f); // 升级成本提高
    _upgradeTime = static_cast<int>(_upgradeTime * 1.3f); // 升级时间延长

    CCLOG("建筑[%s]升级至%d级，新血量：%d", _name.c_str(), _level, _maxHP);
    return true;
}

void Building::update(float dt) {
    Sprite::update(dt); // 基类更新（空实现，子类重写）
}