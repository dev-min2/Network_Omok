
#include "User.h"
#include "SessionManager.h"
#include "PlayerManager.h"
#include "OmokProtocol.h"
#include "OdbcContents.h"
#include "Player.h"
#include "PlayerManager.h"
#include "Room.h"
#include "RoomManager.h"

User::User(ClientInfo& client) : Session(client)
{
	/*m_executeFunc.insert
	(
		std::make_pair(static_cast<uInt16>(PROTOCOL::CHAT_MSG), &User::ParsingChatMsg)
	);*/
	m_executeFunc.insert
	(
		std::make_pair(static_cast<uInt16>(OmProtocol::CS_REQU_ACCOUNT), &User::ParsingRequCreateAccount)
	);
	m_executeFunc.insert
	(
		std::make_pair(static_cast<uInt16>(OmProtocol::CS_REQU_LOGIN), &User::ParsingRequLogin)
	);
	m_executeFunc.insert
	(
		std::make_pair(static_cast<uInt16>(OmProtocol::CS_REQU_CREATE_ROOM), &User::ParsingRequCreateRoom)
	);
	m_executeFunc.insert
	(
		std::make_pair(static_cast<uInt16>(OmProtocol::CS_REQU_ROOM_ENTRY), &User::ParsingRequRoomEntry)
	);
	m_executeFunc.insert
	(
		std::make_pair(static_cast<uInt16>(OmProtocol::CS_REQU_PLAYER_READY), &User::ParsingRequPlayerReady)
	);
	m_executeFunc.insert
	(
		std::make_pair(static_cast<uInt16>(OmProtocol::CS_REQU_PLAYER_ROOM_EXIT), &User::ParsingRequPlayerRoomExit)
	);
	m_executeFunc.insert
	(
		std::make_pair(static_cast<uInt16>(OmProtocol::CS_REQU_PLAYER_START), &User::ParsingRequPlayerStart)
	);
	m_executeFunc.insert
	(
		std::make_pair(static_cast<uInt16>(OmProtocol::CS_REQU_PUT_STONE), &User::ParsingRequPutStone)
	);
}
User::~User()
{

}
void User::PacketParsing(Packet& packet)
{

	auto iter = m_executeFunc.find(packet.GetProtocolID());
	if (iter != m_executeFunc.end())
		iter->second(*this,packet);

}

void User::EventBeforeDelete()
{
	
	if (m_isLogin)
	{
		if (PlayerManager::GetInstance()->FindPlayer(m_id)->GetIsPlayGame())
		{
			// �����߿� �����ٸ�
			Player* pPlayer = PlayerManager::GetInstance()->FindPlayer(m_id);
			pPlayer->LoseGame();
			UserInfoDB* playerInfo = pPlayer->GetPlayerInfo();
			OdbcContents::GetInstance()->UpdateUserInfo(m_id, playerInfo->m_totalPlayCount, playerInfo->m_winGameCount, playerInfo->m_loseGameCount, playerInfo->m_winRate);

			Room* pRoom = RoomManager::GetInstance()->FindRoom(m_roomTitle);

			Player* pAnotherPlayer = pRoom->GetHostPlayer();
			if (pAnotherPlayer->GetPlayerInfo()->m_id == m_id) // ������ �ڱ��ڽ��̶��
				pAnotherPlayer = pRoom->GetEnterPlayer();


			pAnotherPlayer->WinGame();
			UserInfoDB* info = pAnotherPlayer->GetPlayerInfo();
			OdbcContents::GetInstance()->UpdateUserInfo(info->m_id, info->m_totalPlayCount, info->m_winGameCount, info->m_loseGameCount, info->m_winRate);

		}
		if (m_isInRoom)
		{
			CheckExitRoom(m_roomTitle,m_id,false);
		}
		

		OdbcContents::GetInstance()->UpdateLogout(m_id);
		PlayerManager::GetInstance()->DeletePlayer(m_id);
		SendPlayerExit();
	}
}

