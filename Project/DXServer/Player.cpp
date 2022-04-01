#include "Player.h"
#include "OdbcContents.h"
#include "OmokProtocol.h"

Player::Player(User* user) : m_playerInfo(new UserInfoDB()), m_client(user), m_playerScene(PS_CHANNEL),m_isPlayGame(false)
{
	::ZeroMemory(m_omok, sizeof(m_omok));
}
Player::~Player()
{

}

void Player::SetPlayerScene(uInt32 sceneId)
{
	m_playerScene = sceneId;
}

void Player::PutStone(uInt32 stoneType, uInt32 putY, uInt32 putX)
{
	m_omok[putY][putX] = (stoneType + 1);
	// TODO Check;
}

bool Player::CheckFinish(uInt32 stoneType, uInt32 putY, uInt32 putX)
{
	// 끝났다면 true를 리턴.
	// 우상,우,우하,하순으로 8방향 체크
	if (CheckRULD(stoneType,putY,putX))
		return true;
	if (CheckRRLL(stoneType, putY, putX))
		return true;
	if (CheckRDLU(stoneType, putY, putX))
		return true;
	if (CheckDDUU(stoneType, putY, putX))
		return true;

	return false;
}

void Player::ClearOmok()
{
	::ZeroMemory(m_omok, sizeof(m_omok));
}


void Player::WinGame()
{
	m_playerInfo->m_totalPlayCount += 1;
	m_playerInfo->m_winGameCount += 1;

	m_playerInfo->m_winRate = CalculateRate(m_playerInfo->m_winGameCount, m_playerInfo->m_loseGameCount);

}
void Player::LoseGame()
{
	m_playerInfo->m_totalPlayCount += 1;
	m_playerInfo->m_loseGameCount += 1;

	m_playerInfo->m_winRate = CalculateRate(m_playerInfo->m_winGameCount, m_playerInfo->m_loseGameCount);
}

float Player::CalculateRate(uInt32 winCount, uInt32 loseCount)
{
	float rate = (static_cast<float>(m_playerInfo->m_winGameCount) / (m_playerInfo->m_winGameCount + m_playerInfo->m_loseGameCount)) * 100;
	return rate;
}

bool Player::CheckRULD(uInt32 stoneType, uInt32 putY, uInt32 putX)
{
	uInt32 originY = putY;
	uInt32 originX = putX;
	uInt32 finishCount = 0;
	uInt32 countOmok = 5;
	bool turn = false;
	while (true) // 우상,좌하체크
	{
		if (countOmok <= 0)
			break;
		if (!turn)
		{
			if (originY < 0 || originX >= 19)
			{
				originY = putY + 1;
				originX = putX - 1;
				turn = true;
				continue;
			}
			if (m_omok[originY][originX] == stoneType)
			{
				++finishCount;
				if (finishCount == 5)
					return true;
			}
			else
			{
				originY = putY + 1;
				originX = putX - 1;
				turn = true;
				continue;
			}
			--originY;
			++originX;
		}
		else
		{
			if (originY >= 19 || originX < 0)
				break;
			if (m_omok[originY][originX] == stoneType)
			{
				++finishCount;
				if (finishCount == 5)
					return true;
			}
			else
				break;
			++originY;
			--originX;
		}
		--countOmok;
	}
	return false;
}

bool Player::CheckRRLL(uInt32 stoneType, uInt32 putY, uInt32 putX)
{
	uInt32 originY = putY;
	uInt32 originX = putX;
	uInt32 finishCount = 0;
	uInt32 countOmok = 5;
	bool turn = false;
	while (true) // 우상,좌하체크
	{
		if (countOmok <= 0)
			break;
		if (!turn)
		{
			if (originX >= 19)
			{
				originX = putX - 1;
				turn = true;
				continue;
			}
			if (m_omok[originY][originX] == stoneType)
			{
				++finishCount;
				if (finishCount == 5)
					return true;
			}
			else
			{
				originX = putX - 1;
				turn = true;
				continue;
			}
			++originX;
		}
		else
		{
			if (originX < 0)
				break;
			if (m_omok[originY][originX] == stoneType)
			{
				++finishCount;
				if (finishCount == 5)
					return true;
			}
			else
				break;
			--originX;
		}
		--countOmok;
	}
	return false;
}

bool Player::CheckRDLU(uInt32 stoneType, uInt32 putY, uInt32 putX)
{
	uInt32 originY = putY;
	uInt32 originX = putX;
	uInt32 finishCount = 0;
	uInt32 countOmok = 5;
	bool turn = false;
	while (true) // 우상,좌하체크
	{
		if (countOmok <= 0)
			break;
		if (!turn)
		{
			if (originY >= 19 || originX >= 19)
			{
				originY = putY - 1;
				originX = putX - 1;
				turn = true;
				continue;
			}
			if (m_omok[originY][originX] == stoneType)
			{
				++finishCount;
				if (finishCount == 5)
					return true;
			}
			else
			{
				originY = putY - 1;
				originX = putX - 1;
				turn = true;
				continue;
			}
			++originY;
			++originX;
		}
		else
		{
			if (originY < 0 || originX < 0)
				break;
			if (m_omok[originY][originX] == stoneType)
			{
				++finishCount;
				if (finishCount == 5)
					return true;
			}
			else
				break;
			--originY;
			--originX;
		}
		--countOmok;
	}
	return false;
}

bool Player::CheckDDUU(uInt32 stoneType, uInt32 putY, uInt32 putX)
{
	uInt32 originY = putY;
	uInt32 originX = putX;
	uInt32 finishCount = 0;
	uInt32 countOmok = 5;
	bool turn = false;
	while (true) // 우상,좌하체크
	{
		if (countOmok <= 0)
			break;
		if (!turn)
		{
			if (originY >= 19)
			{
				originY = putY - 1;
				turn = true;
				continue;
			}
			if (m_omok[originY][originX] == stoneType)
			{
				++finishCount;
				if (finishCount == 5)
					return true;
			}
			else
			{
				originY = putY - 1;
				turn = true;
				continue;
			}
			++originY;
		}
		else
		{
			if (originY < 0)
				break;
			if (m_omok[originY][originX] == stoneType)
			{
				++finishCount;
				if (finishCount == 5)
					return true;
			}
			else
				break;
			--originY;
		}
		--countOmok;
	}
	return false;
}

void Player::SetPlayerID(std::wstring& name)
{
	m_playerInfo->m_id = name;
}

void Player::SetPlayerInfo(UserInfoDB& info)
{
	*m_playerInfo = info;
}
