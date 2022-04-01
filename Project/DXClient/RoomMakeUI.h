#pragma once
#include "Object2D.h"
class ChannelScene;
class CreateRoomUI;
class Player;
class RoomMakeUI : public Object2D
{
public:
	RoomMakeUI(ChannelScene* pScene);
	~RoomMakeUI() override;
public:
	bool			Init() override;
	bool			Update() override;
	bool			Render() override;
	bool			Release() override;
	void			HitSelect(BaseObject* pObj, DWORD state) override;
public:
	CreateRoomUI*	GetCreateRoomUI() { return m_pCreateRoomUI; }
private:
	bool			CreateRoom();
private:
	ChannelScene*	m_pChannelScene;
	CreateRoomUI*	m_pCreateRoomUI;
};

