#pragma once
#include "std.h"
class Object2D;

enum SceneType
{
	ST_LOGIN = 0,
	ST_ACCOUNT,
	ST_CHANNEL,
	ST_GAME
};

class Scene
{
public:
	//enum SceneType { ST_LOADING, ST_LOGIN, ST_LOBBY, ST_ZONE, ST_RESULT };
	ID3D11Device*			m_pd3dDevice;	// 디바이스 객체
	ID3D11DeviceContext*	m_pContext;// 다비이스 컨텍스트 객체
public:
	static Scene*						m_pScene; // 이 씬을 이용하여 모든 씬 컨트롤
	static Scene*						m_pPrevScene; // 이 씬을 이용하여 모든 씬 컨트롤
	static std::vector<Scene*>			m_delScene;
public:
	virtual bool	Load(std::wstring saveFile);
	virtual bool	Init();
	virtual bool	Update();
	virtual bool	Render();
	virtual bool	Release();
	virtual void    ReSetCollision();
public:
	inline std::map<DWORD, Object2D*>&	GetUIObjList() { return m_UIObjList; }
	inline std::map<DWORD, Object2D*>&	GetItemObjList() { return m_ItemObjList; }
	inline std::map<DWORD, Object2D*>&	GetNpcObjList() { return m_NpcObjList; }
	inline std::map<DWORD, Object2D*>&	GetMapObjList() { return m_MapObjList; }
	inline std::map<DWORD, Object2D*>&	GetBackObjList() { return m_backObjList; }
	inline DWORD						GetObjCount() { return m_objCount++; }
public:
	bool								SceneObjRelease();
	void								ChangeScene(Scene* scene,DWORD index,bool delObject, bool doInit,bool delCurObject);
public:
	Scene();
	virtual ~Scene();

protected:
	// 굳이 map일 필요가없다. list로 하는게 낫다.
	std::map<DWORD, Object2D*> m_UIObjList;
	std::map<DWORD, Object2D*> m_ItemObjList;
	std::map<DWORD, Object2D*> m_NpcObjList;
	std::map<DWORD, Object2D*> m_MapObjList;
	std::map<DWORD, Object2D*> m_backObjList;
	using m_mapIter = std::map<DWORD, Object2D*>::iterator;
	DWORD					   m_objCount;
};

