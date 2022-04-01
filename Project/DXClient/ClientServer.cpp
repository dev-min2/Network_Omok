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
					::MessageBox(g_WindowHandle, L"�� ���� ��", L"�� ���� ����", 0);
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
				// Enter������ �����ϴµ� Host�� ������ ���.
				RecvHostPlayerExit(packet);
			}break;
			case static_cast<uInt16>(OmProtocol::SC_REQU_ENTER_PLAYER_ROOM_EXIT):
			{
				// Host������ �����ϴµ� Enter������ ������ ���.
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
		::MessageBox(g_WindowHandle, L"�������� ����!", L"���� ����", 0);
		CreateAccountScene* pScene = static_cast<CreateAccountScene*>(Scene::m_pScene);
		LoginScene* loginScene = new LoginScene();
		Scene::m_pScene->ChangeScene(loginScene,ST_LOGIN,true,true,false);
		SAFE_DELETE(pScene);
	}
	else if (answer == 1)
	{
		::MessageBox(g_WindowHandle, L"�ߺ� ���̵� �ٽ� �õ�.", L"���� ����", 0);
	}
	else
	{
		::MessageBox(g_WindowHandle, L"����.", L"���� ����", 0);
	}
}

//BroadCast
void ClientServer::RecvLoginResult(Packet* packet)
{
	// �α��� �� && �������� ���� �ִ� ������ �����ʴ´�.
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
			// ���ε��� ������ �ֱ⸸ �Ѵ�.
			//pCs->InsertPlayerInList(pPlayer); // OtherUser;

		}
		else // �ڱ��ڽ� ó���κ�.
		{
			::MessageBox(g_WindowHandle, L"�α��� ����!", L"�α���", 0);
			LoginScene* pScene = static_cast<LoginScene*>(Scene::m_pScene);
			ChannelScene* pCs = new ChannelScene();

			pCs->SetPlayer(pPlayer); // �ڱ��ڽ�.
			Scene::m_pPrevScene = pCs;
			m_isLogin = true;
			Scene::m_pScene->ChangeScene(pCs, ST_CHANNEL, true,true,false);
			SAFE_DELETE(pScene);
		}
		return;
	}
	else if (answer.ret == RV_OVERLAP)
	{
		::MessageBox(g_WindowHandle, L"�ٽ��Է� or ���������ʿ�", L"�α���", 0);
	}
	else if (answer.ret == RV_AlreadyLogin)
	{
		::MessageBox(g_WindowHandle, L"�̹� �α������� ����", L"�α���", 0);
	}
	else
	{
		::MessageBox(g_WindowHandle, L"����.", L"�α���", 0);
	}
	m_isLoginScene = true;
}
//SendPacket
void ClientServer::RecvPlayerList(Packet* packet)
{
	// �α����� ������ �޴� ��Ŷ.(�ڱ��ڽ��� ������ ���� -> �α��ο��� ó��)
	ChannelScene* pCs = static_cast<ChannelScene*>(Scene::m_pScene);
	uInt32 playerCount = 0;
	*packet >> playerCount;

	for (uInt32 player = 0; player < playerCount; ++player)
	{
		UserInfo info;
		*packet >> info.m_name >> info.m_totalPlayGameCount >> info.m_winGameCount >> info.m_loseGameCount >> info.m_winRate;
		Player* pPlayer = new Player(info.m_name,info.m_totalPlayGameCount,info.m_winGameCount,info.m_loseGameCount,info.m_winRate);

		// �ٸ������� ����.
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

	// Room �ϳ��� ó��.
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
		//���� �� ���� ��û�� �ڱ��ڽ��̶��.
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
		// �ٸ� �������
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
		::MessageBox(g_WindowHandle, L"���� �Ұ�", L"�� �����û", 0);
		return;
	}
	//ä�ο� �ִ� ������ �濡 ������ ��
	ChannelScene* pCs;
	if (!m_isGameScene)
		pCs = static_cast<ChannelScene*>(Scene::m_pScene);
	else
		pCs = static_cast<ChannelScene*>(Scene::m_pPrevScene);


	Player* pHostPlayer = PlayerManager::GetInstance()->FindPlayer(entry.hostId);
	Player* pEnterPlayer = PlayerManager::GetInstance()->FindPlayer(entry.enterId);


	auto findRoom = pCs->GetRoomMap().find(entry.roomTitle);
	findRoom->second->EnterPlayerInRoom(2); // ���� ����

	GameScene* pScene = pHostPlayer->GetPlayerGameScene();
	pScene->SetEnterPlayer(pEnterPlayer);
	pEnterPlayer->SetGameScene(pScene);
	
	// EnterUser�� �ڱ��ڽ��� ���.
	if (pCs->GetPlayer()->GetPlayerInfo().m_name == pEnterPlayer->GetPlayerInfo().m_name)
	{
		// �� ��ȯ
		pScene->SetIsHost(false);
		Scene::m_pScene->ChangeScene(pScene,ST_GAME,false,false,true);
		m_isGameScene = true;
	}

}
// SendPacket
void ClientServer::RecvEnterPlayerReady(Packet* packet)
{
	// EnterUser�� ���� ��Ŷ
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

	// findRoom �ؽ��� ����.
	findRoom->second->SetTexture(findRoom->second->GetPlayTexture());

	// ���Ӿ� �����;���.
	GameScene* pScene = PlayerManager::GetInstance()->FindPlayer(start.hostId)->GetPlayerGameScene();
	pScene->ClearOmok();
	
	pScene->SetGameStart(true);

	Player* pHostPlayer = pScene->GetHostPlayer();
	Player* pEnterPlayer = pScene->GetEnterPlayer();

	pHostPlayer->SetPlayGame(true);
	pEnterPlayer->SetPlayGame(true);

	//SetTurn�� �浹�� ���� �÷��̾ ����. ����� ȣ��Ʈ����.
	if (pHostPlayer->GetPlayerInfo().m_name == pCs->GetPlayer()->GetPlayerInfo().m_name)
	{
		
		::MessageBox(g_WindowHandle, L"���� ����!", L"����", 0);
		pScene->SetIsMyTurn(true);
	}
	else if (pEnterPlayer->GetPlayerInfo().m_name == pCs->GetPlayer()->GetPlayerInfo().m_name)
	{
		
		::MessageBox(g_WindowHandle, L"���� ����!", L"����", 0);
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

	

	if (pScene->GetIsMyTurn()) // ���� ������ �ڱ��ڽ��̶��
	{
		pScene->SetIsMyTurn(false);
		
	}
	else // �ƴ϶�� ���� ������ ���� ���� ����
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
			::MessageBox(g_WindowHandle, L"�¸�!", L"���� ����", 0);
			pCs->GetPlayer()->WinGame();
		}
		else
		{
			::MessageBox(g_WindowHandle, L"�й�!", L"���� ����", 0);
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
		//Enter������ ��������.
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
			// �ʱ�ȭ x ���� ���Ӿ��� �浹ü ����.
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
			
			::MessageBox(g_WindowHandle, L"ȣ��Ʈ ����!", L"ȣ��Ʈ ����", 0);
		}
	}
	else
	{
		// �� �� ����.
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

	// Enter���� ����.
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
	//�ڽ��� Enter����
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
