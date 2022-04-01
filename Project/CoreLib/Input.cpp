#include "Input.h"

POINT g_mouse; // extern

Input* Input::m_instance = nullptr;

Input::Input() : m_pressed(false)
{

}

Input* Input::GetInstance()
{
    if (m_instance == nullptr)
        m_instance = new Input();
    return m_instance;
}

void Input::ReleaseInstance()
{
    if (m_instance != nullptr)
    {
        SAFE_DELETE(m_instance);
    }
}

bool Input::Init()
{
    ::ZeroMemory(&m_keyState, sizeof(DWORD) * 256);
    return true;
}

bool Input::Update()
{
    //ȭ����ǥ��
    ::GetCursorPos(&m_mouse);
    //Ŭ���̾�Ʈ ��ǥ��
    ::ScreenToClient(g_WindowHandle, &m_mouse);

    g_mouse = m_mouse;

    // ���콺��ư�� ����,�߰�,������
    for (int key = 0; key < MAX_KEY; ++key)
    {
        SHORT keyState = ::GetAsyncKeyState(key);

        if (keyState & 0x8000) // ���ȴٸ�
        {
            if (m_keyState[key] == KS_FREE)
            {
                m_pressed = true;
                m_getKey = m_keyState[key];
                m_keyState[key] = KS_DOWN;
            }
            else // ������ ���°� DOWN�ε� �������¶�� HOLD
            {
                m_keyState[key] = KS_HOLD;
            }
        }
        else
        {
            if (m_keyState[key] == KS_DOWN || m_keyState[key] == KS_HOLD)
            {
                m_keyState[key] = KS_UP;
            }
            else
            {
                m_pressed = false;
                m_getKey = -100;
                m_keyState[key] = KS_FREE;
            }
        }
    }

    m_mouseState[0] = m_keyState[VK_LBUTTON];
    m_mouseState[1] = m_keyState[VK_RBUTTON];
    m_mouseState[2] = m_keyState[VK_MBUTTON];
    return true;
}

void Input::RefreshKey(DWORD key)
{
    m_keyState[key] = KS_FREE;
}
bool Input::Render()
{
    return true;
}

bool Input::Release()
{
    this->ReleaseInstance();
    return true;
}

DWORD Input::GetKey(DWORD key)
{
    return m_keyState[key];
}


bool Input::GetPressedKey(DWORD key)
{
    if (m_keyState[key] == KS_DOWN)
        return true;
    return false;
}

bool Input::GetHoldKey(DWORD key)
{
    if (m_keyState[key] == KS_HOLD)
        return true;
    return false;
}

bool Input::PressKey()
{
    return m_pressed;
}
