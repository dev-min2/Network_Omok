#include "OkButton.h"
#include "RoomUI.h"
#include "CreateRoomUI.h"
#include "Scene.h"
#include "AsyncIO.h"
#include "OmokProtocol.h"
#include "ChannelScene.h"
#include "Player.h"
OkButton::OkButton(CreateRoomUI* pUI) : m_pUI(pUI)
{
}

OkButton::~OkButton()
{
}

bool OkButton::Init()
{

	return true;
}

bool OkButton::Release()
{
	DXObject::Release();
	return true;
}


void OkButton::HitSelect(BaseObject* pObj, DWORD state)
{
	if (m_selectState & SS_SELECTED)
	{
		//TODO ��Ŷó����
		if (m_pUI->GetTitleLenght() > 0)
		{
			//CreateRoom(); ��Ŷ���� �� ó��.
			Packet packet;
			packet.SetProtocolID(static_cast<uInt16>(OmProtocol::CS_REQU_CREATE_ROOM));
			ChannelScene* cs = static_cast<ChannelScene*>(Scene::m_pScene);
			auto iter = cs->GetRoomMap().find(m_pUI->GetTitleName());
			if (iter != cs->GetRoomMap().end()) // �̹������Ѵٸ�.
			{
				::MessageBox(g_WindowHandle, L"�̹� �����ϴ� ���̸�", L"�� ��������", 0);
				return;
			}
			packet << cs->GetPlayer()->GetPlayerInfo().m_name; // host�̸�
			if (m_pUI->GetPsBuffer().size() > 0)
				packet << m_pUI->GetTitleName() << m_pUI->GetPsBuffer();
			else
				packet << m_pUI->GetTitleName();
			AsyncIO::GetInstance()->SendPacket(packet);
		}
		else
			::MessageBoxA(g_WindowHandle, "�� ���� �Է����� ����.", "�� ��������", 0);

	}
	int test = 10;
}

