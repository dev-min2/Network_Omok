#include "Stone.h"
#include "Scene.h"
Stone::Stone()
{
}

Stone::~Stone()
{
}

bool Stone::Init()
{
    return false;
}

Stone* Stone::CreateStone(StoneType type,float x, float y)
{
    Stone* stone = new Stone();
    stone->Init();
    stone->SetRectSource({ 0,0,50,37 });
    stone->SetRectDraw({ 0,0,50,37 });
    stone->SetPosition(Vector2(x,y));
    stone->SetCollision(CT_IGNORE, ST_IGNORE, ST_GAME);
    if (type == StoneType::BLACK)
    {
        if (!stone->Create(g_pd3dDevice, g_pContext,
            L"../../data/Shader/VertexShader.txt",
            L"../../data/Shader/PixelShader.txt",
            L"../../data/omok/Stone/blackStone.bmp",
            L"../../data/omok/Stone/mask2.bmp"
        ))
        {
            return nullptr;
        }
    }
    else
    {
        if (!stone->Create(g_pd3dDevice, g_pContext,
            L"../../data/Shader/VertexShader.txt",
            L"../../data/Shader/PixelShader.txt",
            L"../../data/omok/Stone/WhiteStone.bmp",
            L"../../data/omok/Stone/mask2.bmp"
        ))
        {
            return nullptr;
        }
    }
    return stone;
}
