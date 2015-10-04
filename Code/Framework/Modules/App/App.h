#ifndef APP_APP_H_
#define APP_APP_H_

#include "App/Common.h"
#include "App/BackdoorAcceptor.h"

// 应用程序
class APP_API App
{
public:
    App();
    virtual ~App();

    // 初始化
    bool Init(int argc, char* argv[]);

    // 初始化实现
    virtual bool InitImpl();

    // 注册选项
    bool RegisterOptions(int argc, char* argv[]);

    // 注册选项实现
    virtual bool RegisterOptionsImpl();

    // 解析选项
    bool ParseOptions();

    // 解析选项实现
    virtual bool ParseOptionsImpl();

    // 解析可以重载的选项
    bool ParseReloadableOptions(bool reload);

    // 解析可以重载的选项实现
    virtual bool ParseReloadableOptionsImpl(bool reload);

    // 关闭
    void Shutdown();

    // 关闭实现
    virtual void ShutdownImpl();

    // 运行
    void Run();

    // 更新
    void Update();

    // 更新实现
    virtual void UpdateImpl();

    // 停止
    void Stop();

    // 停止实现
    virtual void StopImpl();

    // 重载配置
    void Reload();

    // 重载配置实现
    virtual void ReloadImpl();

    // 刷新缓冲区
    void Flush();

    // 刷新缓冲区实现
    virtual void FlushImpl();

    // 增加输入
    void AddInput(const std::string& input);

    // 更新输入
    void UpdateInput();

    // 处理输入
    void HandleInput(const std::string& input);

    // 处理输入实现
    virtual void HandleInputImpl(const std::string& input);

    // 是否退出主循环
    bool IsExitMainLoop();

    // 收到停止信号
    void OnStopSignal();

    // 收到重载配置信号
    void OnReloadSignal();

    // 输出版本信息
    virtual void OutputVersion();

    // 输出输入帮助
    virtual void OutputInputHelp();

    // 销毁自己
    virtual void DestroySelf();

protected:

    // 创建信号
    bool CreateSignals();

    // 发送信号
    bool SendSignal(SIGNAL_TYPE signalType);

    // 打开配置文件
    bool OpenConfigFile(bool reload);

    // 锁定存放进程 ID 的文件, 若成功则表示前一个进程存在
    bool LockPidFile();

    // 进程 ID 写到文件
    bool WritePid(const std::string& pidFile);

    // 从文件读进程 ID
    pid_t ReadPid(const std::string& pidFile);

    // 解析接收器的选项
    void ParseAcceptorOptions(AcceptorProxy& acceptor, const std::string& ip, const std::string& port, const std::string& enable, bool reload);

    // 解析连接器的选项
    void ParseConnectorOptions(ConnectorProxy& connector, const std::string& ip, const std::string& port, const std::string& timeout, const std::string& enable, bool reload);

    // 日志改变的选项
    void LogChangedOption(const std::string& opt, const std::string& preValue, const std::string& newlyValue, bool reload);

protected:

    // 是否退出主循环
    bool m_ExitMainLoop;

    // 是否守护进程
    bool m_Daemon;

    // 信号类型
    SIGNAL_TYPE m_SignalType;

    // 空闲计数
    int m_IdleCount;

    // 当前空闲计数
    int m_CurIdleCount;

    // 空闲挂起时间
    int m_IdleSleep;

    // 总的挂起次数
    uint64_t m_TotalSleepNum;

    // 选项描述
    boost::program_options::options_description m_OptionsDesc;

    // 变量索引表
    boost::program_options::variables_map m_VariablesMap;

    // 版本号
    std::string m_Version;

    // 应用程序名称
    std::string m_AppName;

    // 配置文件
    std::string m_ConfigFile;

    // 存放进程 ID 的文件
    std::string m_PidFile;

    // 日志文件
    std::string m_LogFile;

    // 执行文件所在目录
    std::string m_ExeDir;

    // 输入互斥体
    boost::mutex m_InputMutex;

    // 输入队列
    std::queue<std::string> m_InputQueue;

    // 后门接收器
    BackdoorAcceptor m_BackdoorAcceptor;
};

// 应用程序实例
extern App* g_App;

#endif // APP_APP_H_
