#ifndef __ANIMATION_MANAGER_H__
#define __ANIMATION_MANAGER_H__

#include <string>
#include <unordered_map>

#include "Soldier.h"
#include "cocos2d.h"

// 单个动作的动画配置
struct AnimationFrameConfig {
  std::string framePrefix;  // 帧资源前缀（如"rarbarian_idle_"）
  int frameCount;           // 帧数量
  float delayPerUnit;       // 每帧间隔时间
};

// 士兵类型对应的全套动画配置
struct SoldierAnimationConfig {
  AnimationFrameConfig idle;    //  idle动作配置
  AnimationFrameConfig move;    // 移动动作配置
  AnimationFrameConfig attack;  // 攻击动作配置
  AnimationFrameConfig die;     // 死亡动作配置
};

class AnimationManager {
 public:
  static AnimationManager* getInstance();
  static void destroyInstance();

  // 注册士兵类型的动画配置（由子类在加载前调用）
  void registerAnimationConfig(SoldierType type,
                               const SoldierAnimationConfig& config);

  // 加载指定类型士兵的所有动画（根据注册的配置）
  void loadAnimationsByType(SoldierType type);

  std::vector<std::string> plist(const std::string& s, char delimiter);

  // 获取动画（通过士兵类型+动作名称）
  cocos2d::Animation* getAnimation(SoldierType type, const std::string& action);

  // 生成统一的动画key（内部使用）
  std::string generateAnimKey(SoldierType type, const std::string& action) {
    return std::to_string((int)type) + "_" + action;
  }

 private:
  AnimationManager() = default;
  ~AnimationManager() = default;
  AnimationManager(const AnimationManager&) = delete;
  AnimationManager& operator=(const AnimationManager&) = delete;

  // 加载单组动画帧的通用方法（核心优化点）
  cocos2d::Animation* loadSingleAnimation(const AnimationFrameConfig& config);

  std::unordered_map<SoldierType, SoldierAnimationConfig>
      configMap_;  // 配置映射
  std::unordered_map<std::string, cocos2d::Animation*>
      animations_;  // 动画实例映射
  static AnimationManager* instance_;
};

#endif  // __ANIMATION_MANAGER_H__
