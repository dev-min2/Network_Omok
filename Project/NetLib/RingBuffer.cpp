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
        std::cout << "링버퍼 메모리 할당 실패.. RingBuffer | Create" << std::endl;
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
            std::cout << "[Server] 링버퍼 생성 안했음. | RingBuffer::AllocateToBuffer" << std::endl;
            return nullptr;
        }
        // m_pWritePos전진.
        char* pPreWritePos = nullptr;
        {
            // 링버퍼 오버플로 체크
            if (m_usedBufferSize + allocateLen > m_bufferSize)
            {
                std::cout << "[Server] 링버퍼 오버 플로우 | RingBuffer::AllocateToBuffer" << std::endl;
                return nullptr;
            }

            if ((m_pEndPos - m_pWritePos) >= allocateLen) // 넘어가지 않음.
            {
                pPreWritePos = m_pWritePos;
                m_pWritePos += allocateLen; // 전진.
            }
            else
            {
                // 링버퍼 끝을 넘어간다.
                // [][][][][][][][]
                //         w      p (4바이트 가정. 6바이트가 넘어가면.)
                // [][][][][][][][]
                // b     w        p (w부터 p까지의 데이터를 beginPos에 복사. 그리고 그 뒤에 추가로 allocate.
                // 만약 pReadPos와 겹친다면  nullptr반환.
                int copyLen = static_cast<int>(m_pEndPos - m_pWritePos);
                int compareLen = copyLen + allocateLen;
                int readPosLen = static_cast<int>(m_pReadPos - m_pRingBuffer);

                if (compareLen >= readPosLen) // compareLen이 넘어서거나 같다면 nullptr (pRead를 진행하도록)
                {
                    std::cout << "[Server] 링버퍼 순환 실패 Release 필요.(m_pReadPos) | RingBuffer::AllocateToBuffer" << std::endl;
                    return nullptr;
                }

                // 그게아니라면 복사.
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
            std::cout << "[Server] 링버퍼 생성 안했음. RingBuffer | ReleaseBuffer" << std::endl;
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
