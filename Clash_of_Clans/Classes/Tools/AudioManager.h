#pragma once
#ifndef __AUDIO_MANAGER_H__
#define __AUDIO_MANAGER_H__

#include "AudioEngine.h"
#include "cocos2d.h"

USING_NS_CC;

/**
 * 音频管理器类 - 单例模式
 * 用于统一管理游戏中的所有音频资源
 */
class AudioManager {
 public:
  // 单例访问
  static AudioManager* getInstance();
  static void destroyInstance();

  // 预加载音频资源
  void preloadAllAudio();

  // 背景音乐控制
  void playBackgroundMusic(const std::string& filePath, bool loop = true,
                           float volume = 0.3f);
  void stopBackgroundMusic();
  void pauseBackgroundMusic();
  void resumeBackgroundMusic();
  void setBackgroundMusicVolume(float volume);
  float getBackgroundMusicVolume() const { return _bgmVolume; }

  // 攻击音效控制
  int playAttackSound(const std::string& filePath = "", float volume = 0.7f);
  void stopAllAttackSounds();

  // 通用音效控制
  int playSoundEffect(const std::string& filePath, bool loop = false,
                      float volume = 0.8f);
  void stopSoundEffect(int soundId);

  // 全局音量控制
  void setMute(bool mute);
  bool isMuted() const { return _isMuted; }
  void setMasterVolume(float volume);
  float getMasterVolume() const { return _masterVolume; }

  // 游戏状态音频
  void playGameStartSound();
  void playGameOverSound();
  void playVictorySound();

  // 清理资源
  void stopAllSounds();
  void uncacheAllAudio();

  // 获取当前播放状态
  bool isBackgroundMusicPlaying() const;
  int getPlayingSoundCount() const { return _activeSoundIds.size(); }

 private:
  AudioManager();
  ~AudioManager();

  // 禁止拷贝和赋值
  AudioManager(const AudioManager&) = delete;
  AudioManager& operator=(const AudioManager&) = delete;

  // 内部辅助方法
  int playSoundInternal(const std::string& filePath, bool loop, float volume);
  void removeInactiveSounds();

  static AudioManager* _instance;

  // 音频ID管理
  int _currentBackgroundMusicId;
  std::vector<int> _activeSoundIds;

  // 音量设置
  float _masterVolume;  // 主音量 (0.0 - 1.0)
  float _bgmVolume;     // 背景音乐音量
  float _effectVolume;  // 音效音量

  // 状态
  bool _isMuted;
  bool _isBackgroundMusicLoaded;

  // 默认音频文件路径
  const std::string DEFAULT_BGM = "start_music.mp3";
  const std::string DEFAULT_VICTORY = "victory.mp3";
};

#endif  // __AUDIO_MANAGER_H__