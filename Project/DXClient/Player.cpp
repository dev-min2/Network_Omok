#include "Player.h"
#include "GameScene.h"
#include "PlayerManager.h"
Player::Player(std::wstring& name,int32 total, int32 win, int32 lose, float rate)
	: m_pPlayerInGameScene(nullptr), m_isGameScene(false), m_isPlayingGame(false)
{
	m_userInfo.m_name = name;
	m_userInfo.m_totalPlayGameCount = total;
	m_userInfo.m_winGameCount = win;
	m_userInfo.m_loseGameCount = lose;
	m_userInfo.m_winRate = rate;
}
Player::~Player()
{
}

void Player::WinGame()
{
	m_userInfo.m_totalPlayGameCount += 1;
	m_userInfo.m_winGameCount += 1;

	m_userInfo.m_winRate = CalculateRate(m_userInfo.m_winGameCount, m_userInfo.m_loseGameCount);
}

void Player::LoseGame()
{
	m_userInfo.m_totalPlayGameCount += 1;
	m_userInfo.m_loseGameCount += 1;
	m_userInfo.m_winRate = CalculateRate(m_userInfo.m_winGameCount, m_userInfo.m_loseGameCount);
}

float Player::CalculateRate(uInt32 winCount, uInt32 loseCount)
{
	float rate = (static_cast<float>(m_userInfo.m_winGameCount) / (m_userInfo.m_winGameCount + m_userInfo.m_loseGameCount)) * 100;
	return rate;
}