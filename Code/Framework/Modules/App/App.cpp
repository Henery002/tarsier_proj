#include "App/App.h"

#if TARGET_PLATFORM == PLATFORM_LINUX
#include <signal.h>
#include <sys/file.h>
#endif // TARGET_PLATFORM

// 应用程序实例
App* g_App = NULL;

// 应用程序
App::App()
    : m_ExitMainLoop(false)
    , m_Daemon(false)
    , m_SignalType(SIGNAL_INVALID)
    , m_IdleCount(APP_DEFAULT_IDLE_COUNT)
    , m_CurIdleCount(0)
    , m_IdleSleep(APP_DEFAULT_IDLE_SLEEP)
    , m_TotalSleepNum(0)
    , m_OptionsDesc("Allowed options")
    , m_VariablesMap()
    , m_Version("1.0.0.0")
    , m_AppName("TApp")
    , m_ConfigFile()
    , m_PidFile()
    , m_LogFile()
    , m_ExeDir()
    , m_InputMutex()
    , m_InputQueue()
    , m_BackdoorAcceptor("0.0.0.0", APP_DEFAULT_BACKDOOR_PORT, false)
{
    LOG_TRACE("App::App()");

    BOOST_ASSERT(!g_App);

    g_App = this;
}

App::~App()
{
    LOG_TRACE("App::~App()");

    g_App = NULL;

    Logger::DestroyInst();
}

// 初始化
bool App::Init(int argc, char* argv[])
{
    BOOST_ASSERT(argc >= 0);
    BOOST_ASSERT(argv);

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    LOG_TRACE("App::Init(%d, ...)", argc);

    if (!RegisterOptions(argc, argv))
    {
        return false;
    }

    if (!ParseOptions())
    {
        return false;
    }

    if (!CreateSignals())
    {
        return false;
    }

    if (!Net::Inst()->Init())
    {
        return false;
    }

    LOG_NOTICE("Starting %s...", m_AppName.c_str());

    if (m_BackdoorAcceptor.IsEnable())
    {
        if (!m_BackdoorAcceptor.CreateAcceptor())
        {
            return false;
        }

        m_BackdoorAcceptor.GetAcceptor()->SetAcceptorListener(&m_BackdoorAcceptor);
    }

    if (!InitImpl())
    {
        return false;
    }

    return true;
}

// 初始化实现
bool App::InitImpl()
{
    return true;
}

// 注册选项
bool App::RegisterOptions(int argc, char* argv[])
{
    BOOST_ASSERT(argc >= 0);
    BOOST_ASSERT(argv);

    try
    {
        m_OptionsDesc.add_options()
        ("stop", "Request previous process to stop safely.")
        ("reload", "Request previous process to reload configuration.")
        ("daemon", "Runs as a background process.")
        ("version", "Output version information.")
        ("help", "Output help information.")
        ("config-file", boost::program_options::value<std::string>(), "Specify path of the file for storing the config.")
        ("pid-file", boost::program_options::value<std::string>(), "Specify path of the file for storing the process\'s pid.")
        ("log-file", boost::program_options::value<std::string>(), "Specify path of the file for storing the log.")
        ("log-level", boost::program_options::value<std::string>(), "Specify level of the log.")
        ("console-level", boost::program_options::value<std::string>(), "Specify level of print log to console.")
        ("idle-count", boost::program_options::value<int>(), "Specify idle-cycle-count before entering idle status.")
        ("idle-sleep", boost::program_options::value<int>(), "Specify sleep-time-gap for idle status.")
        ("backdoor-ip", boost::program_options::value<std::string>(), "Specify backdoor bind ip.")
        ("backdoor-port", boost::program_options::value<int>(), "Specify backdoor bind port.")
        ("backdoor-enable", boost::program_options::value<bool>(), "Specify backdoor is enabled.")
        ("backdoor-pwd", boost::program_options::value<bool>(), "Specify backdoor password.")
#if TARGET_PLATFORM == PLATFORM_WINDOWS
        ("console-x", boost::program_options::value<int>(), "Specify x of the console.")
        ("console-y", boost::program_options::value<int>(), "Specify y of the console.")
        ("console-width", boost::program_options::value<int>(), "Specify width of the console.")
        ("console-height", boost::program_options::value<int>(), "Specify height of the console.")
        ("console-buffer-height", boost::program_options::value<int>(), "Specify buffer-height of the console.")
#endif // TARGET_PLATFORM
        ;

        if (!RegisterOptionsImpl())
        {
            return false;
        }

        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, m_OptionsDesc), m_VariablesMap);
        boost::program_options::notify(m_VariablesMap);
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "%s\n", e.what());
        exit(-1);
    }

    return true;
}

