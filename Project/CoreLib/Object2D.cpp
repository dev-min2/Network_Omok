#include "Object2D.h"

Object2D::Object2D() : m_rtSource({ 0,0,0,0, }),m_rtDraw({0,0,g_clientRect.right,g_clientRect.bottom})
{
}

Object2D::~Object2D()
{
}

// 정점 6개 만든다.
void Object2D::Convert(Vector2 center, float width, float height, std::vector<SimpleVertex>& retList)
{
	std::vector<SimpleVertex> list(6);
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;

	// 1   2
	// 3
	list[0].vertexCoord.x = center.x - halfWidth;
	list[0].vertexCoord.y = center.y - halfHeight;
	list[1].vertexCoord.x = center.x + halfWidth;
	list[1].vertexCoord.y = center.y - halfWidth;
	list[2].vertexCoord.x = center.x - halfWidth;
	list[2].vertexCoord.y = center.y + halfWidth;

	//    5
	//4   6
	list[3].vertexCoord = list[2].vertexCoord;
	list[4].vertexCoord = list[1].vertexCoord;
	list[5].vertexCoord.x = center.x + halfWidth;
	list[5].vertexCoord.y = center.y + halfHeight;

	Convert(list, retList);
}

void Object2D::Convert(std::vector<SimpleVertex>& list, std::vector<SimpleVertex>& retList)
{
	// NDC
	retList.resize(list.size());
	for (int i = 0; i < list.size(); i++)
	{
		// 0 ~ 800 -> 0 ~ 1.
		retList[i].vertexCoord.x = list[i].vertexCoord.x / g_clientRect.right;
		retList[i].vertexCoord.y = list[i].vertexCoord.y / g_clientRect.bottom;
		
		// -1 ~ +1로 만드는 구간.
		retList[i].vertexCoord.x = retList[i].vertexCoord.x * 2.0f - 1.0f;
		retList[i].vertexCoord.y = -1.0f * (retList[i].vertexCoord.y * 2.0f - 1.0f);
	}

	// 91,1, 42, 56 => 0 ~ 1로 변환.
	// LoadTexture에서 호출한 GetDesc함수.
	if (m_rtSource.left == 0 && m_rtSource.right == 0 && m_rtSource.top == 0 && m_rtSource.bottom)
	{
		// 딱히 지정안해줬다면 화면 전체를 채운다.
		retList[0].texCoord.x = 0.0f; retList[0].texCoord.y = 0.0f; // v0
		retList[1].texCoord.x = 1.0f; retList[1].texCoord.y = 0.0f; // v1
		retList[2].texCoord.x = 0.0f; retList[2].texCoord.y = 1.0f;
		retList[3].texCoord = retList[2].texCoord;
		retList[4].texCoord = retList[1].texCoord;
		retList[5].texCoord.x = 1.0f; retList[5].texCoord.y = 1.0f;
	}
	else
	{
		float u = m_rtSource.left / (float)m_textureDesc.Width;
		float v = m_rtSource.top / (float)m_textureDesc.Height;
		float w = m_rtSource.right / (float)m_textureDesc.Width;
		float h = m_rtSource.bottom / (float)m_textureDesc.Height;

		retList[0].texCoord.x = u; retList[0].texCoord.y = v; // v0
		retList[1].texCoord.x = u + w; retList[1].texCoord.y = v; // v1
		retList[2].texCoord.x = u; retList[2].texCoord.y = v + h;
		retList[3].texCoord = retList[2].texCoord;
		retList[4].texCoord = retList[1].texCoord;
		retList[5].texCoord.x = u + w; retList[5].texCoord.y = v + h;
	}
}

void Object2D::ConvertIndex(Vector2 center, float width, float height, std::vector<SimpleVertex>& retList)
{
	std::vector<SimpleVertex> list(4);
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;

	// 1   2
	// 3
	list[0].vertexCoord.x = center.x - halfWidth;
	list[0].vertexCoord.y = center.y - halfHeight;
	list[1].vertexCoord.x = center.x + halfWidth;
	list[1].vertexCoord.y = center.y - halfHeight;
	list[2].vertexCoord.x = center.x - halfWidth;
	list[2].vertexCoord.y = center.y + halfHeight;
	list[3].vertexCoord.x = center.x + halfWidth;
	list[3].vertexCoord.y = center.y + halfHeight;

	ConvertIndex(list, retList);
}

