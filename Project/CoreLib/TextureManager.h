#pragma once
#include "std.h"
class Texture;
// ���ҽ��Ŵ������� ����Ȯ���ڵ�����, �̹� �������Ѱ������� üũ�Ѵ�.(�̹̻����ѰŸ� ����)
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