void User::BroadCastExceptMe(Packet& packet)
{
	uInt32 packetLen = packet.GetDefaultPacket().m_ph.m_len;
	DWORD sendBytes = 0, flag = 0;

	char* sendBuffer = m_sendBuffer.AllocateToBuffer(packetLen); // SendMsgCheck���� �����ʿ�.
	::CopyMemory(sendBuffer, (char*)&packet.GetDefaultPacket(), packetLen);

	m_overlappedSend.m_wsaBuf.buf = sendBuffer;
	m_overlappedSend.m_wsaBuf.len = packetLen;
	m_overlappedSend.m_ioType = IO_TYPE::IO_SEND;

	auto it = SessionManager::GetInstance()->GetSessions().GetBegin();
	for (; it != SessionManager::GetInstance()->GetSessions().GetEnd(); ++it)
	{
		if ((*it)->GetSocketValue() == this->GetSocketValue())
			continue;
		char* address = ::inet_ntoa((*it)->m_clientInfo.m_clientAddr.sin_addr);
		// WSASend�� Ƚ����ŭ IOCP���� �����尡 �����.
		// ���� Recv�� �ϳ��� Session�� �ѹ��� �ϳ��� ó��������
		// Send�� ��쿡�� �ѹ��� �ϳ��� Session���Ը� send�Ѵٴ� ������ �����Ƿ� lock�� ��ƾ��Ѵ�.(Dispatch Send����)
		int ret = ::WSASend((*it)->GetSocketValue(), &m_overlappedSend.m_wsaBuf, 1, &sendBytes, flag, &m_overlappedSend.m_overlapped, nullptr);
		if (ret == SOCKET_ERROR && ::WSAGetLastError() != ERROR_IO_PENDING)
		{
			int errorCode = ::WSAGetLastError();
			std::cout << "[Server] WSASend ���� | User | BroadCast() ���� : " << errorCode << std::endl;
			if (errorCode == 10054)
			{
				std::cout << "[Server] Ŭ���̾�Ʈ������ ������ ����" << std::endl;
			}
			// ������ ���´�. 
			(*it)->SetIsConnect(false);
			continue;
		}
		std::cout << "[Server] Ŭ���̾�Ʈ " << address << "���� " << packetLen << " ����Ʈ �۽�.." << std::endl;
		(*it)->GetIoSendCnt().fetch_add(1);
	}
	m_isSend = true;
}

void User::ParsingRequCreateAccount(Packet& packet)
{
	// ��X
	Packet sendPacket;
	sendPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_ACCOUNT));
	CS_REQU_ACCOUNT_PACKET accountInfo;

	packet >> accountInfo.id >> accountInfo.ps >> accountInfo.age;
	uInt32 ret = OdbcContents::GetInstance()->CreateAccount(accountInfo.id, accountInfo.ps, accountInfo.age);
	std::wcout << L"[Server] " << accountInfo.id << L" ���̵�� �������� ��û.\n";
	if (ret == 0)
	{
		std::wcout << L"[Server] " << accountInfo.id << L" �������� ����\n";
		sendPacket << (RV_OK);
	}
	else if (ret == 1)
	{
		std::wcout << L"[Server] " << accountInfo.id << L" �������� ����(�ߺ� ���̵�)\n";
		sendPacket << (RV_OVERLAP);
	}
	else
	{
		std::wcout << L"[Server] " << accountInfo.id << L" �������� ����(����)\n";
		sendPacket << (RV_ERROR);
	}

	SendPacket(sendPacket);
}

void User::ParsingRequLogin(Packet& packet)
{
	Packet sendPacket;
	sendPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_LOGIN));

	CS_REQU_LOGIN_PACKET login;
	packet >> login.id >> login.ps;
	int32 ret = OdbcContents::GetInstance()->RequestLogin(login.id, login.ps);

	if (ret == 0)
	{
		sendPacket << (RV_OK);
		m_isLogin = true;
		Player* pPlayer = new Player(this);
		pPlayer->SetPlayerID(login.id);
		
		// todo ���ϵ� �޾ƿ;� �Ѵ�.
		PlayerManager::GetInstance()->InsertPlayer(login.id, pPlayer);
		m_id = login.id;
		std::pair<bool,UserInfoDB> info = OdbcContents::GetInstance()->SetPlayerInfo(pPlayer);
		info.second.m_id = login.id;
		if (info.first)
		{
			sendPacket << login.id << info.second.m_totalPlayCount << info.second.m_winGameCount << info.second.m_loseGameCount << info.second.m_winRate;
			pPlayer->SetPlayerInfo(info.second);
		}
		std::wcout << L"[Server] " << login.id << L" �α���..\n";
		
		//SendPacket(sendPacket);
		BroadCast(sendPacket); // ��� �������� �������� �˷����Ѵ�.
		SendPlayerList();
  		SendRoomList();
		//SendPacket(roomListSendPacket);
		return;
	}
	else if (ret == 1)
		sendPacket << (RV_OVERLAP);
	else if (ret == 999)
		sendPacket << (RV_AlreadyLogin);
	else
		sendPacket << (RV_ERROR);

	SendPacket(sendPacket);
}

