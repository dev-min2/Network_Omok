#include "ShaderManager.h"
#include "Shader.h"
ShaderManager* ShaderManager::m_pInstance = nullptr;
ShaderManager* ShaderManager::GetInstance()
{
	if (m_pInstance == nullptr)
		m_pInstance = new ShaderManager();
	return m_pInstance;
}

void ShaderManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
		SAFE_DELETE(m_pInstance);
}

Shader* ShaderManager::CreateVertexShader(std::wstring fileName, std::string entry)
{
	std::wstring name = SplitPath(fileName, Multi2Uni(entry));
	Shader* pData = GetData(name);
	if (pData != nullptr) // 있다면
	{
		return pData;
	}
	pData = new Shader();
	if (!pData->CreateVertexShader(fileName, entry))
	{
		SAFE_DELETE(pData);
		return nullptr;
	}
	pData->SetName(name);
	m_shaderList.insert(std::make_pair(pData->GetName(), pData));
	m_index++;
	return pData;
}

Shader* ShaderManager::CreatePixelShader(std::wstring fileName, std::string entry)
{
	std::wstring name = SplitPath(fileName, Multi2Uni(entry));
	Shader* pData = GetData(name);
	if (pData != nullptr) // 있다면
	{
		return pData;
	}
	pData = new Shader();
	if (!pData->CreatePixelShader(fileName, entry))
	{
		SAFE_DELETE(pData);
		return nullptr;
	}
	pData->SetName(name);
	m_shaderList.insert(std::make_pair(pData->GetName(), pData));
	m_index++;
	return pData;
}

std::wstring ShaderManager::SplitPath(std::wstring path, std::wstring entry)
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

Shader* ShaderManager::Load(std::wstring fileName)
{
	TCHAR szFileName[MAX_PATH] = { 0, };
	TCHAR Dirve[MAX_PATH] = { 0, };
	TCHAR Dir[MAX_PATH] = { 0, };
	TCHAR FileName[MAX_PATH] = { 0, };
	TCHAR FileExt[MAX_PATH] = { 0, };

	std::wstring fullpathname = fileName;
	_tsplitpath_s(fullpathname.c_str(), Dirve, Dir, FileName, FileExt);
	std::wstring name = FileName;
	name += FileExt;

	for (auto data : m_shaderList)
	{
		if (data.second->GetName() == name)
		{
			return data.second;
		}
	}
	return nullptr;
}

Shader* ShaderManager::GetData(std::wstring& key)
{
	auto iter = m_shaderList.find(key);
	if (iter != m_shaderList.end())
		return iter->second;
	return nullptr;
}

bool ShaderManager::Init()
{
	return true;
}

bool ShaderManager::Release()
{
	for (auto iter : m_shaderList)
	{
		iter.second->Release();
		SAFE_DELETE(iter.second);
	}
	m_shaderList.clear();
	ReleaseInstance();
	return true;
}

ShaderManager::ShaderManager() : m_index(0)
{
}

ShaderManager::~ShaderManager()
{
}
