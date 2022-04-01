#pragma once
#include "Object2D.h"
class StartBtn : public Object2D
{
public:
	StartBtn();
	~StartBtn();
public:
	void		HitSelect(BaseObject* pObj, DWORD state) override;
private:
	bool		EnterPlayerCheck();
};

