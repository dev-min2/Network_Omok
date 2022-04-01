#pragma once
#include "Object2D.h"
class GameDlg : public Object2D
{
public:
	bool Update() override;
	void HitSelect(BaseObject* pObj, DWORD state) override;
public:
	GameDlg();
	~GameDlg() override;
};

