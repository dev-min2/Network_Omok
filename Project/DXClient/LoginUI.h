#pragma once
#include "Object2D.h"
#include "DXWrite.h"
class LoginUI : public Object2D
{
public:
	LoginUI();
	~LoginUI() override;
public:
	bool			Init() override;
	bool			Update() override;
	bool			Release() override;
	bool			Render() override;
	void			HitSelect(BaseObject* pObj, DWORD state) override;
public:
	void			InputKeyCheck();
	void			InputPSKeyCheck();
	bool			CheckIdUIArea(LONG x, LONG y);
	bool			CheckPsUIArea(LONG x, LONG y);
	bool			CheckCreateAccountUIArea(LONG x, LONG y);
	bool			CheckLoginUIArea(LONG x, LONG y);
	void			PressId();
	void			PressPs();
private:
	std::wstring	m_idBuffer;
	std::wstring	m_psBuffer;
	std::wstring	m_psSecretBuffer;
	bool			m_idPressed;
	bool			m_psPressed;
	bool			m_capsLock;
	RECT			m_idRect; // 585,395
	RECT			m_psRect; // 585,449
	RECT			m_inputIdRect;
	RECT			m_inputPsRect;
	RECT			m_inputAccountRect;
	RECT			m_inputLoginRect;

};

