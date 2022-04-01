#include "DXObject.h"
#include "ObjectManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Texture.h"
#include "Shader.h"

void DXObject::SetDevice(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pContext = pContext;
}

bool DXObject::Create(ID3D11Device* pd3dDevice, 
	ID3D11DeviceContext* pContext,
	const TCHAR* VSShaderFileName,
	const TCHAR* PSShaderFileName,
	const TCHAR* textureFileName ,
	const TCHAR* maskFileName )
{

	HRESULT hr;


	SetDevice(pd3dDevice, pContext);
	//화면좌표계
	// 0,0   ~      800,0
	// 
	//      400,300
	// 
	// 0,600        800,600
	//직각좌표계(왼손좌표계)    ->  NDC좌표로 변환 -> 화면좌표계
	// -1,1   ~      1,1             z=(0~1)
	// 
	//        0,0
	// 
	// -1,-1          1,-1
	
	if (!LoadTexture(textureFileName, maskFileName))
		return false;
	if (!SetVertexData()) // 2D,3D 선택.
		return false;
	if (!SetIndexData()) // 2D,3D 선택.
		return false;
	if (!SetConstantData())
		return false;

	//GPU에 버퍼 할당. 정점버퍼,인덱스버퍼 생성 및 세이더 생성
	if (!CreateVertexBuffer())
		return false;
	if (!CreateIndexBuffer())
		return false;
	if (!CreateConstantBuffer())
		return false;
	if (!CompileShaderVS(VSShaderFileName, "VS"))
		return false;
	if (!CompileShaderPS(PSShaderFileName, "PS"))
		return false;

	if (!CreateInputLayout())
		return false;




	// (소스컬러*D3D11_BLEND_SRC_ALPHA) 
	//                  + 
	// (대상컬러*D3D11_BLEND_INV_SRC_ALPHA)
	// 컬러   =  투명컬러값 = (1,1,1,1)
	// 마스크 =  1.0 - 투명컬러값 = (1,1,1,1)

	// FinalColor = SrcColor*SrcAlpha + DestColor*(1.0f- SrcAlpha) 	    
	// if SrcAlpha == 0 완전투명
	//           FinalColor() = SrcColor*0 + DestColor*(1-0)
	//                FinalColor = DestColor;
	// if SrcAlpha == 1 완전불투명
	//           FinalColor() = SrcColor*1 + DestColor*(1-1)
	//                FinalColor = SrcColor;
	// 혼합상태 = 소스(지금드로우객체 픽셀) (연산) 대상(백버퍼 객체:픽셀)
	// 혼합상태 = 픽셀쉐이더 출력 컬러  (연산:사칙연산) 출력버퍼의 컬러
	if (maskFileName != nullptr)
	{
		D3D11_BLEND_DESC  blenddesc;
		ZeroMemory(&blenddesc, sizeof(D3D11_BLEND_DESC));
		/*bd.AlphaToCoverageEnable;
		bd.IndependentBlendEnable;*/
		blenddesc.RenderTarget[0].BlendEnable = TRUE;
		blenddesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blenddesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blenddesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//// A 연산 저장
		blenddesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blenddesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blenddesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blenddesc.RenderTarget[0].RenderTargetWriteMask =
			D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = m_pd3dDevice->CreateBlendState(&blenddesc, &m_alphaBlend);

		blenddesc.RenderTarget[0].BlendEnable = FALSE;
		hr = m_pd3dDevice->CreateBlendState(&blenddesc, &m_alphaBlendDisable);
	}
	return true;
}

bool DXObject::LoadTexture(const TCHAR* colorFileName, const TCHAR* maskFileName)
{
	m_pColorTexture = TextureManager::GetInstance()->Load(colorFileName);
	if(maskFileName != nullptr)
		m_pMaskTexture = TextureManager::GetInstance()->Load(maskFileName);
	m_textureDesc = m_pColorTexture->GetTextureDesc();
	return true;
}

bool DXObject::SetVertexData()
{
	return true;
}

bool DXObject::SetIndexData()
{
	return true;
}

bool DXObject::SetConstantData()
{
	return true;
}

bool DXObject::Init()
{
	return true;
}

bool DXObject::Update()
{

	return true;
}

bool DXObject::Render()
{
	// 모두 세팅했으면 렌더링 파이프라인에 실어보낸다.
	if (m_pColorTexture != nullptr)
		m_pContext->PSSetShaderResources(0, 1, m_pColorTexture->GetSRV());
	if (m_pMaskTexture != nullptr)
		m_pContext->PSSetShaderResources(1, 1, m_pMaskTexture->GetSRV());

	if (m_pVSShader != nullptr)
		m_pContext->VSSetShader(m_pVSShader->GetVertexShader(),NULL,0);

	if (m_pPSShader != nullptr)
		m_pContext->PSSetShader(m_pPSShader->GetPixelShader(), NULL, 0);

	if(m_blend)
		m_pContext->OMSetBlendState(m_alphaBlend, 0, -1);
	else
		m_pContext->OMSetBlendState(m_alphaBlendDisable, 0, -1);

	

	m_pContext->IASetInputLayout(m_pVertexLayout);

	UINT startSlot;
	UINT numBuffers;
	UINT strides = sizeof(SimpleVertex);
	UINT offset = 0;

	// 생성한 버퍼 셋
	m_pContext->IASetVertexBuffers(
		0, 1, &m_pVertexBuffer, &strides, &offset
	);
	m_pContext->IASetIndexBuffer(
		m_pindexBuffer,DXGI_FORMAT_R32_UINT,0
	);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);
	if (m_indexList.size() <= 0)
		m_pContext->Draw(m_vertexList.size(), 0);
	else
		m_pContext->DrawIndexed(m_indexList.size(), 0, 0);
	return true;
}