// 注册选项实现
bool App::RegisterOptionsImpl()
{
    return true;
}

// 解析选项
bool App::ParseOptions()
{
    // 配置文件
    if (m_VariablesMap.count("config-file"))
    {
        m_ConfigFile = m_VariablesMap["config-file"].as<std::string>();
    }

    if (!m_ConfigFile.empty() && !OpenConfigFile(false))
    {
        exit(-1);
    }

    // 进程 ID 文件
    if (m_VariablesMap.count("pid-file"))
    {
        m_PidFile = m_VariablesMap["pid-file"].as<std::string>();
    }

    if (m_PidFile.empty())
    {
        m_PidFile = GetExeDir() + m_AppName + ".pid";
    }

    // 停止
    if (m_VariablesMap.count("stop"))
    {
        if (!LockPidFile())
        {
            if (SendSignal(SIGNAL_STOP))
            {
                printf("Succeed to stop %s.\n", m_AppName.c_str());
            }
            exit(0);
        }
        else
        {
            fprintf(stderr, "Failed to stop %s, because the program is not running.\n", m_AppName.c_str());
            exit(-1);
        }
    }

    // 重载配置
    if (m_VariablesMap.count("reload"))
    {
        if (!LockPidFile())
        {
            if (SendSignal(SIGNAL_RELOAD))
            {
                printf("Succeed to reload %s.\n", m_AppName.c_str());
            }
            exit(0);
        }
        else
        {
            fprintf(stderr, "Failed to reload %s, because the program is not running.\n", m_AppName.c_str());
            exit(-1);
        }
    }

    // 输出版本信息
    if (m_VariablesMap.count("version"))
    {
        OutputVersion();
        exit(0);
    }

    // 输出帮助信息
    if (m_VariablesMap.count("help"))
    {
        std::cout << m_OptionsDesc;
        exit(0);
    }

    if (!LockPidFile())
    {
        fprintf(stderr, "Failed to start %s, because the program is already running.\n", m_AppName.c_str());
        exit(-1);
    }

    // 日志文件
    if (m_VariablesMap.count("log-file"))
    {
        m_LogFile = m_VariablesMap["log-file"].as<std::string>();
    }

    if (m_LogFile.empty())
    {
        m_LogFile = GetExeDir() + m_AppName + ".log";
    }

    if (!Logger::Inst()->OpenFile(m_LogFile.c_str()))
    {
        return false;
    }

    if (!ParseReloadableOptions(false))
    {
        return false;
    }

#if TARGET_PLATFORM == PLATFORM_WINDOWS

    // 控制台坐标
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow)
    {
        int consoleX = 0;
        int consoleY = 0;

        if (m_VariablesMap.count("console-x"))
        {
            consoleX = m_VariablesMap["console-x"].as<int>();
        }

        if (m_VariablesMap.count("console-y"))
        {
            consoleY = m_VariablesMap["console-y"].as<int>();
        }

        SetWindowPos(consoleWindow, NULL, consoleX, consoleY, 0, 0, SWP_NOSIZE);
    }

    HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdHandle)
    {
        int consoleWidth = 123;
        int consoleHeight = 40;
        int consoleBufferHeight = 1000;

        // 控制台宽度
        if (m_VariablesMap.count("console-width"))
        {
            int w = m_VariablesMap["console-width"].as<int>();
            if (w <= 0)
            {
                LOG_WARN("Bad console-width %d.", w);
            }
            else
            {
                consoleWidth = w;
            }
        }

        // 控制台高度
        if (m_VariablesMap.count("console-height"))
        {
            int h = m_VariablesMap["console-height"].as<int>();
            if (h <= 0)
            {
                LOG_WARN("Bad console-height %d.", h);
            }
            else
            {
                consoleHeight = h;
            }
        }

        // 控制台缓冲区高度
        if (m_VariablesMap.count("console-buffer-height"))
        {
            int h = m_VariablesMap["console-buffer-height"].as<int>();
            if (h <= consoleHeight)
            {
                LOG_WARN("Bad console-buffer-height %d.", h);
            }
            else
            {
                consoleBufferHeight = h;
            }
        }

        COORD bufferSize = { consoleWidth, consoleBufferHeight };
        SMALL_RECT winRect = { 0, 0, bufferSize.X - 1, consoleHeight - 1 };

        SetConsoleScreenBufferSize(stdHandle, bufferSize);
        SetConsoleWindowInfo(stdHandle, TRUE, &winRect);
    }

    SetConsoleTitleA(m_AppName.c_str());

