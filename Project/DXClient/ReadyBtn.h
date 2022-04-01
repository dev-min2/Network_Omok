#pragma once
#include "Object2D.h"
class ReadyBtn : public Object2D
{
public:
	ReadyBtn();
	~ReadyBtn() override;
public:
	void		HitSelect(BaseObject* pObj, DWORD state) override;

private:
};

