#include "CancleButton.h"
#include "CreateRoomUI.h"
CancleButton::CancleButton(CreateRoomUI* ui) : m_pUI(ui)
{
}

CancleButton::~CancleButton()
{
}

bool CancleButton::Release()
{
	DXObject::Release();
	return true;
}

void CancleButton::HitSelect(BaseObject* pObj, DWORD state)
{
	if (m_selectState & SS_SELECTED)
	{
		//TODO 패킷처리등
		if(!m_pUI->GetIsHide())
			m_pUI->ReverseHide();
	}
	// TODO 패킷처리등
	int test = 10;
}
