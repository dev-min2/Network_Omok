#pragma once
#include "DXObject.h"
class Object2D : public DXObject
{
	// 화면좌표계를 NDC 변환(2D는 공간변환 x)
public:
	Object2D();
	virtual ~Object2D();
public:
	virtual void	Convert(Vector2 center, float width, float height, std::vector<SimpleVertex>& retList);
	virtual void	Convert(std::vector<SimpleVertex>& list, std::vector<SimpleVertex>& retList);

	virtual void	ConvertIndex(Vector2 center, float width, float height, std::vector<SimpleVertex>& retList);
	virtual void	ConvertIndex(std::vector<SimpleVertex>& list, std::vector<SimpleVertex>& retList);
public:
	virtual void	SetRectSource(RECT rt);
	virtual void	SetRectDraw(RECT rt);
	virtual void	AddPosition(Vector2 pos);
	virtual void	SetPosition(Vector2 pos);
	virtual void	SetUpdatePosition(Vector2 pos);
	virtual void	UpdateRectDraw(RECT rt);
public:
	virtual bool	SetVertexData() override;
	virtual bool	SetIndexData() override;
public:
	RECT			GetRectSoruce() { return m_rtSource; }

protected:
	RECT	m_rtSource;		// 캐릭터 하나의 전체 크기
	RECT	m_rtDraw;		// 캐릭터 하나의 크기
};

