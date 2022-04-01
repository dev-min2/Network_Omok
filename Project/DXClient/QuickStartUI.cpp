#include "QuickStartUI.h"
#include "ChannelScene.h"
#include "RoomUI.h"
QuickStartUI::QuickStartUI()
{
}

QuickStartUI::~QuickStartUI()
{
}


void QuickStartUI::HitSelect(BaseObject* pObj, DWORD state)
{
	if (m_selectState & SS_SELECTED)
	{
		ChannelScene* pCs = static_cast<ChannelScene*>(Scene::m_pScene);

		auto iter = pCs->GetRoomMap().begin();
		for (; iter != pCs->GetRoomMap().end(); ++iter)
		{
			if (iter->second->CanEnter())
			{
				iter->second->RequestRoomEntry();
				return;
			}
		}
		::MessageBox(g_WindowHandle, L"���尡���� ���� �������� ����", L"���� ã��", 0);
	}
}
