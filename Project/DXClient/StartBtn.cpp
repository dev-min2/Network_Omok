#include "StartBtn.h"
#include "GameScene.h"
#include "AsyncIO.h"
#include "OmokProtocol.h"
StartBtn::StartBtn()
{
}

StartBtn::~StartBtn()
{
}

void StartBtn::HitSelect(BaseObject* pObj, DWORD state)
{
	if (m_selectState & SS_SELECTED)
	{
		//TODO ���� ��Ŷ. (���� �����ִ���, �����ߴ����� üũ)
		EnterPlayerCheck();
		
	}
}

bool StartBtn::EnterPlayerCheck()
{
	GameScene* pGameScene = static_cast<GameScene*>(Scene::m_pScene);

	if (pGameScene->GetGameStart())
		return false;
	if (pGameScene->GetIsHost()) // Host������.
	{
		// Todo Start Packet
		if (pGameScene->GetEnterPlayer() == nullptr)
		{
			::MessageBox(g_WindowHandle, L"������ ������ ����", L"���� �Ұ���", 0);
			return false;
		}
		else if (!pGameScene->GetIsReady())
		{
			::MessageBox(g_WindowHandle, L"������ �غ������� ����", L"���� �Ұ���", 0);
			return false;
		}


		Packet packet;
		packet.SetProtocolID(static_cast<uInt16>(OmProtocol::CS_REQU_PLAYER_START));

		packet << pGameScene->GetTitleName();

		AsyncIO::GetInstance()->SendPacket(packet);
	}
	return true;
}
