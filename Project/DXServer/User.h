//#pragma once
#include "Session.h"
class User : public Session
{
	friend class Session;
public:
	User(ClientInfo& client);
	~User();
public:
	void PacketParsing(Packet& packet) override;
	void EventBeforeDelete() override;
	void BroadCastExceptMe(Packet& packet);
private:
	void ParsingRequCreateAccount(Packet& packet);
	void ParsingRequLogin(Packet& packet);
	void ParsingRequCreateRoom(Packet& packet);
	void ParsingRequRoomEntry(Packet& packet);
	void ParsingRequPlayerReady(Packet& packet);
	void ParsingRequPlayerRoomExit(Packet& packet);
	void ParsingRequPlayerStart(Packet& packet);
	void ParsingRequPutStone(Packet& packet);
private:
	void SendPlayerExit();
	void SendRoomList();
	void SendPlayerList();
	void CheckExitRoom(std::wstring& roomTitle,std::wstring& exitPlayer,bool playerRequExit);
private:
	// ����Լ��� �����ϱ� ���� this�� ������ �˾ƾ��Ѵ�.
	using CallbackFunc = std::function<void(User&,Packet& packet)>;
	std::unordered_map<uInt16, CallbackFunc> m_executeFunc;

public:
	bool			m_isLogin = false;
	bool			m_isInRoom = false;

	std::wstring	m_roomTitle;
	std::wstring    m_id;
};