void User::ParsingRequCreateRoom(Packet& packet)
{
	Packet sendPacket;
	sendPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_CREATE_ROOM));

	if (RoomManager::GetInstance()->GetRoomMap().size() >= 10)
	{
		sendPacket << L"No";
		SendPacket(sendPacket);
		return;
	}
	CS_REQU_CREATE_ROOM_PACKET room;
	packet >> room.hostPlayerId >> room.roomTitle >> room.roomPs;

	m_isInRoom = true;
	m_roomTitle = room.roomTitle;

	Player* pPlayer = PlayerManager::GetInstance()->FindPlayer(room.hostPlayerId);
	pPlayer->SetPlayerScene(PS_GAMESCENE);

	// ���ȣ,
	uInt32 roomIndex = RoomManager::GetInstance()->GetRoomCount();
	Room* pRoom = new Room(roomIndex,room.roomTitle,room.roomPs);
	pRoom->SetHostPlayer(PlayerManager::GetInstance()->FindPlayer(room.hostPlayerId)); // ȣ��Ʈ Set
	RoomManager::GetInstance()->InsertRoom(room.roomTitle, pRoom);

	sendPacket << room.hostPlayerId << room.roomTitle << roomIndex;
	std::wcout << L"[Server] " << room.hostPlayerId << L"(��)�� ����� ��û\n";
	BroadCast(sendPacket);
}

//BroadCastó��. 
void User::ParsingRequRoomEntry(Packet& packet)
{
	CS_REQU_ROOM_ENTRY_PACKET entryRoom;
	packet >> entryRoom.roomTitle >> entryRoom.id;
	Packet sendPacket;
	sendPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_ROOM_ENTRY));
	if (!RoomManager::GetInstance()->EntryRoom(entryRoom.roomTitle, entryRoom.id)) // �濡 ����.
	{
		// TODO ���߿� ���� �Ұ��� ����
		sendPacket << 0;
		SendPacket(sendPacket);
		return;
	}
	m_isInRoom = true;
	m_roomTitle = entryRoom.roomTitle;
	Player* pEnterPlayer = PlayerManager::GetInstance()->FindPlayer(entryRoom.id);
	pEnterPlayer->SetPlayerScene(PS_GAMESCENE);
	

	Room* pRoom = RoomManager::GetInstance()->FindRoom(entryRoom.roomTitle);
	pRoom->SetEnterPlayer(pEnterPlayer); // Enter���� Set.
	pRoom->IncreasePlayer();
	Player* pHostPlayer = pRoom->GetHostPlayer();

	sendPacket << 1 << pHostPlayer->GetPlayerInfo()->m_id << entryRoom.id << entryRoom.roomTitle;

	BroadCast(sendPacket);
}

// SendPacket
void User::ParsingRequPlayerReady(Packet& packet)
{
	CS_REQU_PLAYER_READY_PACKET ready;
	packet >> ready.hostPlayerId;

	Packet sendPacket;
	sendPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_PLAYER_READY));

	User* pUser = PlayerManager::GetInstance()->FindPlayer(ready.hostPlayerId)->GetClient();
	std::wcout << L"[Server] " << ready.hostPlayerId << L"�� �濡�� �����û" << std::endl;
	SendPacketToOtherUser(sendPacket, pUser);
}

void User::ParsingRequPlayerRoomExit(Packet& packet)
{
	Packet SendPacket;
	CS_REQU_PLAYER_ROOM_EXIT_PACKET exitRoom;
	m_roomTitle.clear();/*
	m_isInRoom = false;
	
	packet >> exitRoom.exitRoomTitle >> exitRoom.exitPlayerId >> exitRoom.isHost;


	Player* pPlayer = PlayerManager::GetInstance()->FindPlayer(exitRoom.exitPlayerId);*/
	packet >> exitRoom.exitRoomTitle >> exitRoom.exitPlayerId;


	CheckExitRoom(exitRoom.exitRoomTitle, exitRoom.exitPlayerId,true);
}