bool DXObject::Release()
{
	SAFE_DX_RELEASE(m_alphaBlend);
	SAFE_DX_RELEASE(m_alphaBlendDisable);
	SAFE_DX_RELEASE(m_pVertexBuffer);
	SAFE_DX_RELEASE(m_pindexBuffer);
	SAFE_DX_RELEASE(m_pConstantBuffer);
	SAFE_DX_RELEASE(m_pVertexLayout);
	m_vertexList.clear();
	m_indexList.clear();

	// 매니저에서 다 지움
	//m_pColorTexture->Release();
	//SAFE_DELETE(m_pColorTexture);
	//m_pMaskTexture->Release();
	//SAFE_DELETE(m_pMaskTexture);

	return true;
}

bool DXObject::CompileShaderVS(const TCHAR* shaderFile, const CHAR* shaderFunc)
{
	m_pVSShader = ShaderManager::GetInstance()->CreateVertexShader(shaderFile, shaderFunc);
	return true;
}

bool DXObject::CompileShaderPS(const TCHAR* shaderFile, const CHAR* shaderFunc)
{
	m_pPSShader = ShaderManager::GetInstance()->CreatePixelShader(shaderFile, shaderFunc);
	return true;
}
bool DXObject::CreateVertexBuffer()
{
	HRESULT hr;
	if (m_vertexList.size() <= 0) return false;
	D3D11_BUFFER_DESC bd;
	::ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(SimpleVertex) * m_vertexList.size(); // 크기 할당.
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	::ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
	sd.pSysMem = &m_vertexList.at(0);

	if (FAILED(hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &m_pVertexBuffer)))
	{
		return false;
	}
	return true;
}
bool DXObject::CreateIndexBuffer()
{
	HRESULT hr;
	if(m_indexList.size() <= 0) return false;
	D3D11_BUFFER_DESC bd;
	::ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(DWORD) * m_indexList.size(); // 크기 할당.
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	::ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
	sd.pSysMem = &m_indexList.at(0);

	if (FAILED(hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &m_pindexBuffer)))
	{
		return false;
	}
	return true;
}
bool DXObject::CreateConstantBuffer()
{
	HRESULT hr;
	//gpu메모리에 버퍼 할당(원하는 할당 크기)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(ConstantData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
	sd.pSysMem = &m_constantData;

	if (FAILED(hr = m_pd3dDevice->CreateBuffer(&bd, &sd, &m_pConstantBuffer)))
	{
		return false;
	}
	return true;
}
bool DXObject::CreateInputLayout()
{
	HRESULT hr;
	// 정점쉐이더의 결과를 통해서 정점레이아웃을 생성한다.	
	// 정점버퍼의 각 정점의 어떤 성분을 정점쉐이더에 전달할 거냐
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// 정점 좌표정보.
		{"POSITION",0, DXGI_FORMAT_R32G32_FLOAT, 0,0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// 텍스쳐 좌표 정보.(추가)
		// 5번째 인자 중요. position에서 vector2(float,float 4바이트)를 사용했으므로 그 다음 바이트인 8을 넘겨야함.
		{"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0,8,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT NumElements = sizeof(layout) / sizeof(layout[0]);
	hr = m_pd3dDevice->CreateInputLayout(
		layout,
		NumElements,
		m_pVSShader->GetVSBlob()->GetBufferPointer(),
		m_pVSShader->GetVSBlob()->GetBufferSize(),
		&m_pVertexLayout);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}
void DXObject::SetCollision(DWORD colType, DWORD selType, DWORD index)
{
	if (colType > CT_IGNORE)
		return;
	if (selType > ST_IGNORE)
		return;



	m_collision = Rect(m_pos, m_width, m_height);
	// m_collisionType이 null이면 애초에 넣지않도록.
	if (colType != CT_IGNORE)
	{
		m_collisionType = colType;
		ObjectManager::GetInstance().AddCollisionExecute(this, index,
			std::bind(&BaseObject::HitOverlap, this,
				std::placeholders::_1,
				std::placeholders::_2));
	}
	if (selType != ST_IGNORE)
	{
		m_selectType = selType;
		ObjectManager::GetInstance().AddSelectExecute(this, index,
			std::bind(&BaseObject::HitSelect, this, std::placeholders::_1, std::placeholders::_2));
	}
}
DXObject::DXObject() : m_pColorTexture(nullptr),m_pMaskTexture(nullptr),m_alphaBlend(nullptr),m_alphaBlendDisable(nullptr),
m_pVSShader(nullptr),m_pPSShader(nullptr),m_pVertexBuffer(nullptr),m_pindexBuffer(nullptr),m_pConstantBuffer(nullptr),
m_pVertexLayout(nullptr),m_pd3dDevice(nullptr),m_pContext(nullptr)
{
	m_vertexList.reserve(4);
	m_indexList.reserve(6);
}

DXObject::~DXObject()
{
	m_vertexList.clear();
	m_indexList.clear();
}

void BaseObject::HitOverlap(BaseObject* pObj, DWORD state)
{
}

void BaseObject::HitSelect(BaseObject* pObj, DWORD state)
{
}