#endif // TARGET_PLATFORM

    // 守护进程
    if (m_VariablesMap.count("daemon"))
    {
        m_Daemon = true;

#if TARGET_PLATFORM == PLATFORM_WINDOWS

        if (!FreeConsole())
        {
            LOG_ERROR("FreeConsole() failed, %s.", GetStrError()->c_str());
        }

#elif TARGET_PLATFORM == PLATFORM_LINUX

        if (daemon(1, 0) == -1)
        {
            LOG_ERROR("daemon(1, 0) failed, %s.", GetStrError()->c_str());
        }

#endif // TARGET_PLATFORM
    }

    if (!WritePid(m_PidFile))
    {
        return false;
    }

    return ParseOptionsImpl();
}

// 解析选项实现
bool App::ParseOptionsImpl()
{
    return true;
}

// 解析可以重载的选项
bool App::ParseReloadableOptions(bool reload)
{
    // 日志等级
    if (m_VariablesMap.count("log-level"))
    {
        std::string newlyLogLevel = m_VariablesMap["log-level"].as<std::string>();
        LogChangedOption("log-level", Logger::GetLevelName(Logger::Inst()->GetLevel()), boost::to_string(newlyLogLevel), reload);
        Logger::Inst()->SetLevel(newlyLogLevel);
    }

    // 打印到控制台的日志等级
    if (m_VariablesMap.count("console-level"))
    {
        std::string newlyLogLevel = m_VariablesMap["console-level"].as<std::string>();
        LogChangedOption("console-level", Logger::GetLevelName(Logger::Inst()->GetConsoleLevel()), boost::to_string(newlyLogLevel), reload);
        Logger::Inst()->SetConsoleLevel(newlyLogLevel);
    }

    // 空闲计数
    if (m_VariablesMap.count("idle-count"))
    {
        int idleCount = m_VariablesMap["idle-count"].as<int>();
        if (idleCount <= 0)
        {
            LOG_WARN("Bad idle-count %d.", idleCount);
        }
        else
        {
            LogChangedOption("idle-count", boost::to_string(m_IdleCount), boost::to_string(idleCount), reload);
            m_IdleCount = idleCount;
        }
    }

    // 空闲挂起时间
    if (m_VariablesMap.count("idle-sleep"))
    {
        int idleSleep = m_VariablesMap["idle-sleep"].as<int>();
        if (idleSleep <= 0)
        {
            LOG_WARN("Bad idle-sleep %d.", idleSleep);
        }
        else
        {
            LogChangedOption("idle-sleep", boost::to_string(m_IdleSleep), boost::to_string(idleSleep), reload);
            m_IdleSleep = idleSleep;
        }
    }

    // 后门接收器
    ParseAcceptorOptions(m_BackdoorAcceptor, "backdoor-ip", "backdoor-port", "backdoor-enable", reload);

    // 后门接收器密码
    if (m_VariablesMap.count("backdoor-pwd"))
    {
        std::string newlyPwd = m_VariablesMap["backdoor-pwd"].as<std::string>();
        LogChangedOption("backdoor-pwd", m_BackdoorAcceptor.GetPwd(), newlyPwd, reload);
        m_BackdoorAcceptor.SetPwd(newlyPwd);
    }

    return ParseReloadableOptionsImpl(reload);
}

