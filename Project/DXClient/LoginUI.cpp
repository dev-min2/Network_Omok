#include "LoginUI.h"
#include "Input.h"
#include "Scene.h"
#include "CreateAccountScene.h"
#include "ObjectManager.h"
#include "AsyncIO.h"
#include "OmokProtocol.h"
#include "ClientServer.h"
#include <conio.h>
#include "LoginScene.h"
LoginUI::LoginUI() : m_idPressed(false),m_psPressed(false)
{
	m_idBuffer.reserve(20);
	m_psBuffer.reserve(20);
	m_psSecretBuffer.reserve(20);
}
LoginUI::~LoginUI()
{
}
bool LoginUI::Init()
{

	//m_idRect = { g_clientRect.left,g_clientRect.top,440,444 };
	//m_idRect = { 440,444,g_clientRect.right,g_clientRect.bottom };
	

	m_idRect = { static_cast<LONG>(m_pos.x - 70.0f),static_cast<LONG>(m_pos.y - 52.0f),g_clientRect.right,g_clientRect.bottom }; // 585,395
	m_psRect = { static_cast<LONG>(m_pos.x - 70.0f),static_cast<LONG>(m_pos.y + 16.0f),g_clientRect.right,g_clientRect.bottom }; // 585,449
	
	m_inputIdRect		= { 694,490,900,525 };
	m_inputPsRect		= { 694,561,900,593 };
	m_inputAccountRect  = { 665,692,776,730 };
	m_inputLoginRect	= { 812,692,926,730 };
	return true;
}

bool LoginUI::Update()
{
	InputKeyCheck();

	return true;
}


bool LoginUI::Release()
{
	DXObject::Release();
	m_idBuffer.clear();
	m_psBuffer.clear();
	return true;
}

bool LoginUI::Render()
{
	DXObject::Render();
	if (m_idBuffer.size() > 0)
		g_wt->Draw(m_idBuffer, m_idRect, D2D1::ColorF(0, 0, 0, 1));
	/*if (m_psBuffer.size() > 0)
		g_wt->Draw(m_psBuffer, m_psRect, D2D1::ColorF(0, 0, 0, 1));*/
	if (m_psSecretBuffer.size() > 0)
		g_wt->Draw(m_psSecretBuffer, m_psRect, D2D1::ColorF(0, 0, 0, 1));
	return true;
}

void LoginUI::HitSelect(BaseObject* pObj, DWORD state)
{
	// 468 409  615 435 (ID)
	// 479 473 609 496 (PS)
	LONG clickX = g_mouse.x;
	LONG clickY = g_mouse.y;
	if (m_selectState & SS_SELECTED)
	{
		CheckIdUIArea(clickX, clickY);
		CheckPsUIArea(clickX, clickY);
		CheckCreateAccountUIArea(clickX,clickY);
		CheckLoginUIArea(clickX, clickY);
	}
}

void LoginUI::InputKeyCheck()
{

	if (Input::GetInstance()->GetPressedKey(VK_CAPITAL))
		m_capsLock = !m_capsLock;
	
	PressId();
	PressPs();

	
}

void LoginUI::InputPSKeyCheck()
{
}

bool LoginUI::CheckIdUIArea(LONG x, LONG y)
{
	if (x >= m_inputIdRect.left && x <= m_inputIdRect.right)
	{
		if (y >= m_inputIdRect.top && y <= m_inputIdRect.bottom)
		{
			m_idPressed = !m_idPressed;
			return true;
		}
		else
		{
			m_idPressed = false;
		}
	}
	
	return false;
}

bool LoginUI::CheckPsUIArea(LONG x, LONG y)
{
	if (x >= m_inputPsRect.left && x <= m_inputPsRect.right)
	{
		if (y >= m_inputPsRect.top && y <= m_inputPsRect.bottom)
		{
			m_psPressed = !m_psPressed;
			return true;
		}
		else
		{
			m_psPressed = false;
		}
	}
	return false;
}

bool LoginUI::CheckCreateAccountUIArea(LONG x, LONG y)
{
	if (x >= 665.0f && x <= 776.0f)
	{
		if (y >= 692.0f && y <= 730.0f)
		{
			LoginScene* pScene = static_cast<LoginScene*>(Scene::m_pScene);
			CreateAccountScene* ca = new CreateAccountScene();
			Scene::m_pScene->ChangeScene(ca,ST_ACCOUNT,true,true,false);
			SAFE_DELETE(pScene);
			return true;
		}
	}
	return false;
}

bool LoginUI::CheckLoginUIArea(LONG x, LONG y)
{
	if (x >= m_inputLoginRect.left && x <= m_inputLoginRect.right)
	{
		if (y >= m_inputLoginRect.top && y <= m_inputLoginRect.bottom)
		{
			if (m_idBuffer.size() <= 0 || m_psBuffer.size() <= 0)
			{
				::MessageBox(g_WindowHandle, L"공란 입력", L"로그인 요청", 0);
				return false;
			}
			Packet packet;
			packet.SetProtocolID(static_cast<uInt16>(OmProtocol::CS_REQU_LOGIN));
			packet << m_idBuffer << m_psBuffer;
			AsyncIO::GetInstance()->SendPacket(packet);
			ClientServer::GetInstance()->SetLoginScene(false); // 실패하면 다시 바꿔야함.
			return true;
		}
	}
	return false;
}

void LoginUI::PressId()
{
	char bigC = 'A';
	char num = '0';
	if (m_idPressed)
	{
		if (Input::GetInstance()->GetPressedKey(VK_BACK))//VK_BACK
		{
			if (m_idBuffer.size() > 0)
				m_idBuffer.pop_back();
			return;
		}
		if (m_idBuffer.size() >= 10)
			return;
		if (Input::GetInstance()->GetPressedKey(VK_TAB))
		{
			m_idPressed = false;
			m_psPressed = true;
			return;
		}
		if (!m_capsLock)
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_idBuffer += (bigC + i + 32);
				}
			}
		}
		else
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_idBuffer += (bigC + i);
				}
			}
		}

		for (int i = 0; i < 10; ++i)
		{
			if (Input::GetInstance()->GetPressedKey(num + i))
			{
				m_idBuffer += (num + i);
			}
		}
	}
}

void LoginUI::PressPs()
{
	char bigC = 'A';
	char num = '0';
	if (m_psPressed)
	{
		if (Input::GetInstance()->GetPressedKey(VK_BACK))//VK_BACK
		{
			if (m_psBuffer.size() > 0)
			{
				m_psBuffer.pop_back();
				m_psSecretBuffer.pop_back();
			}
			return;
		}
		if (m_psBuffer.size() >= 10)
			return;
		if (!m_capsLock)
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_psBuffer += (bigC + i + 32);
					m_psSecretBuffer += L'*';
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
					m_psSecretBuffer += L'*';
				}
			}
		}

		for (int i = 0; i < 10; ++i)
		{
			if (Input::GetInstance()->GetPressedKey(num + i))
			{
				m_psBuffer += (num + i);
				m_psSecretBuffer += L'*';
			}
		}
	}
}
