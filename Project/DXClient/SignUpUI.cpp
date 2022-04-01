#include "SignUpUI.h"
#include "DXWrite.h"
#include "Input.h"
#include "AsyncIO.h"
#include "OmokProtocol.h"
SignUpUI::SignUpUI() : m_capsLock(false)
{
	::ZeroMemory(m_pressed, sizeof(m_pressed));
	::ZeroMemory(m_tabKey, sizeof(m_tabKey));
	for (int buf = 0; buf < MAX_INPUT_SIZE; ++buf)
		m_inputBuffer[buf].reserve(15);

	//578 290
	
	//656 362
	// 656 467
	m_inputRect[IT_ID] = { 656,362,929,400 };
	m_inputRect[IT_PS] = { 656,467, 929, 502 };
	m_inputRect[IT_AGE] = { 656,575,929,612 };

	m_nextRect = { 726,663,848,717 };
}

SignUpUI::~SignUpUI()
{
}

bool SignUpUI::Init()
{

	m_writeRect[IT_ID] = { static_cast<LONG>(m_pos.x - 25.0f),static_cast<LONG>(m_pos.y - 185.0f),g_clientRect.right,g_clientRect.bottom };
	m_writeRect[IT_PS] = { static_cast<LONG>(m_pos.x - 25.0f),static_cast<LONG>(m_pos.y - 80.0f),g_clientRect.right,g_clientRect.bottom };
	m_writeRect[IT_AGE] = { static_cast<LONG>(m_pos.x - 25.0f),static_cast<LONG>(m_pos.y + 30.0f),g_clientRect.right,g_clientRect.bottom };
	//460
	return true;
}

bool SignUpUI::Update()
{
	InputKeyCheck();
	return true;
}

bool SignUpUI::Render()
{
	DXObject::Render();
	WriteRender();
	return true;
}

bool SignUpUI::Release()
{
	DXObject::Release();
	for (int del = 0; del < MAX_INPUT_SIZE; ++del)
		m_inputBuffer[del].clear();
	return true;
}

void SignUpUI::HitSelect(BaseObject* pObj, DWORD state)
{
	LONG clickX = g_mouse.x;
	LONG clickY = g_mouse.y;
	if (m_selectState & SS_SELECTED)
	{
		CheckIdArea(clickX, clickY);
		CheckPsArea(clickX, clickY);
		CheckAgeArea(clickX, clickY);
		ClickNext(clickX, clickY);
	}
}

bool SignUpUI::CheckIdArea(LONG clickX, LONG clickY)
{
	//m_inputRect[IT_ID] = { 656,362,929,400 };
	if (clickX >= m_inputRect[IT_ID].left && clickX <= m_inputRect[IT_ID].right)
	{
		if (clickY >= m_inputRect[IT_ID].top && clickY <=  m_inputRect[IT_ID].bottom)
		{
			m_pressed[IT_ID] = !m_pressed[IT_ID];
			return true;
		}
		else
		{
			m_pressed[IT_ID] = false;
		}
	}
	return false;
}

bool SignUpUI::CheckPsArea(LONG clickX, LONG clickY)
{
	if (clickX >=  m_inputRect[IT_PS].left && clickX <= m_inputRect[IT_PS].right)
	{
		if (clickY >= m_inputRect[IT_PS].top && clickY <= m_inputRect[IT_PS].bottom)
		{
			m_pressed[IT_PS] = !m_pressed[IT_PS];
			return true;
		}
		else
		{
			m_pressed[IT_PS] = false;
		}
	}
	return false;
}

bool SignUpUI::CheckAgeArea(LONG clickX, LONG clickY)
{
	if (clickX >= m_inputRect[IT_AGE].left && clickX <= m_inputRect[IT_AGE].right)
	{
		if (clickY >= m_inputRect[IT_AGE].top && clickY <= m_inputRect[IT_AGE].bottom)
		{
			m_pressed[IT_AGE] = !m_pressed[IT_AGE];
			return true;
		}
		else
		{
			m_pressed[IT_AGE] = false;
		}
	}
	return false;
}

