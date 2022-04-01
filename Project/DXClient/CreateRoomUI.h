#pragma once
#include "Object2D.h"
class OkButton;
class CancleButton;
class Player;
class CreateRoomUI : public Object2D
{
public:
	CreateRoomUI();
	~CreateRoomUI() override;
public:
	bool			Init() override;
	bool			Update() override;
	bool			Render() override;
	bool			Release() override;
	void			HitSelect(BaseObject* pObj, DWORD state) override;
public:
	void			ReverseHide();
	void			InputKeyCheck();
	bool			CheckTitleArea(LONG clickX, LONG clickY);
	bool			CheckRoomPsArea(LONG clickX, LONG clickY);
	void			PressTitle();
	void			PressPs();
	void			WriteRender();
	bool			CreateButton();
public:
	DWORD			GetRoomCountAndInc() { return m_roomCount++; }
	OkButton*		GetOkButton() { return m_pOkButton; }
	size_t			GetTitleLenght() { return m_titleBuffer.size(); }
	std::wstring&	GetTitleName() { return m_titleBuffer; }
	std::wstring&	GetPsBuffer() { return m_psBuffer; }
	bool			GetIsHide() { return m_hide; }
public:
	void			ClickHide() { m_hide = true; } // CancleButton에서 호출.
private:
	// 밑 두 오브젝트는 씬 리스트에 넣지않는다.
	OkButton*		m_pOkButton;
	CancleButton*	m_pCancleButton;
	DWORD			m_roomCount;

	bool			m_hide;
	bool			m_capsLock;
	bool			m_pressTitle;
	bool			m_pressPs;
	RECT			m_titleRect;
	RECT			m_roomPSRect;
	RECT			m_inputTitleRect;
	RECT			m_inputPsRect;
	std::wstring	m_titleBuffer;
	std::wstring	m_psBuffer;

};

