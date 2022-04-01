#include "Scene.h"
#include "Object2D.h"
#include "ObjectManager.h"
Scene*              Scene::m_pScene = nullptr;
Scene*              Scene::m_pPrevScene = nullptr;
std::vector<Scene*>	Scene::m_delScene;

bool Scene::Load(std::wstring saveFile)
{
    return true;
}

bool Scene::Init()
{
    m_delScene.reserve(10);
    m_pd3dDevice = g_pd3dDevice;
    m_pContext   = g_pContext;
    return true;
}

bool Scene::Update()
{

    for (auto iter : m_backObjList)
    {
        iter.second->Update();
    }
    for (auto iter : m_UIObjList)
    {
        iter.second->Update();
    }
    for (auto iter : m_ItemObjList)
    {
        iter.second->Update();
    }
    for (auto iter : m_NpcObjList)
    {
        iter.second->Update();
    }
    for (auto iter : m_MapObjList)
    {
        iter.second->Update();
    }
    return true;
}

bool Scene::Render()
{
    for (auto iter : m_backObjList)
    {
        iter.second->Render();
    }
    for (auto iter : m_UIObjList)
    {
        iter.second->Render();
    }
    for (auto iter : m_ItemObjList)
    {
        iter.second->Render();
    }
    for (auto iter : m_NpcObjList)
    {
        iter.second->Render();
    }
    for (auto iter : m_MapObjList)
    {
        iter.second->Render();
    }
    
    return true;
}

bool Scene::Release()
{
    return true;
}

void Scene::ReSetCollision()
{
}

bool Scene::SceneObjRelease()
{
    for (auto iter : m_UIObjList)
    {
        auto* obj = iter.second;
        obj->Release();
        SAFE_DELETE(obj);
    }
    for (auto iter : m_ItemObjList)
    {
        auto* obj = iter.second;
        obj->Release();
        SAFE_DELETE(obj);
    }
    for (auto iter : m_NpcObjList)
    {
        auto* obj = iter.second;
        obj->Release();
        SAFE_DELETE(obj);
    }
    for (auto iter : m_MapObjList)
    {
        auto* obj = iter.second;
        obj->Release();
        SAFE_DELETE(obj);
    }
    for (auto iter : m_backObjList)
    {
        auto* obj = iter.second;
        obj->Release();
        SAFE_DELETE(obj);
    }
    m_UIObjList.clear();
    m_backObjList.clear();
    m_MapObjList.clear();
    m_NpcObjList.clear();
    m_ItemObjList.clear();
    
    return true;
}

void Scene::ChangeScene(Scene* scene, DWORD index, bool delObject,bool doInit, bool delCurObject)
{
    //m_pScene->SceneObjRelease();

    if (delObject)
    {
        // 현재는 로그인,account,gameScene만 
        m_pScene->Release();
        m_pScene->SceneObjRelease();
        ObjectManager::GetInstance().ReleaseSceneObj();
    }
    if (delCurObject)
    {
        ObjectManager::GetInstance().ReleaseCurSceneObj();
    }
    ObjectManager::GetInstance().SetSceneIndex(index);
    ObjectManager::GetInstance().DoSceneInit(doInit);
    ObjectManager::GetInstance().m_changeScene = true;
    m_pScene = scene;
}

Scene::Scene() : m_objCount(0),m_pd3dDevice(nullptr),m_pContext(nullptr)
{
}

Scene::~Scene()
{
}
