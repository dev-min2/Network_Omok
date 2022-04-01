#pragma once
#include "std.h"
#include "Collision.h"
#pragma comment(lib,"DirectXTK.lib")
#pragma comment(lib, "DirectXTex.lib")
class Texture;
class Shader;
struct SimpleVertex
{
	Vector2 vertexCoord;
	Vector2 texCoord;
};
enum CollisionType
{
	CT_BLOCK = 0,
	CT_OVERLAP,
	CT_IGNORE
};
enum SelectType
{
	ST_BLOCK = 0,
	ST_OVERLAP,
	ST_IGNORE
};
enum SelectState
{
	SS_DEFAULT	= 0x00000000, // 커서가 위에 없을 때. (FOCUS상태에서 다른 곳을 ACTIVE하면 전환)
	SS_HOVER	= 0x00000001, // 커서가 위에 있을 때.
	SS_FOCUS	= 0x00000010, // ACTIVE상태에서 왼쪽 버튼을 다른곳에서 놓았을 때.
	SS_ACTIVE	= 0x00000100, // 마우스 왼쪽버튼을 누르고 있을 때.
	SS_SELECTED = 0x00001000 // ACTIVE상태에서 왼쪽버튼 놓았을 때.
};

struct BaseObject
{
	virtual void HitOverlap(BaseObject* pObj, DWORD state);
	virtual void HitSelect(BaseObject* pObj, DWORD state); // 마우스
	BaseObject() : m_collisionID(-1),m_speed(200.0f),m_direction(0.0f,0.0f),m_width(0.f),m_height(0.f),
		m_collision(),m_collisionType(CT_IGNORE),m_selectType(ST_IGNORE),m_selectState(SS_DEFAULT), m_select(false),
		m_destroy(false), m_selectID(-1)
	{

	}
	virtual ~BaseObject() {}
	virtual bool Release() { return true; }
	std::wstring	m_name;
	bool			m_destroy;
	int				m_collisionID;
	int				m_selectID;
	float			m_speed;
	Vector2			m_pos;
	Vector2			m_direction;
	float			m_width;
	float			m_height;
	Rect			m_collision;
	DWORD			m_collisionType;
	DWORD			m_selectType;
	DWORD			m_selectState;
	bool			m_select;
	bool			m_blend = true;
};

struct ConstantData
{
	Vector4 color;
	Vector4 time;
};

// SetCollision함수 호출 후 (현재 씬의 오브젝트로) Create함수 호출.
class DXObject : public BaseObject
{
public:
	void    SetDevice(ID3D11Device* pd3dDevice,ID3D11DeviceContext* pContext);
	bool	Create(ID3D11Device* pd3dDevice,ID3D11DeviceContext* pContext,
		const TCHAR* VSShaderFileName,
		const TCHAR* PSShaderFileName,
		const TCHAR* textureFileName,
		const TCHAR* maskFileName = nullptr);
public:
	virtual bool	LoadTexture(const TCHAR* colorFileName, const TCHAR* maskFileName);
	virtual bool	SetVertexData();
	virtual bool	SetIndexData();
	virtual bool	SetConstantData();
	
public:
	virtual bool	Init();
	virtual bool	Update();
	virtual bool	Render();
	virtual bool	Release();
public:
	Texture*		GetTexture() { return m_pColorTexture; }
	void			SetTexture(Texture* pTexture) { m_pColorTexture = pTexture; }
private:
	bool			CompileShaderVS(const TCHAR* shaderFile, const CHAR* shaderFunc);
	bool			CompileShaderPS(const TCHAR* shaderFile, const CHAR* shaderFunc);
	bool			CreateVertexBuffer();
	bool			CreateIndexBuffer();
	bool			CreateConstantBuffer();
	bool			CreateInputLayout();
public:
	void			SetCollision(DWORD colType, DWORD selType,DWORD index);
	Vector2			GetPos() { return m_pos; }
	D3D11_TEXTURE2D_DESC& GetTextureDesc() { return m_textureDesc; }
public:
	DXObject();
	virtual ~DXObject();

protected:
	//Object에 삽입될 텍스쳐
	// ----노말-----
	Texture*					m_pColorTexture;
	// ----마스크----
	Texture*					m_pMaskTexture;
	// 알파블렌딩.
	ID3D11BlendState*			m_alphaBlend;
	ID3D11BlendState*			m_alphaBlendDisable;
	// ----쉐이더----
	Shader*						m_pVSShader;
	Shader*						m_pPSShader;

	D3D11_TEXTURE2D_DESC		m_textureDesc;

	//std::vector<SimpleVertex>	m_initScreenList;
	std::vector<SimpleVertex>	m_vertexList; // 정점(좌표,uv)의 정보를 들고있다.
	ID3D11Buffer*				m_pVertexBuffer; 
	std::vector<DWORD>			m_indexList; // 정점(좌표,uv)의 정보를 들고있다.
	ID3D11Buffer*				m_pindexBuffer;
	ConstantData				m_constantData;
	ID3D11Buffer*				m_pConstantBuffer;

	ID3D11InputLayout*			m_pVertexLayout;

	ID3D11Device*				m_pd3dDevice;
	ID3D11DeviceContext*		m_pContext;

};

