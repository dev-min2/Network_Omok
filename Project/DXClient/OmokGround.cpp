#include "OmokGround.h"
#include "GameScene.h"
#include "Stone.h"
#include "SoundManager.h"
#include "Sound.h"
#include "Scene.h"
#include "AsyncIO.h"
#include "OmokProtocol.h"
#include "ChannelScene.h"
#include "Player.h"
#include "RoomUI.h"
bool OmokGround::Init()
{
	if (!CreateHoverStone())
		return false;
	return true;
}
bool OmokGround::Update()
{
	
    return true;
}

bool OmokGround::Release()
{	
	DXObject::Release();



	return false;
}




void OmokGround::HitSelect(BaseObject* pObj, DWORD state)
{
	if (!m_pGameScene->GetGameStart() || !m_pGameScene->GetIsMyTurn())
		return;
	float clickX = static_cast<float>(g_mouse.x);
	float clickY = static_cast<float>(g_mouse.y);
	if (m_selectState & SS_HOVER)
	{
		if (CheckOmokGround(clickX, clickY))
		{
			CheckHoverStone(clickX, clickY);
		}
		else
			m_pHoverStone->SetUpdatePosition(Vector2(-50.0f, -50.0f));
	}
	if (m_selectState & SS_SELECTED)
	{
		
		if(CheckOmokGround(clickX,clickY))
		{
			if (PutStone(clickX, clickY))
			{
				if (m_pGameScene->m_omokPair[m_putY][m_putX].first)
					return;
				//m_pGameScene->SetIsMyTurn(false);
				uInt32 stoneType;
				ChannelScene* pCs = static_cast<ChannelScene*>(Scene::m_pPrevScene);
				if (pCs->GetPlayer()->GetPlayerInfo().m_name == m_pGameScene->GetHostPlayer()->GetPlayerInfo().m_name)
					stoneType = static_cast<uInt32>(StoneType::BLACK);
				else
					stoneType = static_cast<uInt32>(StoneType::WHITE);

				
				
				Player* pPutPlayer = pCs->GetPlayer();
				Player* pAnotherPlayer = nullptr;

				if (m_pGameScene->GetHostPlayer()->GetPlayerInfo().m_name == pPutPlayer->GetPlayerInfo().m_name)
					pAnotherPlayer = m_pGameScene->GetEnterPlayer();
				else
					pAnotherPlayer = m_pGameScene->GetHostPlayer();

				Packet packet;
				packet.SetProtocolID(static_cast<uInt16>(OmProtocol::CS_REQU_PUT_STONE));

				packet << pPutPlayer->GetPlayerInfo().m_name << pAnotherPlayer->GetPlayerInfo().m_name << m_pGameScene->GetTitleName()
					<< m_putY << m_putX << stoneType;

				AsyncIO::GetInstance()->SendPacket(packet);
				m_putY = 0;
				m_putX = 0;
			}
			
			
		}
	}
}

OmokGround::OmokGround(GameScene* pScene) : m_pPutStoneSound(nullptr), m_pHoverStone(nullptr), m_pGameScene(pScene),
m_putY(0),m_putX(0)
{

	m_stones.reserve(100);
	int testa = 10;
}

OmokGround::~OmokGround()
{
}


bool OmokGround::CheckOmokGround(float clickX, float clickY)
{

	auto omok = m_pGameScene->m_omokPair;
	if (
		omok[0][0].second.x <= clickX && omok[0][0].second.y <= clickY &&
		omok[18][18].second.x >= clickX && omok[18][18].second.y >= clickY
		)
		return true;
	return false;
}

bool OmokGround::PutStone(float clickX,float clickY)
{
	auto omok = m_pGameScene->m_omokPair;
	for (uInt32 omokY = 0; omokY < MAX_Y; ++omokY)
	{
		for (uInt32 omokX = 0; omokX < MAX_X; ++omokX)
		{
			float diffPlusX = omok[omokY][omokX].second.x + MAX_LIMIT;
			float diffPlusY = omok[omokY][omokX].second.y + MAX_LIMIT;
			float diffMinusX = omok[omokY][omokX].second.x - MAX_LIMIT;
			float diffMinusY = omok[omokY][omokX].second.y - MAX_LIMIT;
			if (clickX >= diffMinusX && clickX <= diffPlusX)
			{
				if (clickY >= diffMinusY && clickY <= diffPlusY)
				{
					if (!m_pGameScene->m_omokPair[omokY][omokX].first)
					{

						//Stone* stone = new Stone();
						//if(m_pGameScene->GetIsHost())
						//	stone = stone->CreateStone(StoneType::BLACK,omok[omokY][omokX].second.x, omok[omokY][omokX].second.y);
						//else
						//	stone = stone->CreateStone(StoneType::WHITE, omok[omokY][omokX].second.x, omok[omokY][omokX].second.y);
						//m_stones.push_back(stone);
						//m_pGameScene->m_omokPair[omokY][omokX].first = true;
						m_putY = omokY;
						m_putX = omokX;
						return true;
					}
				}
			}
		}
	}
	return false;
}

void OmokGround::CheckHoverStone(float clickX, float clickY)
{
	auto omok = m_pGameScene->m_omokPair;
	for (uInt32 omokY = 0; omokY < MAX_Y; ++omokY)
	{
		for (uInt32 omokX = 0; omokX < MAX_X; ++omokX)
		{
			float diffPlusX = omok[omokY][omokX].second.x + MAX_HOVER_LIMIT;
			float diffPlusY = omok[omokY][omokX].second.y + MAX_HOVER_LIMIT;
			float diffMinusX = omok[omokY][omokX].second.x - MAX_HOVER_LIMIT;
			float diffMinusY = omok[omokY][omokX].second.y - MAX_HOVER_LIMIT;
			if (clickX >= diffMinusX && clickX <= diffPlusX)
			{
				if (clickY >= diffMinusY && clickY <= diffPlusY)
				{
					if (omok[omokY][omokX].first == false)
						m_pHoverStone->SetUpdatePosition(Vector2(omok[omokY][omokX].second.x, omok[omokY][omokX].second.y));
					else
						m_pHoverStone->SetUpdatePosition(Vector2(-50.0f, -50.0f));
					break;
				}
			}
		}
	}
}

bool OmokGround::CreateHoverStone()
{
	m_pHoverStone = new Stone();
	m_pHoverStone->Init();
	m_pHoverStone->SetRectSource({ 0,0,50,37 });
	m_pHoverStone->SetRectDraw({ 0,0,50,37 });
	//m_pHoverStone->SetPosition(Vector2(44.0f,39.0f));
	m_pHoverStone->SetCollision(CT_IGNORE, ST_IGNORE, ST_GAME);
	if (!m_pHoverStone->Create(g_pd3dDevice, g_pContext,
		L"../../data/Shader/VertexShader.txt",
		L"../../data/Shader/PixelShader.txt",
		L"../../data/omok/Stone/hoverStone.bmp",
		L"../../data/omok/Stone/WhiteStone-mask2.bmp"
	))
	{
		return false;
	}
	m_pGameScene->GetUIObjList().insert(std::make_pair(500, m_pHoverStone));
	m_pGameScene->GetObjCount(); // +1
	return true;
}

void OmokGround::ReleaseStones()
{
	// 돌 삭제. 게임끝나면 없애게끔.
	uInt32 len = m_stones.size();
	for (uInt32 stone = 0; stone < len; ++stone)
	{
		m_stones[stone]->Release();
		SAFE_DELETE(m_stones[stone]);
	}
	m_stones.clear();
	m_pHoverStone->SetUpdatePosition(Vector2(-50.0f, -50.0f));
}
