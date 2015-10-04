#include "Core/MemAllocator.h"
#include "Core/Logger.h"

// 内存分配器实例
MemAllocator* g_MemAllocator = NULL;

// 内存分配器
MemAllocator::MemAllocator()
    : m_UnfreeUsrMemBytes(0)
    , m_UnfreeSysMemBytes(0)
    , m_Mutex()
{
    LOG_TRACE("MemAllocator::MemAllocator()");

    if (!g_MemAllocator)
    {
        g_MemAllocator = this;
    }

    memset(m_UnfreeUsrMemBytesTagged, 0, sizeof(m_UnfreeUsrMemBytesTagged));

    QUEUE_INIT(&m_UnfreeMemQueue);
}

MemAllocator::~MemAllocator()
{
    LOG_TRACE("MemAllocator::~MemAllocator()");

    for (QUEUE_T* q = QUEUE_HEAD(&m_UnfreeMemQueue); q != QUEUE_SENTINEL(&m_UnfreeMemQueue); q = QUEUE_NEXT(q))
    {
        MemBlock* memBlock = QUEUE_DATA(q, MemBlock, unfreeMemQueue);

        LOG_ERROR("MEMORY LEAKS|%d|%s|%d", memBlock->usrBytes, memBlock->fileName, memBlock->line);
    }

    //BOOST_ASSERT(QUEUE_EMPTY(&m_UnfreeMemQueue));

    if (g_MemAllocator == this)
    {
        g_MemAllocator = NULL;
    }
}

// 分配内存
void* MemAllocator::AllocMem(int bytes, int tag, const char* fileName, int line)
{
    BOOST_ASSERT(bytes >= 0);
    BOOST_ASSERT(0 <= tag && tag < MAX_TAG_NUM);
    BOOST_ASSERT(fileName);

    boost::lock_guard<boost::mutex> lockGuard(m_Mutex);

    int nameLen = strlen(fileName);
    int realBytes = bytes + sizeof(MemBlock);

    MemBlock* memBlock = (MemBlock*)malloc(realBytes);

    memset(memBlock, 0, sizeof(MemBlock));

    // 记录标签
    memBlock->tag = tag;

    // 记录分配内存大小
    memBlock->usrBytes = bytes;
    memBlock->sysBytes = realBytes + nameLen + 1;

    // 记录申请的代码位置
    memBlock->fileName = (char*)malloc(nameLen + 1);
    memBlock->fileName[nameLen] = '\0';
    strcpy(memBlock->fileName, fileName);

    memBlock->line = line;

    // 用户真正使用的内存
    memBlock->usrMem = (char*)memBlock + offsetof(MemBlock, usrMem);

    // 压入内存统计队列
    QUEUE_INSERT_TAIL(&m_UnfreeMemQueue, &memBlock->unfreeMemQueue);

    // 增加统计数据
    m_UnfreeUsrMemBytes += bytes;
    m_UnfreeSysMemBytes += memBlock->sysBytes;
    m_UnfreeUsrMemBytesTagged[tag] += bytes;

    return memBlock->usrMem;
}

// 释放内存
void MemAllocator::FreeMem(void* mem, const char* fileName, int line)
{
    BOOST_ASSERT(mem);
    BOOST_ASSERT(fileName);

    boost::lock_guard<boost::mutex> lockGuard(m_Mutex);

    MemBlock* memBlock = (MemBlock*)((char*)mem - offsetof(MemBlock, usrMem));

    int tag = memBlock->tag;

    BOOST_ASSERT(0 <= tag && tag < MAX_TAG_NUM);

    free(memBlock->fileName);

    // 减少统计数据
    m_UnfreeUsrMemBytes -= memBlock->usrBytes;
    m_UnfreeSysMemBytes -= memBlock->sysBytes;
    m_UnfreeUsrMemBytesTagged[tag] -= memBlock->usrBytes;

    QUEUE_REMOVE(&memBlock->unfreeMemQueue);

    free(memBlock);
}

// 获得没有释放的用户内存字节数
int MemAllocator::GetUnfreeUsrMemBytes() const
{
    return m_UnfreeUsrMemBytes;
}

// 获得没有释放的系统内存字节数, NOTE: 系统内存数 >= 用户内存数, 因为具体实现中可能会使用内存池或增加调试信息
int MemAllocator::GetUnfreeSysMemBytes() const
{
    return m_UnfreeSysMemBytes;
}
