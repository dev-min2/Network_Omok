#pragma once
#include "std.h"
class Shader;
class ShaderManager 
{
public:
	static	ShaderManager*				GetInstance();
	static	void						ReleaseInstance();
public:
	Shader*								CreateVertexShader(std::wstring fileName, std::string entry);
	Shader*								CreatePixelShader(std::wstring fileName, std::string entry);
public:
	std::wstring						SplitPath(std::wstring path, std::wstring entry);
	Shader*								Load(std::wstring fileName);
	Shader*								GetData(std::wstring& key);
public:
	bool								Init();
	bool								Release();
private:
	ShaderManager();
	~ShaderManager();
private:
	static ShaderManager*				m_pInstance;
	std::map<std::wstring, Shader*>		m_shaderList;
	DWORD								m_index;


};

