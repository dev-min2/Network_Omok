#pragma once
#include "Object2D.h"
#define MAX_LIST 10
class Scene;
// ���� �浹ó����.
class ObjectManager
{
	friend class Scene;
	using CollisionFunction = std::function<void(BaseObject*, DWORD)>;
	using SelectFunction	= std::function<void(BaseObject*, DWORD)>;

	using CollFuncIterator  = std::map<int, CollisionFunction>::iterator;
	using SelFuncIterator	= std::map<int, SelectFunction>::iterator;
public:
	static ObjectManager&		GetInstance()
	{
		static ObjectManager ins;
		return ins;
	}
public:
	void						AddCollisionExecute(BaseObject* owner,DWORD index, CollisionFunction func);
	void						AddSelectExecute(BaseObject* owner, DWORD index, CollisionFunction func);
	void						DeleteCollisionExecute(BaseObject* owner,DWORD index);
	void						DeleteSelectExecute(BaseObject* owner, DWORD index);
	bool						Init();
	bool						Update();
	bool						Release();
	void						ChangeScene() { m_changeScene = !m_changeScene; m_isPrevDelete = false; m_isCurDelete = false; }
	void						SetSceneIndex(DWORD index) { m_prevSceneIndex = m_curSceneIndex;  m_curSceneIndex = index; }
	void						ReleaseSceneObj() { m_isPrevDelete = true; }
	void						ReleaseCurSceneObj() { m_isCurDelete = true; }
	void						DoSceneInit(bool value) { m_doInit = value; }
private:
	ObjectManager() : m_excuteCollisionID(0), m_excuteSelectID(0), m_changeScene(false){ }
public:
	~ObjectManager() = default;
private:
	DWORD								m_excuteCollisionID;
	DWORD								m_excuteSelectID;
	bool								m_changeScene;
	//Create�� �߰�.(Object�� �ִ°�)
	std::map<int, BaseObject*>			m_objectList[MAX_LIST];
	std::map<int, BaseObject*>			m_selectList[MAX_LIST];
private:
	// �ش� ������Ʈ�� �浹ó�� �Լ��� �ִ� ��.
	std::map<int, CollisionFunction>	m_collFuncionExe[MAX_LIST]; 
	std::map<int, SelectFunction>		m_selFuncionExe[MAX_LIST]; // ���콺�浹 üũ.
	DWORD								m_curSceneIndex = 0;
	DWORD								m_prevSceneIndex = 0;
	bool								m_isPrevDelete = false;
	bool								m_isCurDelete = false;
	bool								m_doInit = false;

};

