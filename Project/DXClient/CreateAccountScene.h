#pragma once
#include "Scene.h"
class Sound;
class Object2D;
class SignUpUI;
class CreateAccountScene : public Scene
{
public:
	CreateAccountScene();
	~CreateAccountScene() override;
public:
	bool Load(std::wstring file) override;
	bool Init() override;
	bool Update() override;
	bool Render() override;
	bool Release() override;
private:
	bool CreateBackgroundImage();
	bool CreateSignUpUI();

private:
	Sound*		m_pBackGroundMusic;
	Object2D*	m_pBackgroundImage;
	SignUpUI*	m_pSignupUI;
};

