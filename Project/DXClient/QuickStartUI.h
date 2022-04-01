#pragma once
#include "Object2D.h"
class QuickStartUI : public Object2D
{
public:
	QuickStartUI();
	~QuickStartUI() override;
public:
	void	HitSelect(BaseObject* pObj, DWORD state) override;
private:

};

