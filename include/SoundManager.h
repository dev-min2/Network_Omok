#pragma once
#include "std.h"
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
#pragma comment(lib,"fmod_vc.lib")
class Sound;
class SoundManager
{
public:
	static SoundManager* GetInstance();
	static void			 ReleaseInstance();
public:
	Sound*			Load(std::string fileName);
	Sound*			GetSound(std::wstring soundName);
	bool			Init();
	bool			Update();
	bool			Render();
	bool			Release();
public:
	~SoundManager();
private:
	SoundManager();
private:
	DWORD							m_index;
	FMOD::System*					m_pSystem;
	std::map<std::wstring, Sound*>	m_soundList;
	static	SoundManager*			m_pInstance;

};

