#pragma once
#include "Object2D.h"
enum class StoneType
{
	BLACK = 0,
	WHITE
};
class Stone : public Object2D
{
public:
	Stone();
	~Stone() override;
public:
	bool	Init() override;
public:
	Stone*	CreateStone(StoneType type,float x, float y);
private:
	StoneType		m_stoneType;

};

