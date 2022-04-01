#include "Texture.h"

Texture::Texture() : m_index(0),m_pTexture(nullptr),m_pSRV(nullptr)
{
	::ZeroMemory(m_buffer, sizeof(m_buffer));
}

Texture::~Texture()
{
}

bool Texture::Load(std::wstring& fileName)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(
		g_pd3dDevice,
		fileName.c_str(),
		(ID3D11Resource**)&m_pTexture,
		&m_pSRV);
	if (FAILED(hr))
	{
		hr = DirectX::CreateDDSTextureFromFile(
			g_pd3dDevice,
			fileName.c_str(),
			(ID3D11Resource**)&m_pTexture,
			&m_pSRV);
		if (FAILED(hr))
		{
			return false;
		}
	}
	m_pTexture->GetDesc(&m_textureDesc);
	return true;
}

bool Texture::Release()
{
	SAFE_DX_RELEASE(m_pTexture); 
	SAFE_DX_RELEASE(m_pSRV);
	return true;
}
