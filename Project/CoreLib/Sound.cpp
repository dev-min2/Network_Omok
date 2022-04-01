#include "Sound.h"

Sound::Sound() : m_pSystem(nullptr),m_pSound(nullptr),m_pChannel(nullptr),m_volume(0.5f)
{
}

Sound::~Sound()
{
}

void Sound::Set(FMOD::System* pSystem, std::wstring name, int index)
{
    m_pSystem   = pSystem;
    m_name      = name;
    m_index     = index;
}

bool Sound::Init()
{
    return true;
}

bool Sound::Update()
{
    return true;
}

bool Sound::Render()
{
    return true;
}

bool Sound::Release()
{
    if (m_pSound)
    {
        m_pSound->release();
        m_pSound = nullptr;
    }
    return true;
}

void Sound::Play(bool loop)
{
    bool play = false;
    if (m_pChannel != nullptr)
    {
        m_pChannel->isPlaying(&play); // play������ Ȯ��.
    }
    if (play == false) // �÷������� �ƴ϶��
    {
        // ä���� �÷��� �Ǵ� ������ ��� ��� (Context��������)
        // DX == HRESULT, FMOD == FMOD_RESULT
        FMOD_RESULT ret = m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
        if (ret == FMOD_OK)
        {
            if (loop)
                m_pChannel->setMode(FMOD_LOOP_NORMAL);
            else
                m_pChannel->setMode(FMOD_LOOP_OFF);
        }
    }
}

void Sound::PlayEffect()
{
}

void Sound::Stop()
{
    if (m_pChannel != nullptr) // ä���̾��ٴ� �ǹ̴� ���带 �������� �ʾҴٴ��ǹ�
    {
        m_pChannel->stop();
    }
}

void Sound::Paused()
{
    bool play = false;
    m_pChannel->isPlaying(&play);
    if (play) // ���尡 �÷������� �� paused
    {
        bool paused;
        m_pChannel->getPaused(&paused); // ���� paused�������� �ƴ���
        m_pChannel->setPaused(!paused);
    }
}

void Sound::VolumeUp(float volume)
{
    if (m_pChannel != nullptr)
    {
        float currentVolume;
        m_pChannel->getVolume(&currentVolume);
        m_volume = currentVolume + volume;
        m_volume = max(0.0f, m_volume);
        m_volume = min(1.0f, m_volume);
        m_pChannel->setVolume(m_volume);
    }
}

void Sound::VolumeDown(float volume)
{
    if (m_pChannel != nullptr)
    {
        float currentVolume;
        m_pChannel->getVolume(&currentVolume);
        m_volume = currentVolume - volume;
        m_volume = max(0.0f, m_volume);
        m_volume = min(1.0f, m_volume);
        m_pChannel->setVolume(m_volume);
    }
}
