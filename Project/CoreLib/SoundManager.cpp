#include "SoundManager.h"
#include "Sound.h"
SoundManager* SoundManager::m_pInstance = nullptr;

SoundManager* SoundManager::GetInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new SoundManager();
    return m_pInstance;
}

void SoundManager::ReleaseInstance()
{
    if (m_pInstance != nullptr)
    {
        SAFE_DELETE(m_pInstance);
    }
}

Sound* SoundManager::Load(std::string fileName)
{
    TCHAR szFileName[MAX_PATH] = { 0, };
    TCHAR Dirve[MAX_PATH] = { 0, };
    TCHAR Dir[MAX_PATH] = { 0, };
    TCHAR FileName[MAX_PATH] = { 0, };
    TCHAR FileExt[MAX_PATH] = { 0, };
    // 경로 분할.
    std::wstring fullpathname = Multi2Uni(fileName);
    _tsplitpath_s(fullpathname.c_str(), Dirve, Dir, FileName, FileExt);
    std::wstring name = FileName;
    name += FileExt;

    auto iter = m_soundList.find(name);
    if (iter != m_soundList.end())
        return iter->second;

    Sound* pSound = new Sound;
    FMOD_RESULT ret = m_pSystem->createSound(fileName.c_str(), FMOD_DEFAULT, 0, pSound->GetSoundAddress());

    if (ret != FMOD_OK)
        return nullptr;

    m_soundList.insert(std::make_pair(name, pSound));

    pSound->Set(m_pSystem, name, m_index++);
    return pSound;
    
}

Sound* SoundManager::GetSound(std::wstring soundName)
{
    return nullptr;
}

bool SoundManager::Init()
{
    FMOD_RESULT ret;
    ret = FMOD::System_Create(&m_pSystem);
    ret = m_pSystem->init(32, FMOD_INIT_NORMAL, 0);
    return true;
}

bool SoundManager::Update()
{
    m_pSystem->update();
    return true;
}

bool SoundManager::Render()
{
    return true;
}

bool SoundManager::Release()
{
    for (auto del : m_soundList)
    {
        del.second->Release();
        delete del.second;
    }
    m_soundList.clear();
    m_pSystem->close();
    m_pSystem->release();

    this->ReleaseInstance();
    return true;
}

SoundManager::~SoundManager()
{
}

SoundManager::SoundManager() : m_index(0), m_pSystem(nullptr)
{
}
