#include "Building.h"

#include "BuildingManager.h"
Building::Building()
    : _level(1),
      _maxLevel(2),
      _maxHP(0),
      _currentHP(0),
      _size(0.0f),
      _camp(CampType::PLAYER),
      _type(BuildingType::COMMON),
      _isDestroyed(false),
      _upgradeCost(0),
      _upgradeTime(0.0f),
      _name(""),
      _ruinsSprite(nullptr),
      _hpBarBackground(nullptr),
      _hpBarForeground(nullptr),
      _showHpBar(true),
      _hpBarWidth(50.0f),
      _hpBarHeight(5.0f),
      _hpBarOffsetY(120.0f) {}  // 血条在建筑上方的偏移

Building::~Building() {
  // 清理废墟图片
  if (_ruinsSprite) {
    _ruinsSprite->removeFromParent();
    _ruinsSprite = nullptr;
  }

  // 清理血条
  if (_hpBarBackground) {
    _hpBarBackground->removeFromParent();
    _hpBarBackground = nullptr;
  }
  if (_hpBarForeground) {
    _hpBarForeground->removeFromParent();
    _hpBarForeground = nullptr;
  }
}

Building* Building::create(const std::string& texPath, const std::string& name,
                           CampType camp, int level, int maxLevel, int maxHP,
                           float size, int upgradeCost, float upgradeTime,
                           BuildingType type, float collision_radius) {
  Building* building = new (std::nothrow) Building();
  if (building &&
      building->init(texPath, name, camp, level, maxLevel, maxHP, size,
                     upgradeCost, upgradeTime, type, collision_radius)) {
    building->autorelease();  // Cocos2d自动内存管理
    BuildingManager::getInstance()->addBuilding(building);

    return building;
  }
  CC_SAFE_DELETE(building);
  return nullptr;
}

bool Building::init(const std::string& texPath, const std::string& name,
                    CampType camp, int level, int maxLevel, int maxHP,
                    float size, int upgradeCost, float upgradeTime,
                    BuildingType type, float collision_radius) {
  // 初始化Sprite（Cocos2d渲染基础）
  if (!Sprite::initWithFile(texPath)) {
    CCLOG("Building init failed: texture not found - %s", texPath.c_str());
    return false;
  }
  // 初始化通用属性
  _name = name;
  _camp = camp;
  _level = level;
  _maxHP = maxHP;
  _currentHP = maxHP;  // 初始血量满
  _size = size;
  _upgradeCost = upgradeCost;
  _upgradeTime = upgradeTime;
  _type = type;
  _isDestroyed = false;
  collision_radius_ = collision_radius;
  _hpBarWidth = _size * 0.8f;  // 血条宽度为建筑的80%

  // 设置建筑大小和锚点
  this->setAnchorPoint(Vec2(0.5f, 0.5f));

  // 创建血条
  createHpBar();

  // 注册每帧更新
  this->scheduleUpdate();
  BuildingManager::addBuilding(this);
  return true;
}

// 创建血条
void Building::createHpBar() {
  // 创建血条背景（红色）
  _hpBarBackground = DrawNode::create();
  _hpBarBackground->drawSolidRect(Vec2(-_hpBarWidth / 2, 0),
                                  Vec2(_hpBarWidth / 2, _hpBarHeight),
                                  Color4F(0.5f, 0.0f, 0.0f, 0.8f)  // 深红色背景
  );
  _hpBarBackground->setPosition(Vec2(55, _hpBarOffsetY));
  this->addChild(_hpBarBackground, 10);  // 确保血条在建筑上方

  // 创建血条前景（绿色）
  _hpBarForeground = DrawNode::create();
  _hpBarForeground->drawSolidRect(Vec2(-_hpBarWidth / 2, 0),
                                  Vec2(_hpBarWidth / 2, _hpBarHeight),
                                  Color4F(0.0f, 1.0f, 0.0f, 0.8f)  // 绿色前景
  );
  _hpBarForeground->setPosition(Vec2(55, _hpBarOffsetY));
  this->addChild(_hpBarForeground, 11);  // 前景在背景上方
}

