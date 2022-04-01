#pragma once
#include "Scene.h"
class Sound;
class Object2D;
class LoginUI;
class LoginScene : public Scene
{
public:
	LoginScene();
	~LoginScene() override;
public:
	bool		Load(std::wstring file) override;
	bool		Init() override;
	bool		Update() override;
	bool		Render() override;
	bool		Release() override;
public:
	bool		CreateBackgroundImage();
	bool		CreateLoginUI();
private:
	Sound*		m_pBackgroundMusic;
	Object2D*	m_pBackgroundImage;
	LoginUI*	m_pLoginUI;

};

