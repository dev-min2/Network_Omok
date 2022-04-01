#pragma once
#include "std.h"
class Shader
{
public:
	Shader();
	~Shader();
public:
	virtual bool					CreateVertexShader(std::wstring& filename, std::string entry = "VS");
	virtual bool					CreatePixelShader(std::wstring& filename, std::string entry = "PS");
public:
	inline ID3DBlob*				GetVSBlob() { return m_pVSCodeResult; }
	inline ID3DBlob*				GetPSBlob() { return m_pPSCodeResult; }
	inline ID3DBlob*				GetErrorBlob() { return m_pErrorMsg; }
	inline std::wstring				GetName() { return m_name; }
	inline void						SetName(std::wstring& name) { m_name = name;}
	inline ID3D11VertexShader*		GetVertexShader() { return m_pVertexShader; }
	inline ID3D11PixelShader*		GetPixelShader() { return m_pPixelShader; }
public:
	bool							Load(std::wstring& VSShadername, std::wstring& PSShaderName);
	bool							Init();
	bool							Update();
	bool							Render();
	bool							Release();
private:
	DWORD				m_index;
	std::wstring		m_name;
	std::wstring		m_msg;
	TCHAR				m_buffer[MAX_BUFFER];
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*	m_pPixelShader;
	ID3DBlob*			m_pVSCodeResult;
	ID3DBlob*			m_pErrorMsg;
	ID3DBlob*			m_pPSCodeResult;

};