// 解析可以重载的选项实现
bool App::ParseReloadableOptionsImpl(bool reload)
{
    return true;
}

// 关闭
void App::Shutdown()
{
    LOG_TRACE("App::Shutdown()");

    ShutdownImpl();

    Net::Inst()->Shutdown();

    MemAllocator::DestroyInst();

    google::protobuf::ShutdownProtobufLibrary();

    DestroySelf();
}

// 关闭实现
void App::ShutdownImpl()
{

}

#if TARGET_PLATFORM == PLATFORM_WINDOWS

// 事件数组
static std::string s_EventNames[NUM_SIGNAL_TYPE];
static HANDLE s_Events[NUM_SIGNAL_TYPE] = { 0 };

// 事件监听线程
static void _EventListenThread()
{
    LOG_TRACE("_EventListenThread Start");

    while (!g_App->IsExitMainLoop())
    {
        DWORD evt = WaitForMultipleObjects(sizeof(s_Events) / sizeof(s_Events[0]), s_Events, FALSE, 1000);

        if (evt == WAIT_OBJECT_0)
        {
            g_App->OnStopSignal();
            break;
        }
        else if (evt == WAIT_OBJECT_0 + 1)
        {
            g_App->OnReloadSignal();

            if (!ResetEvent(s_Events[SIGNAL_RELOAD]))
            {
                LOG_ERROR("ResetEvent(\"%s\") failed, %s.", s_EventNames[SIGNAL_RELOAD].c_str(), GetStrError()->c_str());
            }
        }
        else if (evt == WAIT_FAILED)
        {
            LOG_ERROR("WaitForMultipleObjects() failed, %s.", GetStrError()->c_str());
        }
    }

    LOG_TRACE("_EventListenThread Stop");
}

// 主线程句柄
static HANDLE s_MainThread = 0;

// 控制台事件处理函数
static BOOL __stdcall _ConsoleHandler(DWORD ctrlType)
{
    const char* msg = "";

    switch (ctrlType)
    {
    case CTRL_C_EVENT:
        msg = "Ctrl-C pressed, exiting...";
        break;
    case CTRL_CLOSE_EVENT:
        msg = "Console closing, exiting...";
        break;
    case CTRL_BREAK_EVENT:
        msg = "Ctrl-Break pressed, exiting...";
        break;
    case CTRL_LOGOFF_EVENT:
        msg = "User logs off, exiting...";
        break;
    case CTRL_SHUTDOWN_EVENT:
        msg = "System shutdown, exiting...";
        break;
    default:
        return FALSE;
    }

    LOG_INFO(msg);

    if (s_Events[SIGNAL_STOP])
    {
        if (!SetEvent(s_Events[SIGNAL_STOP]))
        {
            LOG_ERROR("SetEvent(\"%s\") failed, %s.", s_EventNames[SIGNAL_STOP].c_str(), GetStrError()->c_str());
        }

        if (ctrlType == CTRL_CLOSE_EVENT)
        {
            WaitForSingleObject(s_MainThread, INFINITE);
        }
    }

    return TRUE;
}

#endif // #if TARGET_PLATFORM

// 输入线程
static void _InputThread()
{
    LOG_TRACE("_InputThread Start");

    while (!g_App->IsExitMainLoop())
    {
        std::string input;
        std::getline(std::cin, input);
        g_App->AddInput(input);
    }

    LOG_TRACE("_InputThread Stop");
}

// 运行
void App::Run()
{
    LOG_NOTICE("Started %s.", m_AppName.c_str());

#if TARGET_PLATFORM == PLATFORM_WINDOWS

    s_MainThread = GetCurrentThread();

    if (!SetConsoleCtrlHandler(_ConsoleHandler, TRUE))
    {
        LOG_ERROR("SetConsoleCtrlHandler() failed, %s.", GetStrError()->c_str());
    }

    boost::thread eventListenThread(_EventListenThread);

#endif // #if TARGET_PLATFORM

    if (!m_Daemon)
    {
        boost::thread inputThread(_InputThread);
    }

    while (!m_ExitMainLoop)
    {
        switch (m_SignalType)
        {
        case SIGNAL_STOP:
            Stop();
            break;
        case SIGNAL_RELOAD:
            Reload();
            break;
        };

        Update();
    }

#if TARGET_PLATFORM == PLATFORM_WINDOWS
    eventListenThread.join();
#endif // #if TARGET_PLATFORM

    LOG_NOTICE("Stoped %s.", m_AppName.c_str());
}

