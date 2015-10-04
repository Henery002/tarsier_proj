#ifndef CORE_LOGGER_H_
#define CORE_LOGGER_H_

#include "Core/Common.h"
#include "Core/Singleton.h"

// 日志等级
enum LOG_LEVEL_TYPE
{
    LOG_LEVEL_SILENT, // 静默等级, 不输出任何日志
    LOG_LEVEL_BILL, // 账单信息, 金钱、物品等变化信息
    LOG_LEVEL_FATAL, // 致命错误, 导致系统不可用的错误
    LOG_LEVEL_ALERT, // 紧急错误, 必须对其采取紧急措施的错误
    LOG_LEVEL_CRIT, // 严重错误, 导致系统处于危险状态的错误
    LOG_LEVEL_ERROR, // 一般错误, 一般性的错误提示
    LOG_LEVEL_WARN, // 警告信息, 系统仍然正常运行，但可能存在隐患的提示信息
    LOG_LEVEL_NOTICE, // 通知信息, 正常状态下的重要提示信息
    LOG_LEVEL_INFO, // 消息报告, 一般性的提示信息
    LOG_LEVEL_DEBUG, // 调试信息, 调试过程产生的信息
    LOG_LEVEL_TRACE, // 跟踪信息, 跟踪代码执行顺序的信息
    NUM_LOG_LEVEL_TYPE
};

// 日志处理器
class CORE_API LogHandler
{
public:
    LogHandler() {}
    virtual ~LogHandler() {}

    // 打印日志
    virtual void PrintLog(LOG_LEVEL_TYPE lv, const char* msg, int len) = 0;

    // 刷新日志输出缓冲
    virtual void FlushLog() = 0;
};

typedef std::vector<LogHandler*> LogHandlerContainer;

// 日志
class CORE_API Logger : public SingletonDynamic<Logger>
{
public:
    // 最大日志长度
    enum { MAX_LOG_LEN = 1024 };

public:
    Logger();
    ~Logger();

    // 打印日志
    void PrintLog(LOG_LEVEL_TYPE lv, const char* fileName, int line, const char* fmt, ...);

    // 日志等级
    LOG_LEVEL_TYPE GetLevel() const;
    void SetLevel(LOG_LEVEL_TYPE lv);
    void SetLevel(const std::string& lv);

    // 打印到控制台的日志等级
    LOG_LEVEL_TYPE GetConsoleLevel() const;
    void SetConsoleLevel(LOG_LEVEL_TYPE lv);
    void SetConsoleLevel(const std::string& lv);

    // 文件
    bool OpenFile(const std::string& fileName);
    void CloseFile();

    // 刷新日志输出缓冲
    void FlushLog();

    // 日志处理器
    void AddLogHandler(LogHandler* logHandler);
    void DelLogHandler(LogHandler* logHandler);

    // 获得日志等级名称
    static const char* GetLevelName(LOG_LEVEL_TYPE lv);

protected:
    // 日志等级
    LOG_LEVEL_TYPE m_Level;

    // 打印到控制台的日志等级
    LOG_LEVEL_TYPE m_ConsoleLevel;

    // 互斥体
    boost::mutex m_Mutex;

    // 文件
    FILE* m_File;

    // 所有的日志处理器
    LogHandlerContainer m_TotalLogHandler;

#if TARGET_PLATFORM == PLATFORM_WINDOWS
    // 标准输出句柄
    HANDLE m_StdOutputHandle;

    // 原来的控制台属性
    WORD m_PreConsoleAttr;
#endif // TARGET_PLATFORM
};

// 日志实例
extern Logger* g_Logger;

#define LOG_BILL(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_BILL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ALERT(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_ALERT, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_CRIT(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_CRIT, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_NOTICE(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_NOTICE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt, ...) Logger::Inst()->PrintLog(LOG_LEVEL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif // CORE_LOGGER_H_
