#include "RoomUI.h"
#include "OkButton.h"
#include "DXWrite.h"
#include "CreateRoomUI.h"
#include "Scene.h"
#include "ChannelScene.h"
#include "AsyncIO.h"
#include "RoomMakeUI.h"
#include "OmokProtocol.h"
#include "Player.h"
#include "GameScene.h"
#include "TextureManager.h"
#include <string>
RoomUI::RoomUI(std::wstring& hostName, std::wstring& titleName, uInt32 roomIndex, uInt32 playerCountInRoom, bool isPlayGame)
	: m_roomIndex(roomIndex),m_playerCountInRoom(playerCountInRoom),m_isPlayState(isPlayGame),m_pPlayTexture(nullptr),
	m_pWaitTexture(nullptr)
{
	m_pPlayTexture = TextureManager::GetInstance()->Load(L"../../data/Omok/Channel/RoomX.bmp");
	m_room[RN_ROOM_NUM].reserve(5);
	m_room[RN_ROOM_LEVEL].reserve(5);
	m_room[RN_ROOM_STATE].reserve(5);
	m_room[RN_ROOM_TITLE].reserve(25);
	m_room[RN_ROOM_HOST].reserve(12);
	m_room[RN_ROOM_PLAYER_COUNT].reserve(5);
	

	m_room[RN_ROOM_NUM] += std::to_wstring(roomIndex + 1);
	m_room[RN_ROOM_LEVEL] += L"자유";
	m_room[RN_ROOM_TITLE] += titleName;
	if(m_playerCountInRoom <= 1)
		m_room[RN_ROOM_PLAYER_COUNT] += L"1 / 2";
	else
		m_room[RN_ROOM_PLAYER_COUNT] += L"2 / 2";
	m_room[RN_ROOM_HOST] += hostName;

	
}

RoomUI::~RoomUI()
{
}

bool RoomUI::Init()
{

	// TITLE 260 160 (초기 위치)
	// PLAYERCOUNT 693 160 (초기위치)
	// Y값 증가량 31
	LONG posX = static_cast<LONG>(m_pos.x);
	LONG posY = static_cast<LONG>(m_pos.y - 5.0f);
	m_roomRect[RN_ROOM_NUM]			 =  { posX - 470, posY, g_clientRect.right,g_clientRect.bottom };
	m_roomRect[RN_ROOM_LEVEL]		 =	{ posX - 413, posY, g_clientRect.right,g_clientRect.bottom };
	m_roomRect[RN_ROOM_TITLE]		 =	{ posX - 260, posY, g_clientRect.right,g_clientRect.bottom };
	m_roomRect[RN_ROOM_HOST]		 =	{ posX + 185, posY, g_clientRect.right,g_clientRect.bottom };
	m_roomRect[RN_ROOM_PLAYER_COUNT] =	{ posX + 333, posY, g_clientRect.right,g_clientRect.bottom };
	return true;
}

bool RoomUI::Render()
{
	DXObject::Render();
	WriteRender();
	return false;
}

bool RoomUI::Release()
{
	DXObject::Release();
	return true;
}

void RoomUI::HitSelect(BaseObject* pObj, DWORD state)
{
	
	if (m_selectState & SS_SELECTED)
	{
		ChannelScene* pScene = static_cast<ChannelScene*>(Scene::m_pScene);
		if (!pScene->GetRoomMakeUI()->GetCreateRoomUI()->GetIsHide())
			return;
		RequestRoomEntry();
	}
}

void RoomUI::SetRoomIndex(DWORD index)
{
	// -1일 때 적용되는 것.
	m_room[RN_ROOM_NUM].clear();
	m_room[RN_ROOM_NUM] += std::to_wstring(index + 1);

	
	m_roomIndex = index;
	float roomLocation = 210.0f + (index * 40.0f);
	Vector2 pos = { m_pos.x,roomLocation };
	SetUpdatePosition(pos);
	LONG posX = static_cast<LONG>(pos.x);
	LONG posY = static_cast<LONG>(pos.y - 5.0f);


	m_roomRect[RN_ROOM_NUM] = { posX - 470, posY, g_clientRect.right,g_clientRect.bottom };
	m_roomRect[RN_ROOM_LEVEL] = { posX - 413, posY, g_clientRect.right,g_clientRect.bottom };
	m_roomRect[RN_ROOM_TITLE] = { posX - 260, posY, g_clientRect.right,g_clientRect.bottom };
	m_roomRect[RN_ROOM_HOST] = { posX + 185, posY, g_clientRect.right,g_clientRect.bottom };
	m_roomRect[RN_ROOM_PLAYER_COUNT] = { posX + 333, posY, g_clientRect.right,g_clientRect.bottom };
}

void RoomUI::RequestRoomEntry()
{
	
	ChannelScene* pCs = static_cast<ChannelScene*>(Scene::m_pScene);
	
	Packet packet;
	packet.SetProtocolID(static_cast<uInt16>(OmProtocol::CS_REQU_ROOM_ENTRY));
	packet << m_room[RN_ROOM_TITLE] << pCs->GetPlayer()->GetPlayerInfo().m_name;

	AsyncIO::GetInstance()->SendPacket(packet);
}

bool RoomUI::CanEnter()
{
	if (m_room[RN_ROOM_PLAYER_COUNT] == L"1 / 2")
		return true;
	return false;
}

void RoomUI::ChangeHostName(std::wstring& newHostId)
{
	m_room[RN_ROOM_HOST].clear();

	m_room[RN_ROOM_HOST] += newHostId;
}

void RoomUI::EnterPlayerInRoom(uInt32 count)
{
	m_room[RN_ROOM_PLAYER_COUNT].clear();
	if(count == 1)
		m_room[RN_ROOM_PLAYER_COUNT] += L"1 / 2";
	else
		m_room[RN_ROOM_PLAYER_COUNT] += L"2 / 2";
}

void RoomUI::WriteRender()
{
	g_wt->DrawUserFormat(m_room[RN_ROOM_NUM]   , m_roomRect[RN_ROOM_NUM], D2D1::ColorF(0, 0, 0, 1));
	g_wt->DrawUserFormat(m_room[RN_ROOM_LEVEL], m_roomRect[RN_ROOM_LEVEL], D2D1::ColorF(0, 0, 0, 1));
	g_wt->DrawUserFormat(m_room[RN_ROOM_TITLE], m_roomRect[RN_ROOM_TITLE], D2D1::ColorF(0, 0, 0, 1));
	g_wt->DrawUserFormat(m_room[RN_ROOM_HOST], m_roomRect[RN_ROOM_HOST], D2D1::ColorF(0, 0, 0, 1));
	g_wt->DrawUserFormat(m_room[RN_ROOM_PLAYER_COUNT], m_roomRect[RN_ROOM_PLAYER_COUNT], D2D1::ColorF(0, 0, 0, 1));
	
}
