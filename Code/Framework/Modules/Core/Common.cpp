#include "Core/Common.h"
#include "Core/Logger.h"

namespace boost
{

void CORE_API assertion_failed(char const* expr, char const* function, char const* file, long line)
{
    LOG_ALERT("ASSERTION_FAILED|%s|%s|%s:%d", expr, function, file, line);

    assert(false);
}

void CORE_API assertion_failed_msg(char const* expr, char const* msg, char const* function, char const* file, long line)
{
    LOG_ALERT("ASSERTION_FAILED_MSG|%s|%s|%s|%s:%d", expr, msg, function, file, line);

    assert(false);
}

} // namespace boost

// 获得执行文件所在目录
const std::string& GetExeDir()
{
    static std::string exeDir;

    if (exeDir.empty())
    {
#if TARGET_PLATFORM == PLATFORM_WINDOWS

        char modulePath[MAX_PATH]= { '\0' };
        char longPath[MAX_PATH] = { '\0' };

        GetModuleFileNameA(NULL, modulePath, MAX_PATH);
        GetLongPathNameA(modulePath, longPath, MAX_PATH);

        exeDir = longPath;
        exeDir = exeDir.erase(exeDir.rfind('\\') + 1);

#elif TARGET_PLATFORM == PLATFORM_LINUX

        char buf[PATH_MAX] = { '\0' };
        readlink("/proc/self/exe", buf, sizeof(buf) - 1);

        exeDir = buf;
        exeDir = exeDir.erase(exeDir.rfind('/') + 1);

#else

        BOOST_STATIC_ASSERT(false);

#endif // TARGET_PLATFORM
    }

    return exeDir;
}

#if TARGET_PLATFORM == PLATFORM_WINDOWS

// 把进程挂起一段时间, 单位: 微秒(千分之一毫秒)
void usleep(unsigned long usec)
{
    SleepEx(usec / 1000, TRUE);
}

// 把进程挂起一段时间, 单位: 秒
unsigned int sleep(unsigned int seconds)
{
    return SleepEx(seconds * 1000, TRUE);
}

// 获得错误字符串
boost::shared_ptr<std::string> GetStrError(DWORD* errorCode)
{
    char buf1[4096];
    char buf2[4096];

    DWORD curErrorCode = GetLastError();
    if (errorCode)
    {
        curErrorCode = *errorCode;
    }

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, curErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf1, sizeof(buf1), NULL);

    // 去掉 \r\n
    int len = strlen(buf1);
    if (len >= 2 && buf1[len - 2] == '\r' && buf1[len - 1] == '\n')
    {
        buf1[len - 2] = '\0';
    }

    len = snprintf(buf2, sizeof(buf2) - 1, "ErrorCode=%u, ErrorString=%s", curErrorCode, buf1);
    buf2[len] = '\0';

    return boost::shared_ptr<std::string>(new std::string(&buf2[0]));
}

#elif TARGET_PLATFORM == PLATFORM_LINUX

// 获得错误字符串
boost::shared_ptr<std::string> GetStrError(int* errorCode)
{
    char buf[4096];

    int curErrorCode = errno;
    if (errorCode)
    {
        curErrorCode = *errorCode;
    }

    int len = snprintf(buf, sizeof(buf) - 1, "errno=%d, strerror=%s.", curErrorCode, strerror(curErrorCode));
    buf[len] = '\0';

    return boost::shared_ptr<std::string>(new std::string(&buf[0]));
}

#endif // TARGET_PLATFORM