void User::ParsingRequPlayerStart(Packet& packet)
{
	CS_REQU_PLAYER_START_PACKET start;
	packet >> start.roomTitle;

	Packet sendPacket;
	sendPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_PLAYER_START));

	Room* pRoom = RoomManager::GetInstance()->FindRoom(start.roomTitle);
	
	Player* pHostPlayer = pRoom->GetHostPlayer();
	Player* pEnterPlayer = pRoom->GetEnterPlayer();
	pHostPlayer->SetPlayGame(true);
	pEnterPlayer->SetPlayGame(true);
	
	

	if (pRoom->GetRoomState()) // �̹� �������̶��
	{
		sendPacket << 0;
	}
	else
	{
		
		sendPacket << 1 << start.roomTitle << pRoom->GetHostPlayer()->GetPlayerInfo()->m_id;
		pRoom->SetRoomState(true);
	}


	BroadCast(sendPacket);

}

void User::ParsingRequPutStone(Packet& packet)
{
	CS_REQU_PUT_STONE_PACKET put;
	packet >> put.putId >> put.anotherId >> put.roomTitle >>put.putY >> put.putX >> put.stoneType;

	Player* pPutPlayer = PlayerManager::GetInstance()->FindPlayer(put.putId);
	Player* pAnotherPlayer = PlayerManager::GetInstance()->FindPlayer(put.anotherId);

	Packet sendPacket;
	sendPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_PUT_STONE));



	pPutPlayer->PutStone(put.stoneType, put.putY, put.putX);
	pAnotherPlayer->PutStone(put.stoneType, put.putY, put.putX);

	sendPacket << put.putY << put.putX << put.stoneType;


	SendPacket(sendPacket);
	SendPacketToOtherUser(sendPacket, pAnotherPlayer->GetClient());

	uInt32 type = 0;
	if (put.stoneType == 0)
		type += 1;
	else
		type += 2;

	if (pPutPlayer->CheckFinish(type,put.putY,put.putX))
	{
		Packet finishPacket;
		finishPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_FINISH));
		pPutPlayer->ClearOmok();
		pAnotherPlayer->ClearOmok();

		pPutPlayer->WinGame();
		pAnotherPlayer->LoseGame();

		finishPacket << pPutPlayer->GetPlayerInfo()->m_id << put.roomTitle;
		Room* pRoom =RoomManager::GetInstance()->FindRoom(put.roomTitle);

		pRoom->SetRoomState(false);

		Player* pHostPlayer = pRoom->GetHostPlayer();
		Player* pEnterPlayer = pRoom->GetEnterPlayer();
		pHostPlayer->SetPlayGame(false);
		pEnterPlayer->SetPlayGame(false);


		BroadCast(finishPacket);


		UserInfoDB* pPutInfo = pPutPlayer->GetPlayerInfo();
		UserInfoDB* pAnotherInfo = pAnotherPlayer->GetPlayerInfo();
		OdbcContents::GetInstance()->UpdateUserInfo(pPutInfo->m_id, pPutInfo->m_totalPlayCount, pPutInfo->m_winGameCount, pPutInfo->m_loseGameCount, pPutInfo->m_winRate);
		OdbcContents::GetInstance()->UpdateUserInfo(pAnotherInfo->m_id, pAnotherInfo->m_totalPlayCount, pAnotherInfo->m_winGameCount, pAnotherInfo->m_loseGameCount, pAnotherInfo->m_winRate);
				/*
		SendPacket(finishPacket);
		SendPacketToOtherUser(finishPacket, pAnotherPlayer->GetClient());*/

	}

	
}

void User::SendRoomList()
{
	/// <summary>
	/// �� ��� ��û�κ�.
	/// </summary>
  	auto roomMap = RoomManager::GetInstance()->GetRoomMap();
	if (roomMap.size() <= 0)
	{
		return;
	}
	std::wcout << L"[Server] " << m_id << L" ���� �� ��� �۽�\n";

	Packet roomListSendPacket;
	roomListSendPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_ROOMLIST));
	uInt32 roomCount = roomMap.size();
	roomListSendPacket << (roomCount);
	uInt32 count = 0;

	std::vector<std::wstring> titles(roomCount);
	std::vector<std::wstring> hosts(roomCount);
	std::vector<uInt32> roomindices(roomCount);
	std::vector<uInt32> playerCountInRoom(roomCount);
	std::vector<uInt32> roomState(roomCount);
	std::vector<std::wstring> enters(roomCount);


	for (auto iter = roomMap.begin(); iter != roomMap.end(); ++iter)
	{
		Room* pRoom = iter->second;
		titles[count] += pRoom->GetTitleName();
		hosts[count] += pRoom->GetHostPlayer()->GetPlayerInfo()->m_id;
		roomindices[count] = pRoom->GetRoomIndex();
		playerCountInRoom[count] = pRoom->GetPlayerCount();
		roomState[count] = pRoom->GetRoomState();
		if (pRoom->GetEnterPlayer() != nullptr)
			enters[count] += iter->second->GetEnterPlayer()->GetPlayerInfo()->m_id;
		else
			enters[count] += L"NoEnterPlayer";
		++count;
	}

	for (uInt32 title = 0; title < roomCount; ++title)
		roomListSendPacket << titles[title];
	for (uInt32 host = 0; host < roomCount; ++host)
		roomListSendPacket << hosts[host];
	for (uInt32 index = 0; index < roomCount; ++index)
		roomListSendPacket << roomindices[index];
	for (uInt32 count = 0; count < roomCount; ++count)
		roomListSendPacket << playerCountInRoom[count];
	for (uInt32 state = 0; state < roomCount; ++state)
		roomListSendPacket << roomState[state];
	for (uInt32 enter = 0; enter < roomCount; ++enter)
		roomListSendPacket << enters[enter];


	SendPacket(roomListSendPacket);
}

