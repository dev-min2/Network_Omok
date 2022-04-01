#include "Collision.h"

bool Collision::RectToPoint(Rect rt, int x, int y)
{
    if (rt.m_min.x <= x && rt.m_max.x >= x &&
        rt.m_min.y <= y && rt.m_max.y >= y)
    {
        return true;
    }
    return false;
}

bool Collision::RectToPoint(Rect rt, Vector2 v)
{
    if (rt.m_min.x <= v.x && rt.m_max.x >= v.x &&
        rt.m_min.y <= v.y && rt.m_max.y >= v.y)
    {
        return true;
    }
    return false;
}

CollisionResult Collision::RectToRect(Rect rt1, Rect rt2)
{
    Rect rtInterctionRes;
    int ret = Collision::IntersectRect(rt1, rt2, &rtInterctionRes);
    if (ret <= 0)
    {
        return CR_RECT_OUT;
    }
    if (rtInterctionRes == rt2) return CR_RECT_OVERLAP;
    return CR_RECT_IN;
}

CollisionResult Collision::ToRect(Rect rt1, Rect rt2)
{
    // 거리 판정
    float fDistanceX;
    float fDistanceY;
    //서로 중점으로부터의 거리 구한다.
    fDistanceX = fabs(rt1.m_middle.x - rt2.m_middle.x);
    fDistanceY = fabs(rt1.m_middle.y - rt2.m_middle.y);

    // 
    float fToX = rt1.m_size.x / 2.0f + rt1.m_size.x / 2.0f;
    float fToY = rt1.m_size.y / 2.0f + rt1.m_size.y / 2.0f;
    if (fDistanceX < fToX && fDistanceY < fToY)
    {
        return CR_RECT_OVERLAP;
    }
    return CR_RECT_OUT; // 0임.
}

Rect Collision::UnionRect(Rect rt1, Rect rt2)
{
    //합집합.
    Rect rt;
    rt.m_min.x = rt1.m_min.x < rt2.m_min.x ? rt1.m_min.x : rt2.m_min.x;
    rt.m_min.y = rt1.m_min.y < rt2.m_min.y ? rt1.m_min.y : rt2.m_min.y;
    rt.m_max.x = rt1.m_max.x < rt2.m_max.x ? rt2.m_max.x : rt1.m_max.x;
    rt.m_max.y = rt1.m_max.y < rt2.m_max.y ? rt2.m_max.y : rt1.m_max.y;
    rt.m_size.x = rt.m_max.x - rt.m_min.x;
    rt.m_size.y = rt.m_max.y - rt.m_min.y;
    rt.m_middle = (rt.m_min + rt.m_max) / 2.0f;

    return rt;
}

bool Collision::IntersectRect(Rect rt1, Rect rt2, Rect* pRect)
{
    // 교집합
    Rect rt;
    Rect rtUnion = UnionRect(rt1, rt2);
    if (rtUnion.m_size.x <= (rt1.m_size.x + rt2.m_size.x) &&
        rtUnion.m_size.y <= (rt1.m_size.y + rt2.m_size.y))
    {
        if (pRect != nullptr)
        {
            //min은 더 큰쪽.
            pRect->m_min.x = rt1.m_min.x < rt2.m_min.x ? rt2.m_min.x : rt1.m_min.x;
            pRect->m_min.y = rt1.m_min.y < rt2.m_min.y ? rt2.m_min.y : rt1.m_min.y;

            // max는 더 작은쪽.
            //즉 
            // --------
            // 1    2-2----1
            // 1 ---2-2    1
            //      1------1
            // 일 때 저기 2인 부분을 구하는 것.
            pRect->m_max.x = rt1.m_max.x < rt2.m_max.x ? rt1.m_max.x : rt2.m_max.x;
            pRect->m_max.y = rt1.m_max.y < rt2.m_max.y ? rt1.m_max.y : rt2.m_max.y;

            pRect->m_size.x = pRect->m_max.x - pRect->m_min.x;
            pRect->m_size.y = pRect->m_max.y - pRect->m_min.y;
            pRect->m_middle = (pRect->m_max + pRect->m_min) / 2.0f;
        }
        return true;
    }
    return false;
}
