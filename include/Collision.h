#pragma once
#include "Vector3.h"
enum CollisionResult
{
	CR_RECT_OUT = 0,
	CR_RECT_IN,
	CR_RECT_OVERLAP
};

struct Rect
{
	Vector2	m_min;
	Vector2	m_max;
	Vector2 m_middle;
	Vector2 m_size;
	bool operator == (const Rect& v)
	{
		if (fabs((m_min - v.m_min).Length()) < 0.0001f)
		{
			if (fabs((m_max - v.m_max).Length()) < 0.0001f)
			{
				return true;
			}
		}
		return false;
	}
	Rect() {};

	// 사각형 영역
	Rect(Vector2& vMin, Vector2& vMax)
	{
		this->m_min = vMin;
		this->m_max = vMax;
		m_middle = (m_max + m_min) / 2.0f;
		m_size.x = m_max.x - m_min.x;
		m_size.y = m_max.y - m_min.y;
	}
	Rect(Vector2& v, float w, float h)
	{
		// 수정.
		this->m_middle = v;
		this->m_min = m_middle - Vector2(w / 2.0f, h / 2.0f);
		this->m_max = m_middle + Vector2(w / 2.0f, h / 2.0f);
		this->m_size.x = w;
		this->m_size.y = h;

		/*this->m_min = v;
		this->m_max = m_min + Vector2(w, h);
		m_middle = (m_max + m_min) / 2.0f;
		this->m_size.x = w;
		this->m_size.y = h;*/
	}
};


class Collision
{
public:

	static bool					RectToPoint(Rect rt, int x, int y);
	static bool					RectToPoint(Rect rt, Vector2 v);
	static CollisionResult		RectToRect(Rect, Rect);
	static CollisionResult		ToRect(Rect rt1, Rect rt2);
	static Rect					UnionRect(Rect rt1, Rect rt2);
	static bool					IntersectRect(Rect rt1, Rect rt2, Rect* pRect);
};

