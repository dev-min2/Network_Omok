#pragma once
#include <list>
#include <mutex>
template <typename T>
class SyncList 
{
public:
	SyncList();
	~SyncList();
public:
	void								PushBack(const T& obj);
	void								PushFront(const T& obj);
	void								PopBack();
	void								PopFront();
	size_t								GetSize();
	typename std::list<T>::iterator		GetBegin();
	typename std::list<T>::iterator		GetEnd();
	typename std::list<T>::iterator		Erase(typename std::list<T>::iterator& iter);
private:
	std::recursive_mutex				m_mutex;
	std::list<T>						m_syncList;
};

template<typename T>
SyncList<T>::SyncList()
{

}

template<typename T>
SyncList<T>::~SyncList()
{
}

template<typename T>
void SyncList<T>::PushBack(const T& obj)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	{
		m_syncList.push_back(obj);
	}
}

template<typename T>
void SyncList<T>::PushFront(const T& obj)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	{
		m_syncList.push_front(obj);
	}
}

template<typename T>
void SyncList<T>::PopBack()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	{
		m_syncList.pop_back();
	}
}

template<typename T>
void SyncList<T>::PopFront()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	{
		m_syncList.pop_front();
	}
}


template<typename T>
size_t SyncList<T>::GetSize()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	{
		return m_syncList.size();
	}
}

template<typename T>
inline typename std::list<T>::iterator SyncList<T>::GetBegin()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	{
		return m_syncList.begin();
	}
}

template<typename T>
inline typename std::list<T>::iterator SyncList<T>::GetEnd()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	{
		return m_syncList.end();
	}
}

template<typename T>
inline typename std::list<T>::iterator SyncList<T>::Erase(typename std::list<T>::iterator& iter)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	{
		 return m_syncList.erase(iter);
	}
}
