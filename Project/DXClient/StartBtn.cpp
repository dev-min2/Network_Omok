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
		//TODO 시작 패킷. (입장 유저있는지, 레디했는지를 체크)
		EnterPlayerCheck();
		
	}
}

bool StartBtn::EnterPlayerCheck()
{
	GameScene* pGameScene = static_cast<GameScene*>(Scene::m_pScene);

	if (pGameScene->GetGameStart())
		return false;
	if (pGameScene->GetIsHost()) // Host유저만.
	{
		// Todo Start Packet
		if (pGameScene->GetEnterPlayer() == nullptr)
		{
			::MessageBox(g_WindowHandle, L"입장한 유저가 없음", L"시작 불가능", 0);
			return false;
		}
		else if (!pGameScene->GetIsReady())
		{
			::MessageBox(g_WindowHandle, L"유저가 준비중이지 않음", L"시작 불가능", 0);
			return false;
		}


		Packet packet;
		packet.SetProtocolID(static_cast<uInt16>(OmProtocol::CS_REQU_PLAYER_START));

		packet << pGameScene->GetTitleName();

		AsyncIO::GetInstance()->SendPacket(packet);
	}
	return true;
}
