#ifndef BUILDING_H
#define BUILDING_H

#include <string>

#include "BuildingEnums.h"
#include "cocos2d.h"

USING_NS_CC;

class Building : public Sprite {
 protected:
  // 通用属性
  int _level;               // 等级
  int _maxLevel;            // 最高等级
  int _maxHP;               // 最大血量
  int _currentHP;           // 当前血量
  float _size;              // 边长（建筑大小）
  CampType _camp;           // 阵营归属
  BuildingType _type;       // 建筑类型
  bool _isDestroyed;        // 是否被摧毁
  int _upgradeCost;         // 升级所需资源
  float _upgradeTime;       // 升级时间（秒）
  std::string _name;        // 建筑名称
  float collision_radius_;  // 建筑碰撞半径

 public:
  // Cocos2d风格创建函数（内存自动管理）
  static Building* create(const std::string& texPath, const std::string& name,
                          CampType camp, int level, int maxLevel, int maxHP,
                          float size, int upgradeCost, float upgradeTime,
                          BuildingType type, float collision_radius);

  // 初始化函数
  virtual bool init(const std::string& texPath, const std::string& name,
                    CampType camp, int level, int maxLevel, int maxHP,
                    float size, int upgradeCost, float upgradeTime,
                    BuildingType type, float collision_radius);

  // 通用方法
  virtual void takeDamage(int damage);     // 受攻击
  virtual bool upgrade();                  // 升级（返回是否成功）
  virtual void checkDestroyed();           // 检查是否被摧毁
  virtual void update(float dt) override;  // 每帧更新（虚函数，子类重写）
  BuildingType getType() const { return _type; }  // 获取建筑类型

  // Getter/Setter
  int getLevel() const { return _level; }
  int getCurrentHP() const { return _currentHP; }
  CampType getCamp() const { return _camp; }
  bool isDestroyed() const { return _isDestroyed; }
  const std::string& getName() const override { return _name; }

  // 获取建筑碰撞范围,固定建筑半径
  static float getCollisionRadius() { return 40.0f; }

  // 获取建筑位置（世界坐标系）
  cocos2d::Vec2 getWorldPosition() const { return getPosition(); }

  Building();
  // 析构函数
  virtual ~Building();
};

#endif