// 更新
void App::Update()
{
    UpdateInput();
    UpdateImpl();

    Net::Inst()->Update();

    if (++m_CurIdleCount > m_IdleCount)
    {
        m_CurIdleCount = 0;

        usleep(m_IdleSleep * 1000);

        m_TotalSleepNum++;
    }
}

// 更新实现
void App::UpdateImpl()
{

}

// 停止
void App::Stop()
{
    BOOST_ASSERT(m_SignalType == SIGNAL_STOP);

    LOG_NOTICE("Stoping %s...", m_AppName.c_str());

    m_SignalType = SIGNAL_INVALID;
    m_ExitMainLoop = true;

    StopImpl();

    m_BackdoorAcceptor.DestroyAcceptor();
}

// 停止实现
void App::StopImpl()
{

}

// 重载配置
void App::Reload()
{
    BOOST_ASSERT(m_SignalType == SIGNAL_RELOAD);

    LOG_NOTICE("Reloading %s...", m_AppName.c_str());

    m_SignalType = SIGNAL_INVALID;

    if (!m_ConfigFile.empty() && OpenConfigFile(true))
    {
        ParseReloadableOptions(true);
    }

    ReloadImpl();

    LOG_NOTICE("Reloaded %s.", m_AppName.c_str());

    Logger::Inst()->FlushLog();
}

// 重载配置实现
void App::ReloadImpl()
{

}

// 刷新缓冲区
void App::Flush()
{
    LOG_NOTICE("Flushing %s...", m_AppName.c_str());

    FlushImpl();

    LOG_NOTICE("Flushed %s.", m_AppName.c_str());

    Logger::Inst()->FlushLog();
}

// 刷新缓冲区实现
void App::FlushImpl()
{

}

// 增加输入
void App::AddInput(const std::string& input)
{
    boost::lock_guard<boost::mutex> lockGuard(m_InputMutex);
    m_InputQueue.push(input);
}

// 更新输入
void App::UpdateInput()
{
    while (!m_InputQueue.empty())
    {
        boost::lock_guard<boost::mutex> lockGuard(m_InputMutex);

        HandleInput(m_InputQueue.front());

        m_InputQueue.pop();
    }
}

// 处理输入
void App::HandleInput(const std::string& input)
{
    if (input == "stop")
    {
        OnStopSignal();
    }
    else if (input == "reload")
    {
        OnReloadSignal();
    }
    else if (input == "flush")
    {
        Flush();
    }
    else if (input == "help")
    {
        OutputInputHelp();
    }
    else
    {
        HandleInputImpl(input);
    }
}

// 处理输入实现
void App::HandleInputImpl(const std::string& input)
{

}

// 是否退出主循环
bool App::IsExitMainLoop()
{
    return m_ExitMainLoop;
}

// 收到停止信号
void App::OnStopSignal()
{
    m_SignalType = SIGNAL_STOP;
}

// 收到重载配置信号
void App::OnReloadSignal()
{
    m_SignalType = SIGNAL_RELOAD;
}

// 输出版本信息
void App::OutputVersion()
{
    printf("%s\n", m_Version.c_str());
}

// 输出输入帮助
void App::OutputInputHelp()
{
    printf("%s\n", "Allowed inputs:");
    printf("  %s\n", "stop");
    printf("  %s\n", "reload");
    printf("  %s\n", "flush");
    printf("  %s\n", "help");
}

// 销毁自己
void App::DestroySelf()
{
    delete this;
}

#if TARGET_PLATFORM == PLATFORM_LINUX

void _OnSigStop(int sig)
{
    g_App->OnStopSignal();
}

