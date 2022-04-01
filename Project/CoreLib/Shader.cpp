#include "Shader.h"

Shader::Shader() : m_index(0),m_pVertexShader(nullptr),m_pPixelShader(nullptr),m_pVSCodeResult(nullptr),
                   m_pPSCodeResult(nullptr),m_pErrorMsg(nullptr)
{
    ::ZeroMemory(m_buffer, sizeof(m_buffer));
}

Shader::~Shader()
{
}

bool Shader::CreateVertexShader(std::wstring& fileName, std::string entry)
{
	HRESULT hr = ::D3DCompileFromFile(
		fileName.c_str(),
		NULL,
		NULL,
		entry.c_str(),
		"vs_5_0",
		0,
		0,
		&m_pVSCodeResult,
		&m_pErrorMsg
	);
	if (FAILED(hr))
	{
		::MessageBoxA(NULL,
			(char*)m_pErrorMsg->GetBufferPointer(),
			"ERROR", MB_OK);
		SAFE_DX_RELEASE(m_pErrorMsg);
		return false;
	}
	hr = g_pd3dDevice->CreateVertexShader(
		m_pVSCodeResult->GetBufferPointer(),
		m_pVSCodeResult->GetBufferSize(),
		NULL,
		&m_pVertexShader);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool Shader::CreatePixelShader(std::wstring& fileName, std::string entry)
{
	HRESULT hr = D3DCompileFromFile(
		fileName.c_str(),
		NULL,
		NULL,
		entry.c_str(),
		"ps_5_0",
		0,
		0,
		&m_pPSCodeResult,
		&m_pErrorMsg
	);
	if (FAILED(hr))
	{
		MessageBoxA(NULL,
			(char*)m_pErrorMsg->GetBufferPointer(),
			"ERROR", MB_OK);
		SAFE_DX_RELEASE(m_pErrorMsg);
		return false;
	}
	hr = g_pd3dDevice->CreatePixelShader(
		m_pPSCodeResult->GetBufferPointer(),
		m_pPSCodeResult->GetBufferSize(),
		NULL,
		&m_pPixelShader);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool Shader::Init()
{
    return true;
}

bool Shader::Update()
{
    return true;
}

bool Shader::Render()
{
    return true;
}

bool Shader::Release()
{
    SAFE_DX_RELEASE(m_pVertexShader);
    SAFE_DX_RELEASE(m_pPixelShader);
    SAFE_DX_RELEASE(m_pVSCodeResult);
    SAFE_DX_RELEASE(m_pErrorMsg);
    SAFE_DX_RELEASE(m_pPSCodeResult);
    return true;
}

bool Shader::Load(std::wstring& VSShaderName, std::wstring& PSShaderName)
{
	if (!CreateVertexShader(VSShaderName))
	{
		return false;
	}
	if (!CreatePixelShader(PSShaderName))
	{
		return false;
	}
	return true;
}
