#include "TextureManager.h"
#include "Texture.h"
TextureManager* TextureManager::m_pInstance = nullptr;

TextureManager::TextureManager() : m_index(0)
{
}

TextureManager::~TextureManager()
{
}

TextureManager* TextureManager::GetInstance()
{
    if (m_pInstance == nullptr)
        m_pInstance = new TextureManager();
    return m_pInstance;
}

void TextureManager::ReleaseInstance()
{
    if (m_pInstance != nullptr)
        SAFE_DELETE(m_pInstance);
}

bool TextureManager::Init()
{
    return true;
}


bool TextureManager::Release()
{
    for (auto iter : m_textureList)
    {
        iter.second->Release();
        SAFE_DELETE(iter.second);
    }
    m_textureList.clear();
    ReleaseInstance();
    return true;
}

std::wstring TextureManager::SplitPath(std::wstring path, std::wstring entry)
{
    TCHAR szFileName[MAX_PATH] = { 0, };
    TCHAR Dirve[MAX_PATH] = { 0, };
    TCHAR Dir[MAX_PATH] = { 0, };
    TCHAR FileName[MAX_PATH] = { 0, };
    TCHAR FileExt[MAX_PATH] = { 0, };

    std::wstring fullpathname = path;
    _tsplitpath_s(fullpathname.c_str(), Dirve, Dir, FileName, FileExt);
    std::wstring name = FileName;
    name += FileExt;

    if (!entry.empty())
    {
        name += entry;
   }
    return name;
}

Texture* TextureManager::Load(std::wstring fileName)
{
    std::wstring name = SplitPath(fileName, L""); // 먼저 이름 + 확장자를 가져온다.
    Texture* pData = GetData(name); // 있는지 확인
    if (pData != nullptr)
        return pData;

    pData = new Texture();
    if (pData->Load(fileName) == false) // 텍스쳐생성
    {
        SAFE_DELETE(pData);
        return nullptr;
    }
    pData->SetName(name);
    m_textureList.insert(std::make_pair(name, pData));
    m_index++;
    return pData;
}

Texture* TextureManager::GetData(std::wstring& key)
{
    auto iter = m_textureList.find(key);
    if (iter != m_textureList.end())
        return iter->second;
    return nullptr;
}
