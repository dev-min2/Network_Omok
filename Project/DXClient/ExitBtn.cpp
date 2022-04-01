#include "ExitBtn.h"
#include "GameScene.h"
#include "Player.h"
#include "AsyncIO.h"
#include "ChannelScene.h"
#include "GameScene.h"
#include "OmokProtocol.h"

ExitBtn::ExitBtn()
{
}

ExitBtn::~ExitBtn()
{
}

void ExitBtn::HitSelect(BaseObject* pObj, DWORD state)
{
	if (m_selectState & SS_SELECTED)
	{
		GameScene* pScene = static_cast<GameScene*>(Scene::m_pScene);
		if (pScene->GetGameStart())
			return;
		ChannelScene* pCs = static_cast<ChannelScene*>(Scene::m_pPrevScene);

		Packet packet;
		packet.SetProtocolID(static_cast<uInt16>(OmProtocol::CS_REQU_PLAYER_ROOM_EXIT));

		packet << pScene->GetTitleName() << pCs->GetPlayer()->GetPlayerInfo().m_name;

		AsyncIO::GetInstance()->SendPacket(packet);
	}
}