void _OnSigReload(int sig)
{
    g_App->OnReloadSignal();
}

#endif // TARGET_PLATFORM

// 创建信号
bool App::CreateSignals()
{
#if TARGET_PLATFORM == PLATFORM_WINDOWS

    char eventName[256] = { '\0' };

    // 停止事件
    snprintf(eventName, sizeof(eventName) - 1, "Global\\TApp_Stop_%d", getpid());
    s_EventNames[SIGNAL_STOP] = eventName;
    s_Events[SIGNAL_STOP] = CreateEventA(NULL, TRUE, FALSE, eventName);
    if (s_Events[SIGNAL_STOP] == NULL)
    {
        LOG_ERROR("CreateEvent(\"%s\") failed, %s.", eventName, GetStrError()->c_str());
        return false;
    }

    // 重载配置事件
    snprintf(eventName, sizeof(eventName) - 1, "Global\\TApp_Reload_%d", getpid());
    s_EventNames[SIGNAL_RELOAD] = eventName;
    s_Events[SIGNAL_RELOAD] = CreateEventA(NULL, TRUE, FALSE, eventName);
    if (s_Events[SIGNAL_RELOAD] == NULL)
    {
        LOG_ERROR("CreateEvent(\"%s\") failed, %s.", eventName, GetStrError()->c_str());
        return false;
    }

#elif TARGET_PLATFORM == PLATFORM_LINUX

    struct sigaction act;

    memset(&act, 0, sizeof(act));
    sigfillset(&act.sa_mask);

    // 停止
    act.sa_handler = _OnSigStop;

    // kill -s 2 pid or CTRL + C
    sigaction(SIGINT, &act, NULL);
    // kill -s 10 pid
    sigaction(SIGUSR1, &act, NULL);

    // 重载配置
    act.sa_handler = _OnSigReload;

    // kill -s 12 pid
    sigaction(SIGUSR2, &act, NULL);

#else

    BOOST_STATIC_ASSERT(false);

#endif // TARGET_PLATFORM

    return true;
}

// 发送信号
bool App::SendSignal(SIGNAL_TYPE signalType)
{
#if TARGET_PLATFORM == PLATFORM_WINDOWS

    char eventName[256] = { '\0' };
    HANDLE eventHandle = NULL;

    pid_t pid = ReadPid(m_PidFile);

    switch (signalType)
    {
    case SIGNAL_STOP:
        snprintf(eventName, sizeof(eventName) - 1, "Global\\TApp_Stop_%d", pid);
        break;
    case SIGNAL_RELOAD:
        snprintf(eventName, sizeof(eventName) - 1, "Global\\TApp_Reload_%d", pid);
        break;
    default:
        LOG_ERROR("Wrong signalType %d.", signalType);
        break;
    }

    eventHandle = OpenEventA(EVENT_MODIFY_STATE, FALSE, eventName);
    if (!eventHandle)
    {
        LOG_ERROR("OpenEvent(\"%s\") failed, %s.", eventName, GetStrError()->c_str());
        return false;
    }

    if (!SetEvent(eventHandle))
    {
        LOG_ERROR("SetEvent(\"%s\") failed, %s.", eventName, GetStrError()->c_str());
        return false;
    }

    return true;

#elif TARGET_PLATFORM == PLATFORM_LINUX

    int sig = -1;
    pid_t pid = ReadPid(m_PidFile);

    switch (signalType)
    {
    case SIGNAL_STOP:
        sig = SIGUSR1;
        break;
    case SIGNAL_RELOAD:
        sig = SIGUSR2;
        break;
    default:
        LOG_ERROR("Wrong signalType %d.", signalType);
        break;
    }

    if (sig != -1)
    {
        kill(pid, sig);
    }

    return true;

#else

    BOOST_STATIC_ASSERT(false);

#endif // TARGET_PLATFORM
}

