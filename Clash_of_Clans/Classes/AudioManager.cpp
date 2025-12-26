#include "AudioManager.h"

AudioManager* AudioManager::_instance = nullptr;

AudioManager* AudioManager::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new AudioManager();
    }
    return _instance;
}

void AudioManager::destroyInstance()
{
    if (_instance)
    {
        _instance->stopAllSounds();
        _instance->uncacheAllAudio();
        delete _instance;
        _instance = nullptr;
    }
}

AudioManager::AudioManager()
    : _currentBackgroundMusicId(-1)
    , _masterVolume(1.0f)
    , _bgmVolume(1.0f)
    , _effectVolume(0.7f)
    , _isMuted(false)
    , _isBackgroundMusicLoaded(false)
{
    // 预加载常用音频资源
    preloadAllAudio();
}

AudioManager::~AudioManager()
{
    stopAllSounds();
    uncacheAllAudio();
}

void AudioManager::preloadAllAudio()
{
    // 预加载默认音频文件
    std::vector<std::string> audioFiles = {
        DEFAULT_BGM,
        DEFAULT_BUTTON,
        DEFAULT_ATTACK,
        DEFAULT_SELECT,
        DEFAULT_GAME_START,
        DEFAULT_GAME_OVER,
        DEFAULT_VICTORY
    };

    for (const auto& file : audioFiles)
    {
        // 检查文件是否存在
        if (FileUtils::getInstance()->isFileExist(file))
        {
            AudioEngine::preload(file);
        }
        else
        {
            CCLOG("Audio file not found: %s", file.c_str());
        }
    }
}

void AudioManager::playBackgroundMusic(const std::string& filePath, bool loop, float volume)
{
    // 如果已经静音，则不播放
    if (_isMuted) return;

    // 停止当前背景音乐
    if (_currentBackgroundMusicId != -1)
    {
        AudioEngine::stop(_currentBackgroundMusicId);
        _currentBackgroundMusicId = -1;
    }

    // 设置要播放的文件路径
    std::string path = filePath.empty() ? DEFAULT_BGM : filePath;

    // 播放背景音乐
    float actualVolume = volume * _bgmVolume * _masterVolume;
    _currentBackgroundMusicId = AudioEngine::play2d(path, loop, actualVolume);

    // 保存音量设置
    _bgmVolume = volume;

    // 标记背景音乐已加载
    _isBackgroundMusicLoaded = true;
}

void AudioManager::stopBackgroundMusic()
{
    if (_currentBackgroundMusicId != -1)
    {
        AudioEngine::stop(_currentBackgroundMusicId);
        _currentBackgroundMusicId = -1;
    }
}

void AudioManager::pauseBackgroundMusic()
{
    if (_currentBackgroundMusicId != -1)
    {
        AudioEngine::pause(_currentBackgroundMusicId);
    }
}

void AudioManager::resumeBackgroundMusic()
{
    if (_currentBackgroundMusicId != -1 && !_isMuted)
    {
        AudioEngine::resume(_currentBackgroundMusicId);
    }
}

void AudioManager::setBackgroundMusicVolume(float volume)
{
    _bgmVolume = volume;

    if (_currentBackgroundMusicId != -1 && !_isMuted)
    {
        float actualVolume = _bgmVolume * _masterVolume;
        AudioEngine::setVolume(_currentBackgroundMusicId, actualVolume);
    }
}

int AudioManager::playAttackSound(const std::string& filePath, float volume)
{
    if (_isMuted) return -1;

    std::string path = filePath.empty() ? DEFAULT_ATTACK : filePath;
    float actualVolume = volume * _effectVolume * _masterVolume;

    int soundId = playSoundInternal(path, false, actualVolume);
    return soundId;
}

void AudioManager::stopAllAttackSounds()
{
    // 这里可以添加特定逻辑来停止攻击音效
    // 目前使用通用方法
    removeInactiveSounds();
}

void AudioManager::playButtonSound(float volume)
{
    if (_isMuted) return;

    std::string path = DEFAULT_BUTTON;
    float actualVolume = volume * _effectVolume * _masterVolume;

    playSoundInternal(path, false, actualVolume);
}

void AudioManager::playSelectSound(float volume)
{
    if (_isMuted) return;

    std::string path = DEFAULT_SELECT;
    float actualVolume = volume * _effectVolume * _masterVolume;

    playSoundInternal(path, false, actualVolume);
}

int AudioManager::playSoundEffect(const std::string& filePath, bool loop, float volume)
{
    if (_isMuted) return -1;

    float actualVolume = volume * _effectVolume * _masterVolume;
    int soundId = playSoundInternal(filePath, loop, actualVolume);
    return soundId;
}

