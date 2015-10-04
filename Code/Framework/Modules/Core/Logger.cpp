#include "Core/Logger.h"

// 日志实例
Logger* g_Logger = NULL;

// 日志等级名称
static const char* s_LogLevelNames[NUM_LOG_LEVEL_TYPE] = {
    "silent",
    "bill",
    "fatal",
    "alert",
    "crit",
    "error",
    "warn",
    "notice",
    "info",
    "debug",
    "trace"
};

#if TARGET_PLATFORM == PLATFORM_WINDOWS
// 日志等级颜色
static WORD s_LogLevelColors[NUM_LOG_LEVEL_TYPE] = {
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN |  FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
};
#endif // TARGET_PLATFORM

// 日志
Logger::Logger()
    : m_Level(LOG_LEVEL_TRACE)
    , m_ConsoleLevel(LOG_LEVEL_TRACE)
    , m_Mutex()
    , m_File(NULL)
    , m_TotalLogHandler()
#if TARGET_PLATFORM == PLATFORM_WINDOWS
    , m_StdOutputHandle(INVALID_HANDLE_VALUE)
    , m_PreConsoleAttr(0)
#endif // TARGET_PLATFORM
{
    if (!g_Logger)
    {
        g_Logger = this;
    }

#if TARGET_PLATFORM == PLATFORM_WINDOWS
    m_StdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(m_StdOutputHandle != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(m_StdOutputHandle, &csbi))
    {
        m_PreConsoleAttr = csbi.wAttributes;
    }
#endif // TARGET_PLATFORM

    PrintLog(LOG_LEVEL_TRACE, __FILE__, __LINE__, "Logger::Logger()");
}

Logger::~Logger()
{
    PrintLog(LOG_LEVEL_TRACE, __FILE__, __LINE__, "Logger::~Logger()");

    CloseFile();

    if (g_Logger == this)
    {
        g_Logger = NULL;
    }
}

// 打印日志
void Logger::PrintLog(LOG_LEVEL_TYPE lv, const char* fileName, int line, const char* fmt, ...)
{
    BOOST_ASSERT(fmt);
    BOOST_ASSERT(fileName);

    if (lv > m_Level)
    {
        return;
    }

    int len = 0;
    char buf[MAX_LOG_LEN] = { '\0' };
    char* ptr = buf;
    char* last = buf + MAX_LOG_LEN;

    // 时间
    boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());

    len += snprintf(ptr, last - ptr, "%s %s|", to_iso_extended_string(now.date()).c_str(), to_simple_string(now.time_of_day()).c_str());
    ptr = buf + len;

    // 等级
    len += snprintf(ptr, last - ptr, "%s|", s_LogLevelNames[lv]);
    ptr = buf + len;

    // 内容
    va_list ap;
    va_start(ap, fmt);
    len += vsnprintf(ptr, last - ptr - 2, fmt, ap);
    ptr = buf + len;
    va_end(ap);

    // 代码文件和行数
    if (lv <= LOG_LEVEL_ERROR)
    {
        len += snprintf(ptr, last - ptr, "|%s:%d", fileName, line);
        ptr = buf + len;
    }

    // 换行
    buf[len++] = '\n';
    buf[len] = '\0';

    boost::lock_guard<boost::mutex> lockGuard(m_Mutex);

    // 写文件
    if (m_File)
    {
        fwrite(buf, 1, len, m_File);
    }

#if TARGET_PLATFORM == PLATFORM_WINDOWS

    if (lv <= m_ConsoleLevel)
    {
        if (m_StdOutputHandle != INVALID_HANDLE_VALUE)
        {
            SetConsoleTextAttribute(m_StdOutputHandle, s_LogLevelColors[lv]);
        }

        printf("%s", buf);

        if (m_StdOutputHandle != INVALID_HANDLE_VALUE)
        {
            SetConsoleTextAttribute(m_StdOutputHandle, m_PreConsoleAttr);
        }
    }

#ifdef DEBUG
    OutputDebugStringA(buf);
#endif // DEBUG

#elif TARGET_PLATFORM == PLATFORM_LINUX

    if (lv <= m_ConsoleLevel)
    {
        printf("%s", buf);
    }

#endif // TARGET_PLATFORM

    // 日志处理器
    for (std::size_t i = 0; i < m_TotalLogHandler.size(); ++i)
    {
        m_TotalLogHandler[i]->PrintLog(lv, buf, len);
    }

    // 错误日志立即刷新缓冲区, 防止程序挂掉没写入文件
    if (lv <= LOG_LEVEL_ERROR)
    {
        FlushLog();
    }
}

// 日志等级
LOG_LEVEL_TYPE Logger::GetLevel() const
{
    return m_Level;
}

void Logger::SetLevel(LOG_LEVEL_TYPE lv)
{
    m_Level = lv;
}

void Logger::SetLevel(const std::string& lv)
{
    for (int i = 0; i < sizeof(s_LogLevelNames) / sizeof(s_LogLevelNames[0]); ++i)
    {
        if (lv == s_LogLevelNames[i])
        {
            m_Level = (LOG_LEVEL_TYPE)i;
            return;
        }
    }

    LOG_ERROR("Wrong log level %s.", lv.c_str() );
}

// 打印到控制台的日志等级
LOG_LEVEL_TYPE Logger::GetConsoleLevel() const
{
    return m_ConsoleLevel;
}

void Logger::SetConsoleLevel(LOG_LEVEL_TYPE lv)
{
    m_ConsoleLevel = lv;
}

void Logger::SetConsoleLevel(const std::string& lv)
{
    for (int i = 0; i < sizeof(s_LogLevelNames) / sizeof(s_LogLevelNames[0]); ++i)
    {
        if (lv == s_LogLevelNames[i])
        {
            m_ConsoleLevel = (LOG_LEVEL_TYPE)i;
            return;
        }
    }

    LOG_ERROR("Wrong log level %s.", lv.c_str() );
}

// 文件
bool Logger::OpenFile(const std::string& fileName)
{
    CloseFile();

    m_File = fopen(fileName.c_str(), "ab+");
    if (m_File == NULL)
    {
        fprintf(stderr, "Open log file \"%s\" failed.\n", fileName.c_str());
        return false;
    }

    return true;
}

void Logger::CloseFile()
{
    if (m_File)
    {
        fclose(m_File);
        m_File = NULL;
    }
}

// 刷新日志输出缓冲
void Logger::FlushLog()
{
    if (m_File)
    {
        fflush(m_File);
    }

    for (std::size_t i = 0; i < m_TotalLogHandler.size(); ++i)
    {
        m_TotalLogHandler[i]->FlushLog();
    }
}

// 日志处理器
void Logger::AddLogHandler(LogHandler* logHandler)
{
    BOOST_ASSERT(logHandler);

    for (std::size_t i = 0; i < m_TotalLogHandler.size(); ++i)
    {
        if (m_TotalLogHandler[i] == logHandler)
        {
            return;
        }
    }

    m_TotalLogHandler.push_back(logHandler);
}

void Logger::DelLogHandler(LogHandler* logHandler)
{
    BOOST_ASSERT(logHandler);

    for (std::size_t i = 0; i < m_TotalLogHandler.size(); ++i)
    {
        if (m_TotalLogHandler[i] == logHandler)
        {
            m_TotalLogHandler.erase(m_TotalLogHandler.begin() + i);
            return;
        }
    }
}

// 获得日志等级名称
const char* Logger::GetLevelName(LOG_LEVEL_TYPE lv)
{
    return s_LogLevelNames[lv];
}
