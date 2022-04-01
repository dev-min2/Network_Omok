#include "RoomMakeUI.h"
#include "ChannelScene.h"
#include "CreateRoomUI.h"
RoomMakeUI::RoomMakeUI(ChannelScene* pScene) : m_pChannelScene(pScene), m_pCreateRoomUI(nullptr)
{
}

RoomMakeUI::~RoomMakeUI()
{
}

bool RoomMakeUI::Init()
{
	if (!CreateRoom())
		return false;
    return true;
}

bool RoomMakeUI::Update()
{
    return true;
}

bool RoomMakeUI::Render()
{
	DXObject::Render();
    return true;
}

bool RoomMakeUI::Release()
{
	DXObject::Release();
    return true;
}

void RoomMakeUI::HitSelect(BaseObject* pObj, DWORD state)
{
	if (m_selectState & SS_SELECTED)
	{
		m_pCreateRoomUI->ReverseHide();
	}
}

bool RoomMakeUI::CreateRoom()
{
	m_pCreateRoomUI = new CreateRoomUI();
	m_pCreateRoomUI->SetPosition(Vector2(g_clientRect.right / 2.0f, g_clientRect.bottom / 2.0f)); // 800 450
	m_pCreateRoomUI->Init();
	m_pCreateRoomUI->SetRectSource({ 0,0,352,232 });
	m_pCreateRoomUI->SetRectDraw({ 0,0,352,232 });
	m_pCreateRoomUI->SetCollision(CT_IGNORE, ST_OVERLAP, ST_CHANNEL);
	if (!m_pCreateRoomUI->Create(g_pd3dDevice, g_pContext,
		L"../../data/Shader/NoBlendVS.txt",
		L"../../data/Shader/NoBlendPS.txt",
		L"../../data/omok/Channel/CRoom_Back.png"
	))
	{
		return false;
	}
	m_pChannelScene->GetUIObjList().insert(std::make_pair(999, m_pCreateRoomUI));
	return true;
}