void AudioManager::stopSoundEffect(int soundId)
{
    if (soundId != -1)
    {
        AudioEngine::stop(soundId);

        // 从活动列表中移除
        auto it = std::find(_activeSoundIds.begin(), _activeSoundIds.end(), soundId);
        if (it != _activeSoundIds.end())
        {
            _activeSoundIds.erase(it);
        }
    }
}

void AudioManager::setMute(bool mute)
{
    _isMuted = mute;

    if (mute)
    {
        // 静音：暂停所有音频
        AudioEngine::pauseAll();
    }
    else
    {
        // 取消静音：恢复背景音乐和重新设置音量
        if (_currentBackgroundMusicId != -1)
        {
            // 恢复背景音乐并设置正确音量
            float actualVolume = _bgmVolume * _masterVolume;
            AudioEngine::setVolume(_currentBackgroundMusicId, actualVolume);
            AudioEngine::resume(_currentBackgroundMusicId);
        }

        // 恢复其他音频
        AudioEngine::resumeAll();

        // 重新设置所有活动音效的音量
        for (int soundId : _activeSoundIds)
        {
            float actualVolume = _effectVolume * _masterVolume;
            AudioEngine::setVolume(soundId, actualVolume);
        }
    }
}

void AudioManager::setMasterVolume(float volume)
{
    _masterVolume = volume;

    if (!_isMuted)
    {
        // 更新背景音乐音量
        if (_currentBackgroundMusicId != -1)
        {
            float actualVolume = _bgmVolume * _masterVolume;
            AudioEngine::setVolume(_currentBackgroundMusicId, actualVolume);
        }

        // 更新所有音效音量
        for (int soundId : _activeSoundIds)
        {
            float actualVolume = _effectVolume * _masterVolume;
            AudioEngine::setVolume(soundId, actualVolume);
        }
    }
}

void AudioManager::playGameStartSound()
{
    if (_isMuted) return;

    std::string path = DEFAULT_GAME_START;
    float actualVolume = 0.8f * _effectVolume * _masterVolume;

    playSoundInternal(path, false, actualVolume);
}

void AudioManager::playGameOverSound()
{
    if (_isMuted) return;

    std::string path = DEFAULT_GAME_OVER;
    float actualVolume = 0.8f * _effectVolume * _masterVolume;

    playSoundInternal(path, false, actualVolume);
}

void AudioManager::playVictorySound()
{
    if (_isMuted) return;

    std::string path = DEFAULT_VICTORY;
    float actualVolume = 0.8f * _effectVolume * _masterVolume;

    playSoundInternal(path, false, actualVolume);
}

void AudioManager::stopAllSounds()
{
    AudioEngine::stopAll();
    _currentBackgroundMusicId = -1;
    _activeSoundIds.clear();
}

void AudioManager::uncacheAllAudio()
{
    AudioEngine::uncacheAll();
    _isBackgroundMusicLoaded = false;
}

bool AudioManager::isBackgroundMusicPlaying() const
{
    if (_currentBackgroundMusicId == -1) return false;

    auto state = AudioEngine::getState(_currentBackgroundMusicId);
    return state == AudioEngine::AudioState::PLAYING;
}

int AudioManager::playSoundInternal(const std::string& filePath, bool loop, float volume)
{
    // 检查文件是否存在
    if (!FileUtils::getInstance()->isFileExist(filePath))
    {
        CCLOG("Audio file not found: %s", filePath.c_str());
        return -1;
    }

    // 播放音效
    int soundId = AudioEngine::play2d(filePath, loop, volume);

    if (soundId != AudioEngine::INVALID_AUDIO_ID)
    {
        // 添加到活动列表
        _activeSoundIds.push_back(soundId);

        // 设置播放完成回调，自动清理
        AudioEngine::setFinishCallback(soundId, [this, soundId](int id, const std::string& filePath) {
            // 从活动列表中移除
            auto it = std::find(_activeSoundIds.begin(), _activeSoundIds.end(), id);
            if (it != _activeSoundIds.end())
            {
                _activeSoundIds.erase(it);
            }
            });
    }

    return soundId;
}

void AudioManager::removeInactiveSounds()
{
    // 移除已完成播放的音效ID
    _activeSoundIds.erase(
        std::remove_if(_activeSoundIds.begin(), _activeSoundIds.end(),
            [](int soundId) {
                auto state = AudioEngine::getState(soundId);
                return state == AudioEngine::AudioState::PAUSED;
            }),
        _activeSoundIds.end()
    );
}