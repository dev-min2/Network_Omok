#pragma once
#include"Object2D.h"
#define MAX_INPUT_SIZE 3
// 이미 있는 계정이면(DB조회) X처리.
// Next버튼 처리 아직안했음.
// 실패 시 Message박스로 띄우기.
class SignUpUI : public Object2D
{
	enum InputType
	{
		IT_ID = 0,
		IT_PS,
		IT_AGE
	};
public:
	SignUpUI();
	~SignUpUI() override;
public:
	bool			Init() override;
	bool			Update() override;
	bool			Render() override;
	bool			Release() override;
	void			HitSelect(BaseObject* pObj, DWORD state) override;
private:
	bool			CheckIdArea(LONG clickX, LONG clickY);
	bool			CheckPsArea(LONG clickX, LONG clickY);
	bool			CheckAgeArea(LONG clickX, LONG clickY);
	bool			ClickNext(LONG clickX, LONG clickY);
	void			InputKeyCheck();
	void			PressId();
	void			PressPs();
	void			PressAge();
	void			WriteRender();
private:
	std::wstring	m_inputBuffer[MAX_INPUT_SIZE];
	bool			m_pressed[MAX_INPUT_SIZE];
	bool			m_tabKey[MAX_INPUT_SIZE];
	RECT			m_writeRect[MAX_INPUT_SIZE];
	RECT			m_inputRect[MAX_INPUT_SIZE];
	RECT			m_nextRect;
	bool			m_capsLock;


};

