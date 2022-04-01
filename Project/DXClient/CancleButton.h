#pragma once
#include "Object2D.h"
class CreateRoomUI;
class CancleButton : public Object2D
{
public:
	CancleButton(CreateRoomUI* ui);
	~CancleButton() override;
public:
	bool			Release();
	void			HitSelect(BaseObject* pObj, DWORD state) override;
private:
	CreateRoomUI*	m_pUI;
};

