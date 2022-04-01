#pragma once
#include "std.h"
#include "DXTK/DDSTextureLoader.h"
#include "DXTK/WICTextureLoader.h"
class Texture
{
public:
	Texture();
	~Texture();
public:
	D3D11_TEXTURE2D_DESC				GetTextureDesc() { return m_textureDesc; }
	inline std::wstring					GetName() { return m_name; }
	inline void							SetName(std::wstring name) { m_name = name; }
	inline ID3D11ShaderResourceView**	GetSRV() { return &m_pSRV; }
	bool								Load(std::wstring& fileName);
	bool								Release();
private:
	DWORD						m_index;
	std::wstring				m_name;
	std::wstring				m_msg;
	TCHAR						m_buffer[MAX_BUFFER];
	ID3D11Texture2D*			m_pTexture;
	ID3D11ShaderResourceView*	m_pSRV;
	D3D11_TEXTURE2D_DESC		m_textureDesc;

};