// 打开配置文件
bool App::OpenConfigFile(bool reload)
{
    std::ifstream ifs(m_ConfigFile.c_str());
    if (!ifs)
    {
        if (reload)
        {
            LOG_ERROR("Can't open config file \"%s\".", m_ConfigFile.c_str());
        }
        else
        {
            fprintf(stderr, "Can't open config file \"%s\".\n", m_ConfigFile.c_str());
        }
        return false;
    }
    else
    {
        try
        {
            if (reload)
            {
                m_VariablesMap.clear();
            }

            boost::program_options::store(boost::program_options::parse_config_file(ifs, m_OptionsDesc), m_VariablesMap);
            boost::program_options::notify(m_VariablesMap);
        }
        catch (std::exception& e)
        {
            if (reload)
            {
                LOG_ERROR("%s", e.what());
            }
            else
            {
                fprintf(stderr, "%s\n", e.what());
            }
            return false;
        }
    }

    return true;
}

// 锁定存放进程 ID 的文件, 若成功则表示前一个进程存在
bool App::LockPidFile()
{
#if TARGET_PLATFORM == PLATFORM_WINDOWS

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE fileHandle = CreateFileA(m_PidFile.c_str(), GENERIC_READ | GENERIC_WRITE,  FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "CreateFile(\"%s\") failed, %s.\n", m_PidFile.c_str(), GetStrError()->c_str());
        exit(-1);
    }

    OVERLAPPED overlap;

    memset(&overlap, 0, sizeof(overlap));

    // 从4096字节开始加锁, 这样是为了保证后继写 pid 文件时, 能够在文件开始处写数据
    overlap.Offset = 4096;

    if (LockFileEx(fileHandle, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, 4096, 0, &overlap))
    {
        // NOTE: 这里故意不释放 fileHandle 是为了保持锁的存在, 进程退出时由操作系统回收
        return true;
    }
    else
    {
        CloseHandle(fileHandle);

        DWORD errorCode = GetLastError();
        if (errorCode != ERROR_LOCK_VIOLATION)
        {
            fprintf(stderr, "Failed to lock pid file: %s, %s\n", m_PidFile.c_str(), GetStrError(&errorCode)->c_str());
            exit(-1);
        }

        return false;
    }

#elif TARGET_PLATFORM == PLATFORM_LINUX

    int fileHandle = open(m_PidFile.c_str(), O_RDWR | O_CREAT, 0666);
    if (fileHandle < 0)
    {
        fprintf(stderr, "open(\"%s\") failed, errno=%d\n", m_PidFile.c_str(), errno);
        exit(-1);
    }

    int flags = fcntl(fileHandle, F_GETFD);
    if (fcntl(fileHandle,  F_SETFD, flags | FD_CLOEXEC) == -1)
    {

        fprintf(stderr, "Failed to set close-on-exec flag on file: %s, errno=%d\n", m_PidFile.c_str(), errno);
        exit(-1);
    }

    if (!flock(fileHandle, LOCK_EX | LOCK_NB))
    {
        // NOTE: 这里故意不释放 fileHandle 是为了保持锁的存在, 进程退出时由操作系统回收
        return true;
    }
    else
    {
        close(fileHandle);

        if (errno != EWOULDBLOCK)
        {
            fprintf(stderr, "Failed to lock pid file: %s, errno=%d\n", m_PidFile.c_str(), errno);
            exit(-1);
        }

        return false;
    }

#else

    BOOST_STATIC_ASSERT(false);

#endif // TARGET_PLATFORM
}

// 进程 ID 写到文件
bool App::WritePid(const std::string& pidFile)
{
    FILE* fp = fopen(pidFile.c_str(), "wt");

    if (!fp)
    {
        return false;
    }

    int pid = (int)getpid();

    fprintf(fp, "%d\n", pid);
    fflush(fp);
    fclose(fp);

    LOG_INFO("Write pid(%d) into file \"%s\".", pid, pidFile.c_str());

    return true;
}

// 从文件读进程 Id
pid_t App::ReadPid(const std::string& pidFile)
{
    int pid = -1;
    FILE* fp = fopen(pidFile.c_str(), "rt");

    if (fp)
    {
        fscanf(fp, "%d", &pid);
        fclose(fp);
    }

    return (pid_t)pid;
}

