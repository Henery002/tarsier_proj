#ifndef CORE_MEMGUARD_H_
#define CORE_MEMGUARD_H_

#include "Core/Common.h"

// 离开作用域自动释放的内存
class CORE_API MemGuard
{
public:
    MemGuard(int bytes, bool zeroFill, int tag, const char* fileName, int line);
    ~MemGuard();

    // 获得内存指针
    inline void* GetMem() const
    {
        return m_Mem;
    }

    // 获得内存大小
    inline int GetMemSize() const
    {
        return m_MemSize;
    }

protected:
    MemGuard();
    MemGuard(const MemGuard& r);
    MemGuard& operator=(const MemGuard& r);

    // 内存指针
    void* m_Mem;

    // 内存大小
    int m_MemSize;

    // 文件名
    const char* m_FileName;

    // 行数
    int m_Line;
};

#define MEM_GUARD(var, bytes, zeroFill, tag) MemGuard var(bytes, zeroFill, tag, __FILE__, __LINE__)

#endif // CORE_MEMGUARD_H_
