#include "ClientServer.h"
#include "AsyncIO.h"
#include "Scene.h"
#include "LoginScene.h"
#include "ChannelScene.h"
#include "CreateAccountScene.h"
#include "GameScene.h"
#include "Player.h"
#include "PlayerManager.h"
#include "RoomUI.h"
#include "RoomMakeUI.h"
#include "CreateRoomUI.h"
#include "OmokGround.h"
#include "Stone.h"
ClientServer* ClientServer::m_pInstance = nullptr;
ClientServer* ClientServer::GetInstance()
{
	if (m_pInstance == nullptr)
		m_pInstance = new ClientServer();
	return m_pInstance;
}
void ClientServer::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		SAFE_DELETE(m_pInstance);
	}
}
bool ClientServer::Init()
{
	if (!AsyncIO::GetInstance()->InitClientNet())
		return false;
	if (!AsyncIO::GetInstance()->Connect())
		return false;

	return true;
}
bool ClientServer::Update()
{
	ProcessRecvPacket();
	return true;
}
bool ClientServer::Release()
{
	AsyncIO::GetInstance()->CloseClientNet();
	ReleaseInstance();
	return false;
}
void ClientServer::ProcessRecvPacket()
{
	auto iter = AsyncIO::GetInstance()->m_packetPool.begin();

	
	for (; iter != AsyncIO::GetInstance()->m_packetPool.end();)
	{
		Packet* packet = *iter;
		switch ((*iter)->GetProtocolID())
		{
			case static_cast<uInt16>(OmProtocol::SC_REQU_ACCOUNT) :
			{
				SC_REQU_ACCOUNT_PACKET account;	
				*packet >> account.ret;
				RecvCreateAccountPacket(account.ret);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_LOGIN) :
			{
				RecvLoginResult(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_PLAYER_LIST) :
			{
				RecvPlayerList(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_CREATE_ROOM) :
			{
				SC_REQU_CREATE_ROOM_PACKET room;
				*packet >> room.hostPlayerId;
				if (room.hostPlayerId == L"No")
				{
					::MessageBox(g_WindowHandle, L"방 가득 참", L"방 생성 실패", 0);
					break;
				}
				*packet >> room.roomTitle >> room.roomIndex;
				RecvCreateRoom(room);
			}break;

			case static_cast<uInt16>(OmProtocol::SC_REQU_ROOMLIST) :
			{
				SC_REQU_ROOMLIST_PACKET roomCount;
				*packet >> roomCount.roomCount;
				RecvRoomList(packet,roomCount.roomCount);
			}break;
			//SC_REQU_ROOM_ENTRY
			case static_cast<uInt16>(OmProtocol::SC_REQU_ROOM_ENTRY) :
			{
				
				RecvEnterUserInRoom(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_HOST_PLAYER_ROOM_EXIT):
			{
				// Enter유저가 존재하는데 Host가 나가는 경우.
				RecvHostPlayerExit(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_ENTER_PLAYER_ROOM_EXIT):
			{
				// Host유저가 존재하는데 Enter유저가 나가는 경우.
				RecvEnterPlayerExit(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_PLAYER_EXIT) :
			{
				
				RecvPlayerExit(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_PLAYER_READY) :
			{
				RecvEnterPlayerReady(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_PLAYER_START) :
			{
				RecvGameStart(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_PUT_STONE) :
			{
				RecvPutStone(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_FINISH) :
			{
				RecvFinishGame(packet);
			}break;
		}
		delete* iter;
		iter = AsyncIO::GetInstance()->m_packetPool.erase(iter);
	}
	
}
//SendPacket
void ClientServer::RecvCreateAccountPacket(uInt32 answer)
{
	if (answer == 0)
	{
		::MessageBox(g_WindowHandle, L"계정생성 성공!", L"계정 생성", 0);
		CreateAccountScene* pScene = static_cast<CreateAccountScene*>(Scene::m_pScene);
		LoginScene* loginScene = new LoginScene();
		Scene::m_pScene->ChangeScene(loginScene,ST_LOGIN,true,true,false);
		SAFE_DELETE(pScene);
	}
	else if (answer == 1)
	{
		::MessageBox(g_WindowHandle, L"중복 아이디 다시 시도.", L"계정 생성", 0);
	}
	else
	{
		::MessageBox(g_WindowHandle, L"에러.", L"계정 생성", 0);
	}
}

//BroadCast
void ClientServer::RecvLoginResult(Packet* packet)
{
	// 로그인 씬 && 계정생성 씬에 있는 유저는 받지않는다.
	if (m_isLoginScene)
		return;
	SC_REQU_LOGIN_PACKET answer;
	*packet >> answer.ret;
	
	*packet >> answer.id >> answer.totalPlayGameCount >> answer.winGameCount >> answer.loseGameCount >> answer.winRate;
	if (answer.ret == RV_OK)
	{
		
		Player* pPlayer = new Player(answer.id, answer.totalPlayGameCount, answer.winGameCount, answer.loseGameCount, answer.winRate); //Leak
		PlayerManager::GetInstance()->InsertPlayer(answer.id, pPlayer);
		if (m_isLogin)
		{
			ChannelScene* pCs;
			if (!m_isGameScene)
				pCs = static_cast<ChannelScene*>(Scene::m_pScene);
			else
				pCs = static_cast<ChannelScene*>(Scene::m_pPrevScene);
			// 새로들어온 유저를 넣기만 한다.
			//pCs->InsertPlayerInList(pPlayer); // OtherUser;

		}
		else // 자기자신 처리부분.
		{
			::MessageBox(g_WindowHandle, L"로그인 성공!", L"로그인", 0);
			LoginScene* pScene = static_cast<LoginScene*>(Scene::m_pScene);
			ChannelScene* pCs = new ChannelScene();

			pCs->SetPlayer(pPlayer); // 자기자신.
			Scene::m_pPrevScene = pCs;
			m_isLogin = true;
			Scene::m_pScene->ChangeScene(pCs, ST_CHANNEL, true,true,false);
			SAFE_DELETE(pScene);
		}
		return;
	}
	else if (answer.ret == RV_OVERLAP)
	{
		::MessageBox(g_WindowHandle, L"다시입력 or 계정생성필요", L"로그인", 0);
	}
	else if (answer.ret == RV_AlreadyLogin)
	{
		::MessageBox(g_WindowHandle, L"이미 로그인중인 계정", L"로그인", 0);
	}
	else
	{
		::MessageBox(g_WindowHandle, L"에러.", L"로그인", 0);
	}
	m_isLoginScene = true;
}
//SendPacket
void ClientServer::RecvPlayerList(Packet* packet)
{
	// 로그인한 유저가 받는 패킷.(자기자신의 정보는 제외 -> 로그인에서 처리)
	ChannelScene* pCs = static_cast<ChannelScene*>(Scene::m_pScene);
	uInt32 playerCount = 0;
	*packet >> playerCount;

	for (uInt32 player = 0; player < playerCount; ++player)
	{
		UserInfo info;
		*packet >> info.m_name >> info.m_totalPlayGameCount >> info.m_winGameCount >> info.m_loseGameCount >> info.m_winRate;
		Player* pPlayer = new Player(info.m_name,info.m_totalPlayGameCount,info.m_winGameCount,info.m_loseGameCount,info.m_winRate);

		// 다른유저들 삽입.
		PlayerManager::GetInstance()->InsertPlayer(info.m_name, pPlayer);
	}
}
//SendPacket
void ClientServer::RecvRoomList(Packet* packet, uInt32 roomCount)
{
	ChannelScene* pChannelScene = static_cast<ChannelScene*>(Scene::m_pScene);
	std::vector<std::wstring> titles;
	std::vector<std::wstring> hosts;
	std::vector<uInt32>		  roomIndices;
	std::vector<uInt32>		  playerCountInRoom;
	std::vector<uInt32>		  roomState;
	std::vector<std::wstring> enters;

	titles.resize(roomCount);
	hosts.resize(roomCount);
	roomIndices.resize(roomCount);
	playerCountInRoom.resize(roomCount);
	roomState.resize(roomCount);
	enters.resize(roomCount);

	// Room 하나씩 처리.
	for (uInt32 title = 0; title < roomCount; ++title)
	{
		titles[title].reserve(20);
		*packet >> titles[title];
	}
	for (uInt32 host = 0; host < roomCount; ++host)
	{
		hosts[host].reserve(20);
		*packet >> hosts[host];
		Player* pHostPlayer = PlayerManager::GetInstance()->FindPlayer(hosts[host]);
		GameScene* pScene = new GameScene();
		pScene->Init(); 
		pHostPlayer->SetGameScene(pScene);
		pScene->SetHostPlayer(pHostPlayer);
	}
	for (uInt32 index = 0; index < roomCount; ++index)
	{
		*packet >> roomIndices[index];
	}
	for (uInt32 count = 0; count < roomCount; ++count)
	{
		*packet >> playerCountInRoom[count];
	}
	for (uInt32 state = 0; state < roomCount; ++state)
	{
		*packet >> roomState[state];
	}
	for (uInt32 enter = 0; enter < roomCount; ++enter)
	{
		
		*packet >> enters[enter];
		Player* pHostPlayer = PlayerManager::GetInstance()->FindPlayer(hosts[enter]);
		GameScene* pScene = pHostPlayer->GetPlayerGameScene();
		pScene->SetTitleName(titles[enter]);
		if (enters[enter] == L"NoEnterPlayer")
		{
			continue;
		}

		Player* pEnterPlayer = PlayerManager::GetInstance()->FindPlayer(enters[enter]);

		pScene->SetEnterPlayer(pEnterPlayer);
		pEnterPlayer->SetGameScene(pScene);

		pHostPlayer->SetPlayerInGameScene(true);
		pEnterPlayer->SetPlayerInGameScene(true);
	}

	pChannelScene->CreateRoomUIs(roomCount, hosts, titles, roomIndices, playerCountInRoom, roomState);
}

//BroadCast
void ClientServer::RecvCreateRoom(SC_REQU_CREATE_ROOM_PACKET& answer)
{
	
	if (m_isLoginScene)
		return;
	if (m_isLogin == false)
		return;

	ChannelScene* pCs;

	if (!m_isGameScene)
		pCs = static_cast<ChannelScene*>(Scene::m_pScene);
	else
		pCs = static_cast<ChannelScene*>(Scene::m_pPrevScene);

	pCs->CreateRoom(answer.hostPlayerId, answer.roomTitle, answer.roomIndex,1,false);

	GameScene* pScene = new GameScene(); 
	pScene->Init();
	pScene->SetTitleName(answer.roomTitle);
	pScene->SetIsHost(true);
	if (answer.hostPlayerId == pCs->GetPlayer()->GetPlayerInfo().m_name)
	{
		//만약 방 생성 요청이 자기자신이라면.
		Player* pMyPlayer = pCs->GetPlayer();
		
		m_isGameScene = true;	
		pMyPlayer->SetPlayerInGameScene(true);
		pMyPlayer->SetGameScene(pScene);

		pScene->SetHostPlayer(pMyPlayer);
		pCs->GetRoomMakeUI()->GetCreateRoomUI()->ReverseHide();
		Scene::m_pScene->ChangeScene(pScene, ST_GAME, false,false,true);
		
	}
	else
	{
		// 다른 유저라면
		Player* pPlayer = PlayerManager::GetInstance()->FindPlayer(answer.hostPlayerId);
		if (pPlayer == nullptr)
			return;

		pScene->SetHostPlayer(pPlayer);
		pPlayer->SetPlayerInGameScene(true);
		pPlayer->SetGameScene(pScene);
	}

	
	//Scene::m_pPrevScene = cs;
}


//BroadCast
void ClientServer::RecvPlayerExit(Packet* packet)
{
	if (m_isLoginScene)
		return;

	SC_REQU_PLAYER_EXIT_PACKET exitPlayer;
	*packet >> exitPlayer.exitPlayerId;

	PlayerManager::GetInstance()->DeletePlayer(exitPlayer.exitPlayerId);

}


//BroadCast
void ClientServer::RecvEnterUserInRoom(Packet* packet)
{
	if (m_isLoginScene)
		return;

	SC_REQU_ROOM_ENTRY_PACKET entry;
	
	// roomTitle;
	*packet >> entry.entry >> entry.hostId >> entry.enterId >> entry.roomTitle;
	if (!entry.entry)
	{
		::MessageBox(g_WindowHandle, L"입장 불가", L"방 입장요청", 0);
		return;
	}
	//채널에 있는 유저가 방에 입장할 대
	ChannelScene* pCs;
	if (!m_isGameScene)
		pCs = static_cast<ChannelScene*>(Scene::m_pScene);
	else
		pCs = static_cast<ChannelScene*>(Scene::m_pPrevScene);


	Player* pHostPlayer = PlayerManager::GetInstance()->FindPlayer(entry.hostId);
	Player* pEnterPlayer = PlayerManager::GetInstance()->FindPlayer(entry.enterId);


	auto findRoom = pCs->GetRoomMap().find(entry.roomTitle);
	findRoom->second->EnterPlayerInRoom(2); // 방제 수정

	GameScene* pScene = pHostPlayer->GetPlayerGameScene();
	pScene->SetEnterPlayer(pEnterPlayer);
	pEnterPlayer->SetGameScene(pScene);
	
	// EnterUser가 자기자신일 경우.
	if (pCs->GetPlayer()->GetPlayerInfo().m_name == pEnterPlayer->GetPlayerInfo().m_name)
	{
		// 씬 전환
		pScene->SetIsHost(false);
		Scene::m_pScene->ChangeScene(pScene,ST_GAME,false,false,true);
		m_isGameScene = true;
	}

}
// SendPacket
void ClientServer::RecvEnterPlayerReady(Packet* packet)
{
	// EnterUser가 보낸 패킷
	GameScene* pScene = static_cast<GameScene*>(Scene::m_pScene);
	pScene->SetReady(!pScene->GetIsReady());
}
// Broadcast
void ClientServer::RecvGameStart(Packet* packet)
{
	if (m_isLoginScene)
		return;

	SC_REQU_PLAYER_START_PACKET start;
	*packet >> start.isStart;
	if (!start.isStart)
		return;

	*packet >> start.roomTitle >> start.hostId;

	ChannelScene* pCs = nullptr;
	if (!m_isGameScene)
		pCs = static_cast<ChannelScene*>(Scene::m_pScene);
	else
		pCs = static_cast<ChannelScene*>(Scene::m_pPrevScene);

	
	auto findRoom = pCs->GetRoomMap().find(start.roomTitle);

	// findRoom 텍스쳐 변경.
	findRoom->second->SetTexture(findRoom->second->GetPlayTexture());

	// 게임씬 가져와야함.
	GameScene* pScene = PlayerManager::GetInstance()->FindPlayer(start.hostId)->GetPlayerGameScene();
	pScene->ClearOmok();
	
	pScene->SetGameStart(true);

	Player* pHostPlayer = pScene->GetHostPlayer();
	Player* pEnterPlayer = pScene->GetEnterPlayer();

	pHostPlayer->SetPlayGame(true);
	pEnterPlayer->SetPlayGame(true);

	//SetTurn은 흑돌을 가진 플레이어를 기준. 현재는 호스트먼저.
	if (pHostPlayer->GetPlayerInfo().m_name == pCs->GetPlayer()->GetPlayerInfo().m_name)
	{
		
		::MessageBox(g_WindowHandle, L"게임 시작!", L"게임", 0);
		pScene->SetIsMyTurn(true);
	}
	else if (pEnterPlayer->GetPlayerInfo().m_name == pCs->GetPlayer()->GetPlayerInfo().m_name)
	{
		
		::MessageBox(g_WindowHandle, L"게임 시작!", L"게임", 0);
		pScene->SetIsMyTurn(false);
	}
}

// SendPacket
void ClientServer::RecvPutStone(Packet* packet)
{
	SC_REQU_PUT_STONE_PACKET put;
	*packet >> put.putY >> put.putX >> put.stoneType;

	GameScene* pScene = static_cast<GameScene*>(Scene::m_pScene);
	

	
	if(put.stoneType == static_cast<uInt32>(StoneType::BLACK))
		pScene->PutStone(StoneType::BLACK, put.putY, put.putX);
	else
		pScene->PutStone(StoneType::WHITE, put.putY, put.putX);

	

	if (pScene->GetIsMyTurn()) // 돌을 놓은게 자기자신이라면
	{
		pScene->SetIsMyTurn(false);
		
	}
	else // 아니라면 이제 상대방이 돌을 놓을 차례
	{
		pScene->SetIsMyTurn(true);
	}

}
//BroadCast
void ClientServer::RecvFinishGame(Packet* packet)
{
	SC_REQU_FINISH_PACKET finish;
	*packet >> finish.winPlayer >> finish.roomTitle;

	Player* pWinPlayer = PlayerManager::GetInstance()->FindPlayer(finish.winPlayer);

	GameScene* pScene = pWinPlayer->GetPlayerGameScene();

	Player* pHostPlayer = pScene->GetHostPlayer();
	Player* pEnterPlayer = pScene->GetEnterPlayer();

	pHostPlayer->SetPlayGame(false);
	pEnterPlayer->SetPlayGame(false);

	ChannelScene* pCs = nullptr;
	if (m_isGameScene)
		pCs = static_cast<ChannelScene*>(Scene::m_pPrevScene);
	else
		pCs = static_cast<ChannelScene*>(Scene::m_pScene);


	auto findRoom = pCs->GetRoomMap().find(finish.roomTitle);
	findRoom->second->SetTexture(findRoom->second->GetWaitTexture());

	pScene->SetGameStart(false);
	pScene->SetIsMyTurn(false);
	pScene->SetReady(false);
	pScene->GetOmokGround()->ReleaseStones();


	if (pHostPlayer->GetPlayerInfo().m_name == pCs->GetPlayer()->GetPlayerInfo().m_name ||  
		pEnterPlayer->GetPlayerInfo().m_name == pCs->GetPlayer()->GetPlayerInfo().m_name)
	{
		if (pCs->GetPlayer()->GetPlayerInfo().m_name == pWinPlayer->GetPlayerInfo().m_name)
		{
			::MessageBox(g_WindowHandle, L"승리!", L"게임 종료", 0);
			pCs->GetPlayer()->WinGame();
		}
		else
		{
			::MessageBox(g_WindowHandle, L"패배!", L"게임 종료", 0);
			pCs->GetPlayer()->LoseGame();
		}
		pCs->SetInfo(pCs->GetPlayer());
	}


}
//BroadCast
void ClientServer::RecvHostPlayerExit(Packet* packet)
{
	if (m_isLoginScene)
		return;

	SC_REQU_HOST_PLAYER_ROOM_EXIT_PACKET exitHost;

	*packet >> exitHost.titleName >> exitHost.exitHostId >> exitHost.emptyRoom;
	if (exitHost.emptyRoom)
		*packet >> exitHost.newHostID;


	ChannelScene* pScene = nullptr;
	if (!m_isGameScene)
		pScene = static_cast<ChannelScene*>(Scene::m_pScene);
	else
		pScene = static_cast<ChannelScene*>(Scene::m_pPrevScene);


	auto findRoom = pScene->GetRoomMap().find(exitHost.titleName);
	findRoom->second->SetTexture(findRoom->second->GetWaitTexture());
	Player* pPlayer = PlayerManager::GetInstance()->FindPlayer(exitHost.exitHostId);
	
	GameScene* pGameScene = pPlayer->GetPlayerGameScene();

	pGameScene->SetGameStart(false);
	pPlayer->SetGameScene(nullptr);
	pPlayer->SetPlayerInGameScene(false);
	if (exitHost.emptyRoom) 
	{
		//Enter유저는 남아있음.
		findRoom->second->ChangeHostName(exitHost.newHostID);
		findRoom->second->EnterPlayerInRoom(1);

		Player* pEnterPlayer = PlayerManager::GetInstance()->FindPlayer(exitHost.newHostID);
		pEnterPlayer->SetPlayerInGameScene(false);

		pGameScene->SetReady(false);
		
		pGameScene->SetIsMyTurn(false);
		pGameScene->SetHostPlayer(pEnterPlayer);
		pGameScene->SetEnterPlayer(nullptr);
		
		if (pScene->GetPlayer()->GetPlayerInfo().m_name == exitHost.exitHostId)
		{	
			// 초기화 x 현재 게임씬의 충돌체 제거.
			pGameScene->GetOmokGround()->ReleaseStones();
			
			Scene::m_pScene->ChangeScene(pScene, ST_CHANNEL, false, false,false);
		}

		if (pScene->GetPlayer()->GetPlayerInfo().m_name == exitHost.newHostID)
		{

			pGameScene->GetOmokGround()->ReleaseStones();
			pGameScene->SetIsHost(true);
			if (pEnterPlayer->GetPlayGame())
			{
				pEnterPlayer->WinGame();
				pEnterPlayer->SetPlayGame(false);
				pScene->SetInfo(pEnterPlayer);
			}
			
			::MessageBox(g_WindowHandle, L"호스트 변경!", L"호스트 퇴장", 0);
		}
	}
	else
	{
		// 둘 다 없음.
		pGameScene->GetOmokGround()->ReleaseStones();
		pGameScene->Release();
		SAFE_DELETE(pGameScene);

		pScene->GetUIObjList().erase(findRoom->second->GetObjNum());
		pScene->DeleteRoom(exitHost.titleName);	
		

		if (pScene->GetPlayer()->GetPlayerInfo().m_name == exitHost.exitHostId)
		{
			Scene::m_pScene->ChangeScene(pScene, ST_CHANNEL, false, false,false);
		}
	}

}
//BroadCast
void ClientServer::RecvEnterPlayerExit(Packet* packet)
{
	if (m_isLoginScene)
		return;

	SC_REQU_ENTER_PLAYER_ROOM_EXIT_PACKET exitEnter;

	*packet >> exitEnter.titleName >> exitEnter.exitEnterId;
	ChannelScene* pScene = nullptr;
	if (!m_isGameScene)
		pScene = static_cast<ChannelScene*>(Scene::m_pScene);
	else
		pScene = static_cast<ChannelScene*>(Scene::m_pPrevScene);
	auto findRoom = pScene->GetRoomMap().find(exitEnter.titleName);
	findRoom->second->EnterPlayerInRoom(1);
	findRoom->second->SetTexture(findRoom->second->GetWaitTexture());

	// Enter유저 제거.
	Player* pPlayer = PlayerManager::GetInstance()->FindPlayer(exitEnter.exitEnterId);

	GameScene* pGameScene = pPlayer->GetPlayerGameScene();
	Player* pHost = pGameScene->GetHostPlayer();
	if (pGameScene != nullptr)
	{
		pGameScene->SetGameStart(false);
		pGameScene->SetReady(false);
		pGameScene->SetEnterPlayer(nullptr);
	}
	pPlayer->SetGameScene(nullptr);
	pPlayer->SetPlayerInGameScene(false);
	//자신이 Enter유저
	if (pScene->GetPlayer()->GetPlayerInfo().m_name == pPlayer->GetPlayerInfo().m_name)
	{
		pPlayer->SetPlayGame(false);
		pGameScene->GetOmokGround()->ReleaseStones();
		Scene::m_pScene->ChangeScene(Scene::m_pPrevScene, ST_CHANNEL, false, false,false);
		m_isGameScene = false;
	}
	if (pScene->GetPlayer()->GetPlayerInfo().m_name == pHost->GetPlayerInfo().m_name)
	{
		if (pHost->GetPlayGame())
		{
			pHost->WinGame();
			pHost->SetPlayGame(false);
			pScene->SetInfo(pHost);
		}
		pGameScene->GetOmokGround()->ReleaseStones();
	}

}
