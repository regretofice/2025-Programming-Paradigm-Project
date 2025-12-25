#include "ResourceStorageBuilding.h"
#include <algorithm> // for std::clamp

ResourceStorageBuilding::~ResourceStorageBuilding() {}

ResourceStorageBuilding* ResourceStorageBuilding::create(const std::string& texPath, const std::string& name,
    CampType camp, int level, int maxLevel, int maxHP, float size,
    int upgradeCost, float upgradeTime,
    ResourceType resType, int maxStorageCapacity,
    float destroyLossRate) {
    ResourceStorageBuilding* storage = new (std::nothrow) ResourceStorageBuilding();
    if (storage && storage->init(texPath, name, camp, level, maxLevel, maxHP, size, upgradeCost, upgradeTime,
        resType, maxStorageCapacity, destroyLossRate)) {
        storage->autorelease();
        return storage;
    }
    CC_SAFE_DELETE(storage);
    return nullptr;
}

bool ResourceStorageBuilding::init(const std::string& texPath, const std::string& name,
    CampType camp, int level, int maxLevel, int maxHP, float size,
    int upgradeCost, float upgradeTime,
    ResourceType resType, int maxStorageCapacity,
    float destroyLossRate) {
    // 校验丢失率范围
    if (destroyLossRate < 0.0f || destroyLossRate > 1.0f) {
        CCLOG("资源丢失率超出范围，默认设为0.5");
        _destroyLossRate = 0.5f;
    }
    else {
        _destroyLossRate = destroyLossRate;
    }

    // 调用父类初始化
    if (!Building::init(texPath, name, camp, level, maxLevel, maxHP, size, upgradeCost, upgradeTime, BuildingType::RESOURCE, maxStorageCapacity)) {
        return false;
    }

    // 初始化存储属性
    _resType = resType;
    _maxStorageCapacity = maxStorageCapacity;
    _currentStored = 0; // 初始无资源

    return true;
}

bool ResourceStorageBuilding::depositResource(int amount) {
    if (_isDestroyed || amount <= 0) {
        CCLOG("[%s]存入资源失败：建筑已摧毁或存入量无效", _name.c_str());
        return false;
    }

    // 计算可存入量（不超过最大容量）
    int available = _maxStorageCapacity - _currentStored;
    if (available <= 0) {
        CCLOG("[%s]存入资源失败：存储容量已满", _name.c_str());
        return false;
    }
    int actualDeposit = std::min(amount, available);
    _currentStored += actualDeposit;

    // 日志输出
    const char* resName = _resType == ResourceType::GOLD ? "金币" : "圣水";
    CCLOG("[%s]存入%d%s，当前存储：%d/%d", _name.c_str(), actualDeposit, resName, _currentStored, _maxStorageCapacity);
    return true;
}

int ResourceStorageBuilding::withdrawResource(int amount) {
    if (_isDestroyed || amount <= 0) {
        CCLOG("[%s]取出资源失败：建筑已摧毁或取出量无效", _name.c_str());
        return 0;
    }

    // 计算实际取出量（不超过当前存储）
    int actualWithdraw = std::min(amount, _currentStored);
    _currentStored -= actualWithdraw;

    // 日志输出
    const char* resName = _resType == ResourceType::GOLD ? "金币" : "圣水";
    CCLOG("[%s]取出%d%s，剩余存储：%d/%d", _name.c_str(), actualWithdraw, resName, _currentStored, _maxStorageCapacity);
    return actualWithdraw;
}

void ResourceStorageBuilding::loseResourceOnDestroy() {
    if (!_isDestroyed || _currentStored <= 0) return;

    // 计算丢失的资源量
    int lost = static_cast<int>(_currentStored * _destroyLossRate);
    _currentStored -= lost;

    const char* resName = _resType == ResourceType::GOLD ? "金币" : "圣水";
    CCLOG("[%s]被摧毁，丢失%d%s，剩余：%d", _name.c_str(), lost, resName, _currentStored);
}

bool ResourceStorageBuilding::upgrade() {
    if (!Building::upgrade()) return false; // 调用父类升级（等级+1、血量提升）

    // 升级后提升存储容量（示例：每级+20%）
    _maxStorageCapacity = static_cast<int>(_maxStorageCapacity * 1.2f);
    CCLOG("[%s]升级至%d级，存储容量提升至%d", _name.c_str(), _level, _maxStorageCapacity);
    return true;
}

void ResourceStorageBuilding::checkDestroyed() {
    if (_currentHP <= 0 && !_isDestroyed) {
        _isDestroyed = true;
        this->setVisible(false);
        loseResourceOnDestroy(); // 被摧毁时丢失资源
        CCLOG("存储建筑[%s]被摧毁！", _name.c_str());
    }
}

void ResourceStorageBuilding::update(float dt) {
    Building::update(dt); // 调用父类更新
    // 存储建筑无每帧产出，仅检查状态（父类已实现checkDestroyed，此处无需额外逻辑）
}