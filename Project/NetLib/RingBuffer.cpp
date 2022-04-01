#include "RingBuffer.h"

RingBuffer::RingBuffer() : m_pBeginPos(nullptr),m_pEndPos(nullptr),m_pReadPos(nullptr),m_pWritePos(nullptr),
m_bufferSize(RINGBUFFER_SIZE), m_usedBufferSize(0), m_pRingBuffer(nullptr), m_isCreate(false)
{

}

RingBuffer::~RingBuffer()
{
    m_isCreate = false;
    if (m_pRingBuffer != nullptr)
        SAFE_ARR_DELETE(m_pRingBuffer);
}

bool RingBuffer::Create()
{
    if (m_pRingBuffer != nullptr)
        SAFE_ARR_DELETE(m_pRingBuffer);

    m_pRingBuffer = new char[RINGBUFFER_SIZE];
    if (m_pRingBuffer == nullptr)
    {
        std::cout << "������ �޸� �Ҵ� ����.. RingBuffer | Create" << std::endl;
        return false;
    }
    ::ZeroMemory(m_pRingBuffer, RINGBUFFER_SIZE);
    m_pBeginPos = m_pRingBuffer;
    m_pReadPos = m_pRingBuffer;
    m_pWritePos = m_pRingBuffer;
    m_pEndPos = m_pRingBuffer + RINGBUFFER_SIZE - 1;
    m_isCreate = true;
    return true;
}


char* RingBuffer::AllocateToBuffer(uInt32 allocateLen)
{

    std::lock_guard<std::mutex> lock(m_mutex);
    {
        if (!m_isCreate)
        {
            std::cout << "[Server] ������ ���� ������. | RingBuffer::AllocateToBuffer" << std::endl;
            return nullptr;
        }
        // m_pWritePos����.
        char* pPreWritePos = nullptr;
        {
            // ������ �����÷� üũ
            if (m_usedBufferSize + allocateLen > m_bufferSize)
            {
                std::cout << "[Server] ������ ���� �÷ο� | RingBuffer::AllocateToBuffer" << std::endl;
                return nullptr;
            }

            if ((m_pEndPos - m_pWritePos) >= allocateLen) // �Ѿ�� ����.
            {
                pPreWritePos = m_pWritePos;
                m_pWritePos += allocateLen; // ����.
            }
            else
            {
                // ������ ���� �Ѿ��.
                // [][][][][][][][]
                //         w      p (4����Ʈ ����. 6����Ʈ�� �Ѿ��.)
                // [][][][][][][][]
                // b     w        p (w���� p������ �����͸� beginPos�� ����. �׸��� �� �ڿ� �߰��� allocate.
                // ���� pReadPos�� ��ģ�ٸ�  nullptr��ȯ.
                int copyLen = static_cast<int>(m_pEndPos - m_pWritePos);
                int compareLen = copyLen + allocateLen;
                int readPosLen = static_cast<int>(m_pReadPos - m_pRingBuffer);

                if (compareLen >= readPosLen) // compareLen�� �Ѿ�ų� ���ٸ� nullptr (pRead�� �����ϵ���)
                {
                    std::cout << "[Server] ������ ��ȯ ���� Release �ʿ�.(m_pReadPos) | RingBuffer::AllocateToBuffer" << std::endl;
                    return nullptr;
                }

                // �װԾƴ϶�� ����.
                ::CopyMemory(m_pBeginPos, m_pWritePos, copyLen);
                m_pWritePos = m_pBeginPos + copyLen;
                pPreWritePos = m_pWritePos;
                m_pWritePos += allocateLen;
            }
            m_usedBufferSize += allocateLen;
        }

        return pPreWritePos;
    }
}

char* RingBuffer::ReleaseBuffer(uInt32 releaseSize)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    {
        if (!m_isCreate)
        {
            std::cout << "[Server] ������ ���� ������. RingBuffer | ReleaseBuffer" << std::endl;
            return nullptr;
        }
        if (releaseSize > m_usedBufferSize)
        {
            return nullptr;
        }
        
        m_pReadPos += releaseSize;
        if (m_pReadPos == m_pWritePos)
        {
            m_pReadPos = m_pBeginPos;
            m_pWritePos = m_pBeginPos;
        }
        m_usedBufferSize -= releaseSize;
        return m_pReadPos;
    }
}
