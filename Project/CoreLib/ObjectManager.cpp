#include "ObjectManager.h"
#include "Input.h"
#include "Scene.h"
void ObjectManager::AddCollisionExecute(BaseObject* owner, DWORD index,CollisionFunction func)
{
    // 중복체크


    owner->m_collisionID = m_excuteCollisionID++; // 아이디부여
    m_objectList[index].insert(std::make_pair(owner->m_collisionID, owner));
    m_collFuncionExe[index].insert(std::make_pair(owner->m_collisionID, func));
}

void ObjectManager::AddSelectExecute(BaseObject* owner, DWORD index, CollisionFunction func)
{
    owner->m_selectID = m_excuteSelectID++;
    m_selectList[index].insert(std::make_pair(owner->m_selectID, owner));
    m_selFuncionExe[index].insert(std::make_pair(owner->m_selectID, func)); // 충돌처리할 함수 등록.
}

void ObjectManager::DeleteCollisionExecute(BaseObject* owner, DWORD index)
{
    std::map<int, BaseObject*>::iterator objIter;
    objIter = m_objectList[index].find(owner->m_collisionID); // obj Find
    if (objIter != m_objectList[index].end())
        m_objectList[index].erase(objIter);

    CollFuncIterator collIter = m_collFuncionExe[index].find(owner->m_collisionID);
    if (collIter != m_collFuncionExe[index].end())
        m_collFuncionExe[index].erase(collIter);
}

void ObjectManager::DeleteSelectExecute(BaseObject* owner, DWORD index)
{
    std::map<int, BaseObject*>::iterator objIter;
    objIter = m_selectList[index].find(owner->m_selectID); // obj Find
    if (objIter != m_selectList[index].end())
        m_selectList[index].erase(objIter);

    SelFuncIterator selIter = m_selFuncionExe[index].find(owner->m_selectID);
    if (selIter != m_selFuncionExe[index].end())
        m_selFuncionExe[index].erase(selIter);
}


bool ObjectManager::Init()
{
    return true;
}

bool ObjectManager::Update()
{
    if (m_objectList[m_curSceneIndex].size() > 0)
    {
        // 객체 충돌 처리.
        for (auto src : m_objectList[m_curSceneIndex])
        {
            BaseObject* pObjSrc = src.second;
            if (pObjSrc->m_collisionType == CT_IGNORE) continue; // 충돌적용이 없는 오브젝트는 무시.
            DWORD state = CT_OVERLAP;
            for (auto dest : m_objectList[m_curSceneIndex]) // 들고있는 오브젝트들 중 충돌했는지 체크
            {
                BaseObject* pObjDest = dest.second;
                if (pObjSrc == pObjDest) continue; // 같은 객체는 무시.

                if (Collision::ToRect(pObjSrc->m_collision, pObjDest->m_collision)) // 충돌이라면.
                {
                    // 해당 객체의 충돌처리를 진행한다. (HitOverlap을 오버라이딩)
                    CollFuncIterator colIter = m_collFuncionExe[m_curSceneIndex].find(pObjSrc->m_collisionID);
                    if (colIter != m_collFuncionExe[m_curSceneIndex].end()) // 함수가 등록되었는지.
                    {
                        CollisionFunction call = colIter->second;
                        call(pObjDest, state);
                    }
                }
            }
        }
    }
    // mouse select
    if (m_selectList[m_curSceneIndex].size() > 0)
    {
        for (auto src : m_selectList[m_curSceneIndex])
        {
            BaseObject* pObjSrc = src.second;
            if (pObjSrc->m_selectType == CT_IGNORE) continue;
            DWORD state = CT_OVERLAP;

            if (Collision::RectToPoint(pObjSrc->m_collision, (float)g_mouse.x, (float)g_mouse.y))
            {
                DWORD LButtonState = Input::GetInstance()->GetMouseLButtonState();
                pObjSrc->m_selectState = SS_HOVER; // 충돌했다면 위에 올라간 상태.
                if (LButtonState == KS_DOWN) // 처음 누른 상태라면
                {
                    pObjSrc->m_selectState = SS_ACTIVE;
                }
                if (LButtonState == KS_HOLD) // 누르는중인 상태.
                {
                    pObjSrc->m_selectState = SS_FOCUS;
                }
                if (LButtonState == KS_UP) // 눌렀다 뗀 상태.
                {
                    pObjSrc->m_selectState = SS_SELECTED;
                }


                SelFuncIterator collIter = m_selFuncionExe[m_curSceneIndex].find(pObjSrc->m_selectID);
                if (collIter != m_selFuncionExe[m_curSceneIndex].end())
                {
                    SelectFunction call = collIter->second;
                    call(pObjSrc, state);
                }

            }
        }
    }
    if (m_changeScene)
    {
         // 다시 돌려놓는다.
        //ReleaseNextScene();
        if (m_isPrevDelete)
        {
            m_objectList[m_prevSceneIndex].clear();
            m_selectList[m_prevSceneIndex].clear();
        }
        if (m_isCurDelete)
        {
            m_objectList[m_curSceneIndex].clear();
            m_selectList[m_curSceneIndex].clear();
            Scene::m_pScene->ReSetCollision();
        }
        ChangeScene();
        if (m_doInit)
            Scene::m_pScene->Init();
        m_doInit = false;
    }
    return true;
}

bool ObjectManager::Release()
{
    for (DWORD del = 0; del < MAX_LIST; ++del)
    {
        m_objectList[del].clear();
        m_selectList[del].clear();
        m_selFuncionExe[del].clear();
        m_collFuncionExe[del].clear();
    }
    
    return true;
}
