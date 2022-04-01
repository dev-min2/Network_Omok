#pragma once
#include "std.h"
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
#pragma comment(lib,"fmod_vc.lib")
class Sound 
{
public:
	Sound();
	~Sound();
public:
	void			Set(FMOD::System* pSystem, std::wstring name, int index);
	FMOD::Sound*	GetSound() { return m_pSound; }
	FMOD::Sound**	GetSoundAddress() { return &m_pSound; }
	bool			Init();
	bool			Update();
	bool			Render();
	bool			Release();
public:
	void			Play(bool loop = false);
	void			PlayEffect();
	void			Stop();
	void			Paused();
	void			VolumeUp(float volume = 0.1f);
	void			VolumeDown(float volume = 0.1f);

public:
	int					m_index;
	std::wstring		m_name;
	FMOD::System*		m_pSystem;
	FMOD::Sound*		m_pSound;
	FMOD::Channel*		m_pChannel;
	float				m_volume;
	std::wstring		m_msg;
	TCHAR				m_buffer[256];


};