// 更新血条显示
void Building::updateHpBar() {
  if (!_hpBarForeground || !_showHpBar) return;

  // 计算血量百分比
  float hpPercent = (_maxHP > 0) ? ((float)_currentHP / (float)_maxHP) : 0.0f;
  hpPercent = std::max(0.0f, std::min(1.0f, hpPercent));  // 限制在0-1之间

  // 清除旧的血条绘制
  _hpBarForeground->clear();

  // 根据血量百分比绘制新的血条
  float currentWidth = _hpBarWidth * hpPercent;
  if (currentWidth > 0) {
    // 根据血量百分比改变颜色
    Color4F barColor;
    if (hpPercent > 0.6f) {
      barColor = Color4F(0.0f, 1.0f, 0.0f, 0.8f);  // 绿色
    } else if (hpPercent > 0.3f) {
      barColor = Color4F(1.0f, 1.0f, 0.0f, 0.8f);  // 黄色
    } else {
      barColor = Color4F(1.0f, 0.0f, 0.0f, 0.8f);  // 红色
    }

    _hpBarForeground->drawSolidRect(
        Vec2(-_hpBarWidth / 2, 0),
        Vec2(-_hpBarWidth / 2 + currentWidth, _hpBarHeight), barColor);
  }

  // 如果建筑满血，可以隐藏血条（可选）
  if (hpPercent >= 1.0f) {
    _hpBarBackground->setVisible(false);
    _hpBarForeground->setVisible(false);
  } else {
    _hpBarBackground->setVisible(true);
    _hpBarForeground->setVisible(true);
  }
}

// 显示/隐藏血条
void Building::showHpBar(bool show) {
  _showHpBar = show;
  if (_hpBarBackground) {
    _hpBarBackground->setVisible(show);
  }
  if (_hpBarForeground) {
    _hpBarForeground->setVisible(show);
  }
}

void Building::checkDestroyed() {
  if (_currentHP <= 0 && !_isDestroyed) {
    _isDestroyed = true;

    // 隐藏血条
    showHpBar(false);

    // 创建废墟图片
    _ruinsSprite = Sprite::create("ruins.png");
    if (_ruinsSprite) {
      // 设置废墟图片的位置和大小与原建筑相同
      _ruinsSprite->setPosition(this->getPosition());
      _ruinsSprite->setAnchorPoint(Vec2(0.5f, 0.5f));

      // 将废墟图片添加到父节点，设置较低的Z顺序，确保在士兵下方
      if (this->getParent()) {
        // 士兵的Z顺序通常是5，所以废墟设置为3，确保在士兵下方但在地图上方
        this->getParent()->addChild(_ruinsSprite, 3);
      }
    }

    // 隐藏原建筑模型
    this->setVisible(false);
    CCLOG("建筑[%s]被摧毁！", _name.c_str());
    // 可扩展：播放摧毁动画、通知游戏管理器等
  }
  if (_currentHP <= 0 && !_isDestroyed) {
    _isDestroyed = true;

    // 创建废墟图片
    _ruinsSprite = Sprite::create("ruins.png");
    if (_ruinsSprite) {
      // 设置废墟图片的位置和大小与原建筑相同
      _ruinsSprite->setPosition(this->getPosition());
      _ruinsSprite->setAnchorPoint(Vec2(0.5f, 0.5f));

      // 将废墟图片添加到父节点，设置较低的Z顺序，确保在士兵下方
      if (this->getParent()) {
        // 士兵的Z顺序通常是5，所以废墟设置为3，确保在士兵下方但在地图上方
        this->getParent()->addChild(_ruinsSprite, 3);
      }
    }

    // 隐藏原建筑模型
    this->setVisible(false);
    CCLOG("建筑[%s]被摧毁！", _name.c_str());
  }
}

void Building::takeDamage(int damage) {
  if (_isDestroyed) return;

  _currentHP -= damage;
  _currentHP = std::max(_currentHP, 0);  // 血量不低于0

  // 受到伤害时显示血条
  showHpBar(true);

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
  _currentHP = _maxHP;                                   // 升级后血量回满
  _upgradeCost = static_cast<int>(_upgradeCost * 1.5f);  // 升级成本提高
  _upgradeTime = static_cast<int>(_upgradeTime * 1.3f);  // 升级时间延长

  CCLOG("建筑[%s]升级至%d级，新血量：%d", _name.c_str(), _level, _maxHP);

  // 升级后更新血条显示
  updateHpBar();

  return true;
}

void Building::update(float dt) {
  Sprite::update(dt);  // 基类更新（空实现，子类重写）

  // 更新血条显示
  updateHpBar();
}
bool Building::setLevel(int new_level) {
  if (new_level <= _level || new_level > 3) return false;

  _level = new_level;
  return true;
}

void Building::setMaxHp(int max_hp) {
  if (max_hp < _maxHP) return;
  _maxHP = max_hp;
}
void Building::setHp(int hp) {
  if (hp > _maxHP) hp = _maxHP;
  _currentHP = hp;
}