#pragma once
#include "std.h"
class Texture;
// 리소스매니저들은 파일확장자따오기, 이미 생성을한것인지를 체크한다.(이미생성한거면 리턴)
class TextureManager
{
private:
	TextureManager();
	~TextureManager();
public:
	static TextureManager*				GetInstance();
public:
	bool								Init();
	bool								Release();
public:

	std::wstring						SplitPath(std::wstring path, std::wstring entry);
	Texture*							Load(std::wstring fileName);
	Texture*							GetData(std::wstring& key);
private:
	static void							ReleaseInstance();
private:
	static	TextureManager*				m_pInstance;
	std::map<std::wstring, Texture*>	m_textureList;
	DWORD								m_index;
};