bool SignUpUI::ClickNext(LONG clickX, LONG clickY)
{
	if (clickX >= m_nextRect.left && clickX <= m_nextRect.right)
	{
		if (clickY >= m_nextRect.top && clickY <= m_nextRect.bottom)
		{
			if (m_inputBuffer[IT_ID].size() <= 0 || m_inputBuffer[IT_PS].size() <= 0 || m_inputBuffer[IT_AGE].size() <= 0)
			{
				::MessageBox(g_WindowHandle, L"공란 입력", L"계정생성 실패", 0);
				return false; 
			}
			// 패킷 송신
			{
				Packet packet;
				packet.SetProtocolID(static_cast<uInt16>(OmProtocol::CS_REQU_ACCOUNT));
				packet << m_inputBuffer[IT_ID] << m_inputBuffer[IT_PS] << m_inputBuffer[IT_AGE];
				AsyncIO::GetInstance()->SendPacket(packet);
			}
			return true;
		}
	}
	return false;
}

void SignUpUI::InputKeyCheck()
{

	if (Input::GetInstance()->GetPressedKey(VK_CAPITAL))
		m_capsLock = !m_capsLock;
	
	PressId();
	PressPs();
	PressAge();
}

void SignUpUI::PressId()
{
	char bigC = 'A';
	char num = '0';
	if (m_pressed[IT_ID])
	{
		if (Input::GetInstance()->GetPressedKey(VK_BACK))//VK_BACK
		{
			if (m_inputBuffer[IT_ID].size() > 0)
			{
				m_inputBuffer[IT_ID].pop_back();
				m_writeRect[IT_ID].left += 4;
			}
			return;
		}
		if (m_inputBuffer[IT_ID].size() >= 10)
			return;
		if (!m_capsLock)
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_inputBuffer[IT_ID] += (bigC + i + 32);
					m_writeRect[IT_ID].left -= 4;
				}
			}
		}
		else
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_inputBuffer[IT_ID] += (bigC + i);
					m_writeRect[IT_ID].left -= 4;
				}
			}
		}

		for (int i = 0; i < 10; ++i)
		{
			if (Input::GetInstance()->GetPressedKey(num + i))
			{
				m_inputBuffer[IT_ID] += (num + i);
				m_writeRect[IT_ID].left -= 4;
			}
		}
		
	}
}

void SignUpUI::PressPs()
{
	
	char bigC = 'A';
	char num = '0';
	if (m_pressed[IT_PS])
	{
		if (Input::GetInstance()->GetPressedKey(VK_BACK))//VK_BACK
		{
			if (m_inputBuffer[IT_PS].size() > 0)
			{
				m_inputBuffer[IT_PS].pop_back();
				m_writeRect[IT_PS].left += 4;
			}
			return;
		}
		if (m_inputBuffer[IT_PS].size() >= 10)
			return;
		if (!m_capsLock)
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_inputBuffer[IT_PS] += (bigC + i + 32);

					m_writeRect[IT_PS].left -= 4;
				}
			}
		}
		else
		{
			for (int i = 0; i < 26; ++i)
			{
				if (Input::GetInstance()->GetPressedKey(bigC + i))
				{
					m_inputBuffer[IT_PS] += (bigC + i);
					m_writeRect[IT_PS].left -= 4;
				}
			}
		}

		for (int i = 0; i < 10; ++i)
		{
			if (Input::GetInstance()->GetPressedKey(num + i))
			{
				m_inputBuffer[IT_PS] += (num + i);
				m_writeRect[IT_PS].left -= 4;
			}
		}
		
	}
}

void SignUpUI::PressAge()
{
	
	char num = '0';
	if (m_pressed[IT_AGE])
	{
		if (Input::GetInstance()->GetPressedKey(VK_BACK))//VK_BACK
		{
			if (m_inputBuffer[IT_AGE].size() > 0)
				m_inputBuffer[IT_AGE].pop_back();
			return;
		}
		if (m_inputBuffer[IT_AGE].size() >= 2)
			return;
		
		for (int i = 0; i < 10; ++i)
		{
			if (Input::GetInstance()->GetPressedKey(num + i))
			{
				m_inputBuffer[IT_AGE] += (num + i);
			}
		}
	}
}

void SignUpUI::WriteRender()
{
	if (m_inputBuffer[IT_ID].size() > 0)
		g_wt->Draw(m_inputBuffer[IT_ID], m_writeRect[IT_ID], D2D1::ColorF(0, 0, 0, 1));
	if (m_inputBuffer[IT_PS].size() > 0)
		g_wt->Draw(m_inputBuffer[IT_PS], m_writeRect[IT_PS], D2D1::ColorF(0, 0, 0, 1));
	if (m_inputBuffer[IT_AGE].size() > 0)
		g_wt->Draw(m_inputBuffer[IT_AGE], m_writeRect[IT_AGE], D2D1::ColorF(0, 0, 0, 1));
}
