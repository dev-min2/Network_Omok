#pragma once
#include "std.h"
#define RINGBUFFER_SIZE 20480 // 20KB


class RingBuffer final
{
public:
	RingBuffer();
	~RingBuffer();
public:
	bool		Create();
	char*		GetCurrentReadPos() { return m_pReadPos; }
	char*		GetCurrentWritePos() { return m_pWritePos; }
	uInt32		GetAllocatedBytes() { return m_usedBufferSize; }
	char*		AllocateToBuffer(uInt32 allocateLen);
	char*		ReleaseBuffer(uInt32 releaseSize);
private:
	char*		m_pRingBuffer; // 실제 버퍼.
	char*		m_pWritePos; // 쓴 위치.
	char*		m_pReadPos; // 읽을 위치
	char*		m_pBeginPos; // 처음 위치.
	char*		m_pEndPos; // 끝 위치.
private:
	std::mutex	m_mutex;
	uInt32		m_bufferSize;
	uInt32		m_usedBufferSize; // 현재 사용된 크기.
	bool		m_isCreate;
private:
	RingBuffer(const RingBuffer& rhs) = delete;
	RingBuffer& operator=(const RingBuffer& rhs) = delete;
};