// ������ �������� �̹� ������ �÷��̾� ����Ʈ�� ����Ѵ�.
void User::SendPlayerList()
{
	uInt32 count = PlayerManager::GetInstance()->GetPlayerCount();
	if (count <= 1)
		return;
	Packet sendPacket;
	sendPacket.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_PLAYER_LIST));

	auto map = PlayerManager::GetInstance()->GetPlayerMap();
	sendPacket << static_cast<uInt32>(map.size() - 1); // �ڱ��ڽ��� ����.
	for (auto iter = map.begin(); iter != map.end(); ++iter)
	{
		UserInfoDB* player = iter->second->GetPlayerInfo();
		
		if (m_id == player->m_id)
			continue;
		sendPacket << player->m_id << player->m_totalPlayCount << 
			player->m_winGameCount << player->m_loseGameCount << player->m_winRate;
	}
	SendPacket(sendPacket); 
}

void User::CheckExitRoom(std::wstring& roomTitle,std::wstring& exitPlayer,bool playerRequExit)
{
	// ���� �÷��̾ �濡 �ִ»��¿��� �����ٸ�.
	
	Room* pRoom = RoomManager::GetInstance()->FindRoom(roomTitle);
	if (pRoom != nullptr)
	{
		Packet packet;

		Player* pHostPlayer = pRoom->GetHostPlayer();
		Player* pEnterPlayer = pRoom->GetEnterPlayer();

		if(pHostPlayer != nullptr)
			pHostPlayer->SetPlayGame(false);
		if(pEnterPlayer != nullptr)
			pEnterPlayer->SetPlayGame(false);

		if (exitPlayer == pHostPlayer->GetPlayerInfo()->m_id)
		{
			// ���� ȣ��Ʈ������ üũ.
			packet.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_HOST_PLAYER_ROOM_EXIT));

			pRoom->DecreasePlayer();
			packet << roomTitle << exitPlayer; // roomTitle, enterId(�ٲ�� host �̸�)����
			if (pRoom->GetPlayerCount() <= 0)
			{
				packet << 0;
				pRoom->SetHostPlayer(nullptr);
				pRoom->SetEnterPlayer(nullptr);
				
				RoomManager::GetInstance()->DeleteRoom(roomTitle);
			}
			else
			{
				packet << 1 << pEnterPlayer->GetPlayerInfo()->m_id;
				pRoom->SetHostPlayer(pEnterPlayer); // Host�ٲٱ�
				pRoom->SetEnterPlayer(nullptr);
			}
			
			if (!playerRequExit)
				BroadCastExceptMe(packet); // ��ο��� ���� �������ٴ°� �˷�����.
			else
				BroadCast(packet);
		}
		else
		{
			//EnterUser Exit
			packet.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_ENTER_PLAYER_ROOM_EXIT));
			pRoom->SetEnterPlayer(nullptr);
			pRoom->DecreasePlayer();

			packet << roomTitle << exitPlayer;

			if (!playerRequExit)
				BroadCastExceptMe(packet); // ��ο��� ���� �������ٴ°� �˷�����.
			else
				BroadCast(packet);
		}
		m_isInRoom = false;
	}
}


void User::SendPlayerExit()
{
	Packet packet;
	packet.SetProtocolID(static_cast<uInt16>(OmProtocol::SC_REQU_PLAYER_EXIT));

	packet << m_id;
	BroadCast(packet);
}


