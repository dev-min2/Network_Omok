#pragma once
#include "Object2D.h"
#include "Types.h"
#include <string>
#define ROOM_STATE 6
class OkButton;
class RoomUI : public Object2D
{
	enum RoomName
	{
		RN_ROOM_NUM = 0,
		RN_ROOM_LEVEL,
		RN_ROOM_STATE,
		RN_ROOM_TITLE,
		RN_ROOM_HOST,
		RN_ROOM_PLAYER_COUNT
	};
public:
	RoomUI(std::wstring& hostName,std::wstring& titleName, uInt32 roomIndex, uInt32 playerCountInRoom,bool isPlayGame);
	~RoomUI() override;
public:
	bool			Init() override;
	bool			Render() override;
	bool			Release() override;
public:
	void			HitSelect(BaseObject* pObj, DWORD state) override;
public:
	void			SetTitleName(std::wstring& name) { m_room[RN_ROOM_TITLE] = name; }
	std::wstring	GetTitleName() { return m_room[RN_ROOM_TITLE]; }
	DWORD			GetRoomIndex() { return m_roomIndex; }
	void			SetRoomIndex(DWORD index);
	void			RequestRoomEntry();
	bool			CanEnter();
	void			ChangeHostName(std::wstring& newHostId);
	void			EnterPlayerInRoom(uInt32 count);
public:
	Texture*		GetPlayTexture() { return m_pPlayTexture; }
	Texture*		GetWaitTexture() { return m_pWaitTexture; }

	void			SetPlayTexture(Texture* pTex) { m_pPlayTexture = pTex; }
	void			SetWaitTexture(Texture* pTex) { m_pWaitTexture = pTex; }
	void			SetObjNum(uInt32 num) { m_myObjNum = num; }
	uInt32			GetObjNum() { return m_myObjNum; }
private:
	void			WriteRender();
private:
	uInt32			m_myObjNum = 0;
	std::wstring	m_room[ROOM_STATE];
	DWORD			m_roomIndex; // ¹æ¹øÈ£
	RECT			m_roomRect[ROOM_STATE];
	bool			m_isPlayState;
	uInt32			m_playerCountInRoom;
	Texture*		m_pPlayTexture;
	Texture*		m_pWaitTexture;
};

