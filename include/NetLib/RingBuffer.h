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
	char*		m_pRingBuffer; // ���� ����.
	char*		m_pWritePos; // �� ��ġ.
	char*		m_pReadPos; // ���� ��ġ
	char*		m_pBeginPos; // ó�� ��ġ.
	char*		m_pEndPos; // �� ��ġ.
private:
	std::mutex	m_mutex;
	uInt32		m_bufferSize;
	uInt32		m_usedBufferSize; // ���� ���� ũ��.
	bool		m_isCreate;
private:
	RingBuffer(const RingBuffer& rhs) = delete;
	RingBuffer& operator=(const RingBuffer& rhs) = delete;
};

