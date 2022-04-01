#include "ReadyBtn.h"
#include "GameScene.h"
#include "AsyncIO.h"
#include "OmokProtocol.h"
#include "Player.h"
#include "ChannelScene.h"
ReadyBtn::ReadyBtn()
{
}

ReadyBtn::~ReadyBtn()
{
}

void ReadyBtn::HitSelect(BaseObject* pObj, DWORD state)
{
	if (m_selectState & SS_SELECTED)
	{
		
		GameScene* pGameScene = static_cast<GameScene*>(Scene::m_pScene);

		if (pGameScene->GetGameStart())
			return;

		if (!pGameScene->GetIsHost()) // Enter유저일때만
		{
			// Todo Ready Packet.
			pGameScene->SetReady(!pGameScene->GetIsReady());
			
			Packet packet;
			packet.SetProtocolID(static_cast<uInt16>(OmProtocol::CS_REQU_PLAYER_READY));

			packet << pGameScene->GetHostPlayer()->GetPlayerInfo().m_name;

			AsyncIO::GetInstance()->SendPacket(packet);
			

		}

	}
}
