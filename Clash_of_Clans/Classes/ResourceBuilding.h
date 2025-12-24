#ifndef RESOURCE_BUILDING_H
#define RESOURCE_BUILDING_H

#include "Building.h"

class ResourceBuilding : public Building {
private:
    ResourceBuilding();  // 声明私有构造函数
    // 资源建筑专属属性
    ResourceType _resType;  // 产出资源类型
    int _productionPerSec;  // 每秒产出
    int _maxCapacity;       // 最大存储容量
    int _currentRes;        // 当前存储资源

public:
    static ResourceBuilding* create(const std::string& texPath,
        const std::string& name, CampType camp,
        int level, int maxLevel, int maxHP,
        float size, int upgradeCost,
        float upgradeTime, ResourceType resType,
        int productionPerSec, int maxCapacity);

    virtual bool init(const std::string& texPath, const std::string& name,
        CampType camp, int level, int maxLevel, int maxHP,
        float size, int upgradeCost, float upgradeTime,
        ResourceType resType, int productionPerSec,
        int maxCapacity);

    // 专属方法
    void produceResource(float dt);          // 生产资源
    int collectResource();                   // 收集资源（返回收集量）
    virtual void update(float dt) override;  // 重写更新逻辑

    // Getter
    int getCurrentResource() const { return _currentRes; }
    ResourceType getResType() const { return _resType; }

    virtual ~ResourceBuilding();
};

#endif  // RESOURCE_BUILDING_H