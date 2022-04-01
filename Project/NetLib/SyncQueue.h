#pragma once
#include "std.h"
template<typename T>
class SyncQueue
{
public:
	SyncQueue();
	~SyncQueue();
public:
	void						Push(T item);
	void						PopSyncQueue();
	bool						IsEmpty();
	T							GetFrontSyncQueue();
	size_t						GetSize();
private:
	std::queue<T>				m_syncQueue;
	std::recursive_mutex		m_mutex; // 상호배제 커널 객체
};

template<typename T>
inline SyncQueue<T>::SyncQueue()
{
}

template<typename T>
inline SyncQueue<T>::~SyncQueue()
{
}

template<typename T>
inline void SyncQueue<T>::Push(T item)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_syncQueue.push(item);
}

template<typename T>
inline void SyncQueue<T>::PopSyncQueue()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_syncQueue.pop();
}

template<typename T>
inline bool SyncQueue<T>::IsEmpty()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_syncQueue.empty();
}

template<typename T>
inline T SyncQueue<T>::GetFrontSyncQueue()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (GetSize() > 0)
		return m_syncQueue.front();
	return NULL;
}

template<typename T>
inline size_t SyncQueue<T>::GetSize()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	return m_syncQueue.size();
}
