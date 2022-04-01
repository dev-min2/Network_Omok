#pragma once
#include "Object2D.h"
class ExitBtn : public Object2D
{
public:
	ExitBtn();
	~ExitBtn() override;
public:
	void		HitSelect(BaseObject* pObj, DWORD state) override;

private:
};

