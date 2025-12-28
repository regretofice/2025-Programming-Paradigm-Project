#include "Tools/AnimationManager.h"

AnimationManager* AnimationManager::instance_ = nullptr;

AnimationManager* AnimationManager::getInstance() {
  if (!instance_) {
    instance_ = new (std::nothrow) AnimationManager();
  }
  return instance_;
}

void AnimationManager::destroyInstance() {
  if (instance_) {
    for (auto& pair : instance_->animations_) {
      pair.second->release();
    }
    delete instance_;
    instance_ = nullptr;
  }
}

void AnimationManager::registerAnimationConfig(
    SoldierType type, const SoldierAnimationConfig& config) {
  configMap_[type] = config;
}

void AnimationManager::loadAnimationsByType(SoldierType type) {
  // 检查是否已注册配置
  auto configIter = configMap_.find(type);
  if (configIter == configMap_.end()) {
    cocos2d::log("Animation config for type %d not registered!", (int)type);
    return;
  }
  const auto& config = configIter->second;

  // 通用逻辑加载所有动作（无需硬编码士兵类型）
  animations_[generateAnimKey(type, "idle")] = loadSingleAnimation(config.idle);
  animations_[generateAnimKey(type, "move")] = loadSingleAnimation(config.move);
  animations_[generateAnimKey(type, "attack")] =
      loadSingleAnimation(config.attack);
  animations_[generateAnimKey(type, "die")] = loadSingleAnimation(config.die);
}

cocos2d::Animation* AnimationManager::loadSingleAnimation(
    const AnimationFrameConfig& config) {
  Vector<SpriteFrame*> frames;

  for (int i = 1; i <= config.frameCount; i++) {
    char frameName[50];
    sprintf(frameName, "%s%02d.png", config.framePrefix.c_str(), i);

    auto frame =
        SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    if (frame) {
      frames.pushBack(frame);
    }
  }

  if (frames.empty()) return nullptr;

  auto animation =
      Animation::createWithSpriteFrames(frames, config.delayPerUnit);
  animation->retain();
  return animation;
}

// 辅助函数：字符串分割
std::vector<std::string> AnimationManager::plist(const std::string& s,
                                                 char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}
cocos2d::Animation* AnimationManager::getAnimation(SoldierType type,
                                                   const std::string& action) {
  std::string key = generateAnimKey(type, action);
  auto iter = animations_.find(key);

  if (iter != animations_.end()) {
    return iter->second;  // 已缓存动画，直接返回
  }

  // 未缓存，动态生成
  auto configIter = configMap_.find(type);
  if (configIter == configMap_.end()) {
    CCLOG(" No config for SoldierType %d", (int)type);
    return nullptr;
  }

  const SoldierAnimationConfig& soldierConfig = configIter->second;

  //  正确获取对应动作配置
  const AnimationFrameConfig* frameConfig = nullptr;
  if (action == "idle") {
    frameConfig = &soldierConfig.idle;
  } else if (action == "move") {
    frameConfig = &soldierConfig.move;
  } else if (action == "attack") {
    frameConfig = &soldierConfig.attack;
  } else if (action == "die") {
    frameConfig = &soldierConfig.die;
  }

  if (!frameConfig) {
    CCLOG(" Invalid action '%s'", action.c_str());
    return nullptr;
  }

  //  从SpriteFrameCache生成动画帧
  Vector<SpriteFrame*> frames;
  for (int i = 1; i <= frameConfig->frameCount; i++) {
    char frameName[64];
    sprintf(frameName, "%s%02d.png", frameConfig->framePrefix.c_str(), i);

    auto frame =
        SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    if (frame) {
      frames.pushBack(frame);
    } else {
      CCLOG("Missing frame: %s", frameName);
    }
  }

  if (frames.empty()) {
    CCLOG("No frames for %s %s", frameConfig->framePrefix.c_str(),
          action.c_str());
    return nullptr;
  }

  // 创建并缓存动画
  auto animation =
      Animation::createWithSpriteFrames(frames, frameConfig->delayPerUnit);
  animation->retain();
  animations_[key] = animation;

  CCLOG("Generated animation: %s (%d frames)", key.c_str(), (int)frames.size());
  return animation;
}
