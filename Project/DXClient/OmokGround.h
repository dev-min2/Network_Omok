#pragma once
#include "Object2D.h"
#include "Types.h"
class Stone;
class GameScene;
class Sound;
#define MAX_LIMIT 23.0f
#define MAX_HOVER_LIMIT 20.0f

class OmokGround : public Object2D
{
	friend class GameScene;
public:
	bool Init()   override;
	bool Update() override;
	bool Release() override;
	void HitSelect(BaseObject* pObj, DWORD state) override;
public:

	void ReleaseStones();
public:
	OmokGround(GameScene* pGameScene);
	~OmokGround() override;

private:
	bool CheckOmokGround(float clickX, float clickY);
	bool PutStone(float clickX,float clickY);
	void CheckHoverStone(float clickX, float clickY);
	bool CreateHoverStone();
private:
	//Stone*				m_pStone;
	Stone*				m_pHoverStone;
	Sound*				m_pPutStoneSound;
	GameScene*			m_pGameScene;
	std::vector<Stone*> m_stones;
	uInt32				m_putY;
	uInt32				m_putX;
};

