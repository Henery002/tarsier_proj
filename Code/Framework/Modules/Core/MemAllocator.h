#ifndef CORE_MEMALLOCATOR_H_
#define CORE_MEMALLOCATOR_H_

#include "Core/Queue.h"
#include "Core/Singleton.h"

// 内存分配标签
enum MEM_TAG_TYPE
{
    MEM_TAG_DEFAULT,
    MEM_TAG_CORE,
    MEM_TAG_NET,
    MEM_TAG_APP,
    MEM_TAG_USR,
    NUM_MEM_TAG_TYPE
};

// 内存块
struct CORE_API MemBlock
{
    // 字节数
    int usrBytes;

    // 系统内存字节数
    int sysBytes;

    // 标签
    int tag;

    // 文件名
    char* fileName;

    // 行号
    int line;

    // 没有释放的内存队列
    QUEUE_T unfreeMemQueue;

    // 用户真正使用的内存
    char* usrMem;
};

// 内存分配器
class CORE_API MemAllocator : public SingletonDynamic<MemAllocator>
{
public:
    enum { MAX_TAG_NUM = 32 };

public:
    MemAllocator();
    virtual ~MemAllocator();

    // 分配内存
    void* AllocMem(int bytes, int tag, const char* fileName, int line);

    // 释放内存
    void FreeMem(void* mem, const char* fileName, int line);

    // 获得没有释放的用户内存字节数
    int GetUnfreeUsrMemBytes() const;

    // 获得没有释放的系统内存字节数, NOTE: 系统内存数 >= 用户内存数, 因为具体实现中可能会使用内存池或增加调试信息
    int GetUnfreeSysMemBytes() const;

protected:
    // 没有释放的用户内存字节数
    int m_UnfreeUsrMemBytes;

    // 没有释放的系统内存字节数
    int m_UnfreeSysMemBytes;

    // 没有释放的用户内存字节数, 标签分类
    int m_UnfreeUsrMemBytesTagged[MAX_TAG_NUM];

    // 没有释放的内存队列
    QUEUE_T m_UnfreeMemQueue;

    // 互斥体
    boost::mutex m_Mutex;
};

// 内存分配器实例
extern MemAllocator* g_MemAllocator;

// 能跟踪申请内存的代码位置的宏(有助于内存泄露定位), 没特殊情况都使用此宏
#define MALLOC(bytes, tag) MemAllocator::Inst()->AllocMem(bytes, tag, __FILE__, __LINE__)
#define FREE(mem) if (mem) MemAllocator::Inst()->FreeMem(mem, __FILE__, __LINE__); mem = NULL

#endif // CORE_MEMALLOCATOR_H_
