#include "CreateRoomUI.h"
#include "DXWrite.h"
#include "Input.h"
#include "OkButton.h"
#include "CancleButton.h"
#include "Scene.h"
CreateRoomUI::CreateRoomUI() : 
	m_hide(true), m_capsLock(false), m_pressTitle(false),m_pressPs(false), m_roomCount(0),
	m_pOkButton(nullptr),m_pCancleButton(nullptr)
{
	
	m_titleRect  = { 709,382,941,402 };
	m_roomPSRect = { 877,416,944,427 };

	m_titleBuffer.reserve(30);
	m_psBuffer.reserve(5);
}

CreateRoomUI::~CreateRoomUI()
{
}

bool CreateRoomUI::Init()
{
	CreateButton();
	Vector2 okBtnPos     = m_pOkButton->GetPos();
	Vector2 cancelBtnPos = m_pCancleButton->GetPos();
	m_inputTitleRect = { static_cast<LONG>(okBtnPos.x - 10.0f),static_cast<LONG>(okBtnPos.y - 140.0f),g_clientRect.right,g_clientRect.bottom };
	m_inputPsRect = { static_cast<LONG>(cancelBtnPos.x + 10.0f),static_cast<LONG>(cancelBtnPos.y - 108.0f),g_clientRect.right,g_clientRect.bottom };
    return true;
}

bool CreateRoomUI::Update()
{
	if (!m_hide)
	{
		InputKeyCheck();
	}
    return true;
}

bool CreateRoomUI::Render()
{
	if (!m_hide)
	{
		DXObject::Render();
		WriteRender();
		m_pOkButton->Render();
		m_pCancleButton->Render();
	}
    return true;
}

bool CreateRoomUI::Release()
{
    DXObject::Release();
	m_pOkButton->Release();
	m_pCancleButton->Release();
	SAFE_DELETE(m_pOkButton);
	SAFE_DELETE(m_pCancleButton);
    return true;
}

void CreateRoomUI::HitSelect(BaseObject* pObj, DWORD state)
{
	LONG clickX = g_mouse.x;
	LONG clickY = g_mouse.y;
	if (m_selectState & SS_SELECTED)
	{
		CheckTitleArea(clickX,clickY);
		CheckRoomPsArea(clickX,clickY);
	}
}

void CreateRoomUI::ReverseHide()
{
	m_hide = !m_hide;
	if (m_hide) // 닫는다면 버퍼 비우기.
	{
		m_titleBuffer.clear();
		m_psBuffer.clear();
		m_pressTitle = false;
		m_pressPs = false;
	}
}

void CreateRoomUI::InputKeyCheck()
{
	if (Input::GetInstance()->GetPressedKey(VK_CAPITAL))
		m_capsLock = !m_capsLock;

	PressTitle();
	PressPs();
	
}

bool CreateRoomUI::CheckTitleArea(LONG clickX, LONG clickY)
{
	if (clickX >= m_titleRect.left && clickX <= m_titleRect.right)
	{
		if (clickY >= m_titleRect.top && clickY <= m_titleRect.bottom)
		{
			m_pressTitle = !m_pressTitle;
			return true;
		}
		else
			m_pressTitle = false;
	}
	return false;
}

bool CreateRoomUI::CheckRoomPsArea(LONG clickX, LONG clickY)
{
	if (clickX >= m_roomPSRect.left && clickX <= m_roomPSRect.right)
	{
		if (clickY >= m_roomPSRect.top && clickY <= m_roomPSRect.bottom)
		{
			m_pressPs = !m_pressPs;
			return true;
		}
		else
			m_pressPs = false;
	}
	return false;
}

void CreateRoomUI::PressTitle()
{
	char bigC = 'A';
	char num  = '0';
	if (m_pressTitle)
	{
		if (Input::GetInstance()->GetPressedKey(VK_BACK))//VK_BACK
		{
			if (m_titleBuffer.size() > 0)
				m_titleBuffer.pop_back();
			return;
		}
		if (m_titleBuffer.size() >= 15)
			return;
		if (!m_capsLock)
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_titleBuffer += (bigC + i + 32);
				}
			}
		}
		else
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_titleBuffer += (bigC + i);
				}
			}
		}

		for (int i = 0; i < 10; ++i)
		{
			if (Input::GetInstance()->GetPressedKey(num + i))
			{
				m_titleBuffer += (num + i);
			}
		}
	}
}

void CreateRoomUI::PressPs()
{
	char bigC = 'A';
	char num = '0';
	if (m_pressPs)
	{
		if (Input::GetInstance()->GetPressedKey(VK_BACK))//VK_BACK
		{
			if (m_psBuffer.size() > 0)
				m_psBuffer.pop_back();
			return;
		}
		if (m_psBuffer.size() >= 6)
			return;
		if (!m_capsLock)
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_psBuffer += (bigC + i + 32);
				}
			}
		}
		else
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_psBuffer += (bigC + i);
				}
			}
		}

		for (int i = 0; i < 10; ++i)
		{
			if (Input::GetInstance()->GetPressedKey(num + i))
			{
				m_psBuffer += (num + i);
			}
		}
	}
}

void CreateRoomUI::WriteRender()
{
	if (m_titleBuffer.size() > 0)
		g_wt->DrawUserFormat(m_titleBuffer, m_inputTitleRect, D2D1::ColorF(0, 0, 0, 1));
	if (m_psBuffer.size() > 0)
		g_wt->DrawUserFormat(m_psBuffer, m_inputPsRect, D2D1::ColorF(0, 0, 0, 1));
	
}

bool CreateRoomUI::CreateButton()
{
	m_pOkButton = new OkButton(this);
	m_pOkButton->Init();
	m_pOkButton->SetRectSource({ 0,0,90,39 });
	m_pOkButton->SetRectDraw({ 0,0,90,39 });
	m_pOkButton->SetPosition(Vector2(g_clientRect.right / 2.0f - 75.0f, g_clientRect.bottom / 2.0f + 75.0f)); // 800 450
	m_pOkButton->SetCollision(CT_IGNORE, ST_OVERLAP, ST_CHANNEL);
	if (!m_pOkButton->Create(g_pd3dDevice, g_pContext,
		L"../../data/Shader/VertexShader.txt",
		L"../../data/Shader/PixelShader.txt",
		L"../../data/omok/Channel/Alert_Ok.BMP",
		L"../../data/omok/Channel/Alert_Cancel-mask.bmp"
	))
	{
		return false;
	}
	m_pCancleButton = new CancleButton(this);
	m_pCancleButton->Init();
	m_pCancleButton->SetRectSource({ 0,0,90,39 });
	m_pCancleButton->SetRectDraw({ 0,0,90,39 });
	m_pCancleButton->SetPosition(Vector2(g_clientRect.right / 2.0f + 75.0f, g_clientRect.bottom / 2.0f + 75.0f)); // 800 450
	m_pCancleButton->SetCollision(CT_IGNORE, ST_OVERLAP, ST_CHANNEL);
	if (!m_pCancleButton->Create(g_pd3dDevice, g_pContext,
		L"../../data/Shader/VertexShader.txt",
		L"../../data/Shader/PixelShader.txt",
		L"../../data/omok/Channel/Alert_Cancel.BMP",
		L"../../data/omok/Channel/Alert_Cancel-mask.bmp"
	))
	{
		return false;
	}
	return true;
}