// 解析接收器的选项
void App::ParseAcceptorOptions(AcceptorProxy& acceptor, const std::string& ip, const std::string& port, const std::string& enable, bool reload)
{
    bool acceptorEnable = acceptor.IsEnable();
    std::string acceptorIp = acceptor.GetIp();
    int acceptorPort = acceptor.GetPort();

    // 是否启用接收器
    if (m_VariablesMap.count(enable))
    {
        bool newlyAcceptorEnable= m_VariablesMap[enable].as<bool>();
        LogChangedOption(port, boost::to_string(acceptorEnable), boost::to_string(newlyAcceptorEnable), reload);
        acceptorEnable = m_VariablesMap[enable].as<bool>();
    }

    // 接收器 IP
    if (m_VariablesMap.count(ip))
    {
        std::string newlyAcceptorIp = m_VariablesMap[ip].as<std::string>();
        LogChangedOption(port, boost::to_string(acceptorPort), boost::to_string(newlyAcceptorIp), reload);
        acceptorIp = newlyAcceptorIp;
    }

    // 接收器端口
    if (m_VariablesMap.count(port))
    {
        int acceptorPortNewly = m_VariablesMap[port].as<int>();
        if (acceptorPortNewly <= 0)
        {
            LOG_WARN("Bad %s %d.", port.c_str(), acceptorPortNewly);
        }
        else
        {
            LogChangedOption(port, boost::to_string(acceptorPort), boost::to_string(acceptorPortNewly), reload);
            acceptorPort = acceptorPortNewly;
        }
    }

    acceptor.Reset(acceptorIp, acceptorPort, acceptorEnable, reload);
}

// 解析连接器的选项
void App::ParseConnectorOptions(ConnectorProxy& connector, const std::string& ip, const std::string& port, const std::string& timeout, const std::string& enable, bool reload)
{
    std::string connectorIp = connector.GetIp();
    int connectorPort = connector.GetPort();
    int connectorTimeout = connector.GetTimeout();
    bool connectorEnable = connector.IsEnable();

    // 连接器 IP
    if (m_VariablesMap.count(ip))
    {
        std::string newlyConnectorIp = m_VariablesMap[ip].as<std::string>();
        LogChangedOption(port, boost::to_string(connectorPort), boost::to_string(newlyConnectorIp), reload);
        connectorIp = newlyConnectorIp;
    }

    // 连接器端口
    if (m_VariablesMap.count(port))
    {
        int connectorPortNewly = m_VariablesMap[port].as<int>();
        if (connectorPortNewly <= 0)
        {
            LOG_WARN("Bad %s %d.", port.c_str(), connectorPortNewly);
        }
        else
        {
            LogChangedOption(port, boost::to_string(connectorPort), boost::to_string(connectorPortNewly), reload);
            connectorPort = connectorPortNewly;
        }
    }

    // 连接器超时时间
    if (m_VariablesMap.count(timeout))
    {
        int connectorTimeoutNewly = m_VariablesMap[timeout].as<int>();
        if (connectorTimeoutNewly <= 0 && connectorTimeoutNewly != -1)
        {
            LOG_WARN("Bad %s %d.", timeout.c_str(), connectorTimeoutNewly);
        }
        else
        {
            LogChangedOption(timeout, boost::to_string(connectorTimeout), boost::to_string(connectorTimeoutNewly), reload);
            connectorTimeout = connectorTimeoutNewly;
        }
    }

    // 是否启用连接器
    if (m_VariablesMap.count(enable))
    {
        std::string newlyConnectorEnable= m_VariablesMap[enable].as<std::string>();
        LogChangedOption(port, boost::to_string(connectorEnable), boost::to_string(newlyConnectorEnable), reload);
        connectorEnable = m_VariablesMap[enable].as<bool>();
    }

    connector.Reset(connectorIp, connectorPort, connectorTimeout, connectorEnable, reload);
}

// 日志改变的选项
void App::LogChangedOption(const std::string& opt, const std::string& preValue, const std::string& newlyValue, bool reload)
{
    if (preValue != newlyValue && reload)
    {
        LOG_INFO("Reloading option|%s|%s|%s", opt.c_str(), preValue.c_str(), newlyValue.c_str());
    }
}
