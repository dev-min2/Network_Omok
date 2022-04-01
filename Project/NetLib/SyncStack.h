#pragma once
#include "std.h"

template <typename T>
class SyncStack
{
public:
	SyncStack();
	~SyncStack();
public:
	void					Push(T item);
	void					PopSyncStack();
	bool					IsEmpty();
	T						GetLastSyncStack();
	size_t					GetSize();
private:
	std::stack<T>			m_syncStack;
	std::recursive_mutex	m_mutex;
};

template<typename T>
inline SyncStack<T>::SyncStack()
{

}

template<typename T>
inline SyncStack<T>::~SyncStack()
{
}

template<typename T>
inline void SyncStack<T>::Push(T item)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_syncStack.push(item);
}

template<typename T>
inline void SyncStack<T>::PopSyncStack()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_syncStack.pop();
}

template<typename T>
inline bool SyncStack<T>::IsEmpty()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_syncStack.empty();
}

template<typename T>
inline T SyncStack<T>::GetLastSyncStack()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (GetSize() > 0)
		return m_syncStack.top();
	return NULL;
}

template<typename T>
inline size_t SyncStack<T>::GetSize()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_syncStack.size();
}