void Object2D::ConvertIndex(std::vector<SimpleVertex>& list, std::vector<SimpleVertex>& retList)
{
	// NDC
	retList.resize(list.size());
	for (int i = 0; i < list.size(); i++)
	{
		// 0 ~ 800 -> 0 ~ 1.
		retList[i].vertexCoord.x = list[i].vertexCoord.x / g_clientRect.right;
		retList[i].vertexCoord.y = list[i].vertexCoord.y / g_clientRect.bottom;

		// -1 ~ +1로 만드는 구간.
		retList[i].vertexCoord.x = retList[i].vertexCoord.x * 2.0f - 1.0f;
		retList[i].vertexCoord.y = -1.0f * (retList[i].vertexCoord.y * 2.0f - 1.0f);
	}

	// 91,1, 42, 56 => 0 ~ 1로 변환.
	// LoadTexture에서 호출한 GetDesc함수.

	if (m_rtSource.left == 0 && m_rtSource.right == 0 && m_rtSource.top == 0 && m_rtSource.bottom == 0)
	{
		// 딱히 지정안해줬다면 화면 전체를 채운다.
		retList[0].texCoord.x = 0.0f; retList[0].texCoord.y = 0.0f; // v0
		retList[1].texCoord.x = 1.0f; retList[1].texCoord.y = 0.0f; // v1
		retList[2].texCoord.x = 0.0f; retList[2].texCoord.y = 1.0f;
		retList[3].texCoord.x = 1.0f; retList[3].texCoord.y = 1.0f;
	}
	else
	{
		float u = m_rtSource.left / (float)m_textureDesc.Width;
		float v = m_rtSource.top / (float)m_textureDesc.Height;
		float w = m_rtSource.right / (float)m_textureDesc.Width;
		float h = m_rtSource.bottom / (float)m_textureDesc.Height;

		retList[0].texCoord.x = u; retList[0].texCoord.y = v; // v0
		retList[1].texCoord.x = u + w; retList[1].texCoord.y = v; // v1
		retList[2].texCoord.x = u; retList[2].texCoord.y = v + h;
		retList[3].texCoord.x = u + w; retList[3].texCoord.y = v + h;
	}
}

void Object2D::SetRectSource(RECT rt)
{
	m_rtSource = rt;
}

void Object2D::SetRectDraw(RECT rt)
{
	m_rtDraw = rt;
	m_width = rt.right;
	m_height = rt.bottom;
}

void Object2D::AddPosition(Vector2 pos)
{
	m_pos += pos;
	
	m_collision = Rect(m_pos, m_width, m_height); // 사각형 충돌영역 다시 Set.

	ConvertIndex(m_pos, m_width, m_height, m_vertexList);
	m_pContext->UpdateSubresource(m_pVertexBuffer, 0, NULL, &m_vertexList.at(0), 0, 0);
}

void Object2D::SetPosition(Vector2 pos)
{
	m_pos = pos;
}

void Object2D::SetUpdatePosition(Vector2 pos)
{
	m_pos = pos;
	m_collision = Rect(m_pos, m_width, m_height); // 사각형 충돌영역 다시 Set.

	ConvertIndex(m_pos, m_width, m_height, m_vertexList);
	m_pContext->UpdateSubresource(m_pVertexBuffer, 0, NULL, &m_vertexList.at(0), 0, 0);
}

void Object2D::UpdateRectDraw(RECT rt)
{
	m_width = rt.right;
	m_height = rt.bottom;
}

bool Object2D::SetVertexData()
{
	ConvertIndex(m_pos, m_width, m_height, m_vertexList);
	return true;
}

bool Object2D::SetIndexData()
{
	// 인덱스는 정점버퍼의 순서
	// 0   1
	// 2
	//     4
	// 3   5
	//를
	// 0   1
	// 2   3
	//으로.
	m_indexList.push_back(0);
	m_indexList.push_back(1);
	m_indexList.push_back(2);
	m_indexList.push_back(2);
	m_indexList.push_back(1);
	m_indexList.push_back(3);

	return true;
}
