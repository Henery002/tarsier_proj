#include "EchoServer/EchoServer.h"

// 回显服务器实例
EchoServer* g_EchoServer = NULL;

// 回显服务器
EchoServer::EchoServer()
    : m_EchoAcceptor("0.0.0.0", ECHOSERVER_DEFAULT_SERVICE_PORT, true)
{
    g_EchoServer = this;

    m_AppName = "EchoServer";
    m_BackdoorAcceptor.SetPort(ECHOSERVER_DEFAULT_BACKDOOR_PORT);
}

EchoServer::~EchoServer()
{
    g_EchoServer = NULL;
}

// 初始化实现
bool EchoServer::InitImpl()
{
    if (m_EchoAcceptor.IsEnable())
    {
        if (!m_EchoAcceptor.CreateAcceptor())
        {
            return false;
        }

        m_EchoAcceptor.GetAcceptor()->SetAcceptorListener(&m_EchoAcceptor);
    }

    return true;
}

// 注册选项实现
bool EchoServer::RegisterOptionsImpl()
{
    m_OptionsDesc.add_options()
    ("echo-ip", boost::program_options::value<std::string>(), "Specify echo bind ip.")
    ("echo-port", boost::program_options::value<int>(), "Specify echo bind port.")
    ("echo-enable", boost::program_options::value<bool>(), "Specify echo is enabled.")
    ;

    return true;
}

// 解析选项实现
bool EchoServer::ParseOptionsImpl()
{
    return true;
}

// 解析可以重载的选项实现
bool EchoServer::ParseReloadableOptionsImpl(bool reload)
{
    // 回显接收器
    ParseAcceptorOptions(m_EchoAcceptor, "echo-ip", "echo-port", "echo-enable", reload);

    return true;
}

// 关闭实现
void EchoServer::ShutdownImpl()
{

}

// 更新实现
void EchoServer::UpdateImpl()
{

}

// 停止实现
void EchoServer::StopImpl()
{
    m_EchoAcceptor.DestroyAcceptor();
}

// 重载配置实现
void EchoServer::ReloadImpl()
{

}
