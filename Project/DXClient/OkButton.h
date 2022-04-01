#pragma once
#include "Object2D.h"
class RoomUI;
class CreateRoomUI;
class OkButton : public Object2D
{
	friend class	RoomUI;
public:
	OkButton(CreateRoomUI* pUI);
	~OkButton() override;
public:
	bool			Init() override;
	bool			Release() override;
public:
	void			HitSelect(BaseObject* pObj, DWORD state) override;
private:
	CreateRoomUI*	GetCreateRoomUIPtr() { return m_pUI; }
private:
	CreateRoomUI*	m_pUI;
};

