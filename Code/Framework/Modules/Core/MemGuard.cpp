#include "Core/MemGuard.h"
#include "Core/MemAllocator.h"

// 离开作用域自动释放的内存
MemGuard::MemGuard(int bytes, bool zeroFill, int tag, const char* fileName, int line)
    : m_Mem(NULL)
    , m_MemSize(bytes)
    , m_FileName(fileName)
    , m_Line(line)
{
    m_Mem = MemAllocator::Inst()->AllocMem(bytes, tag, fileName, line);

    if (zeroFill)
    {
        memset(m_Mem, 0, bytes);
    }
}

MemGuard::~MemGuard()
{
    if (m_Mem)
    {
        MemAllocator::Inst()->FreeMem(m_Mem, m_FileName, m_Line);
        m_Mem = NULL;
    }
}
