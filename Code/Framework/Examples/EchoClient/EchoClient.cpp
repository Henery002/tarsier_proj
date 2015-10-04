#include "EchoClient/EchoClient.h"

// 例子应用程序实例
EchoClient* g_EchoClient = NULL;

// 例子应用程序
EchoClient::EchoClient()
    : m_Socket("127.0.0.1", ECHOCLIENT_DEFAULT_SERVICE_PORT)
    , m_RecvThread()
{
    g_EchoClient = this;

    m_AppName = "EchoClient";
}

EchoClient::~EchoClient()
{
    g_EchoClient = NULL;
}

// 接收线程
static void _RecvThread()
{
    LOG_TRACE("_RecvThread Start");

    while (!g_App->IsExitMainLoop())
    {
        if (g_EchoClient->GetSocket().GetSocketState() != SOCKET_STATE_ESTABLISHED)
        {
            usleep(1000);
            continue;
        }

        int len = 0;

        do
        {
            char recvbuf[4096] = { '0' };

            len = g_EchoClient->GetSocket().Recv(recvbuf, sizeof(recvbuf) - 1);

            if (len > 0)
            {
                LOG_INFO("recv|%s", recvbuf);
            }

        } while (len > 0);
    }

    LOG_TRACE("_RecvThread Stop");
}

// 初始化实现
bool EchoClient::InitImpl()
{
    m_Socket.Connect();

    m_RecvThread = boost::thread(_RecvThread);

    return true;
}

// 注册选项实现
bool EchoClient::RegisterOptionsImpl()
{
    m_OptionsDesc.add_options()
    ("echo-ip", boost::program_options::value<std::string>(), "Specify echo server ip.")
    ("echo-port", boost::program_options::value<int>(), "Specify echo server port.")
    ;

    return true;
}

// 解析选项实现
bool EchoClient::ParseOptionsImpl()
{
    return true;
}

// 解析可以重载的选项实现
bool EchoClient::ParseReloadableOptionsImpl(bool reload)
{
    std::string srvIp = m_Socket.GetIp();
    int srvPort = m_Socket.GetPort();

    if (m_VariablesMap.count("echo-ip"))
    {
        std::string newlySrvIp = m_VariablesMap["echo-ip"].as<std::string>();
        LogChangedOption("echo-ip", boost::to_string(srvIp), boost::to_string(newlySrvIp), reload);
        srvIp = newlySrvIp;
    }

    if (m_VariablesMap.count("echo-port"))
    {
        int newlySrvPort = m_VariablesMap["echo-port"].as<int>();
        LogChangedOption("echo-port", boost::to_string(srvPort), boost::to_string(newlySrvPort), reload);
        srvPort = newlySrvPort;
    }

    if (srvIp != m_Socket.GetIp() || srvPort != m_Socket.GetPort())
    {
        m_Socket.SetIp(srvIp);
        m_Socket.SetPort(srvPort);

        if (reload)
        {
            m_Socket.Connect();
        }
    }

    return true;
}

// 关闭实现
void EchoClient::ShutdownImpl()
{

}

// 更新实现
void EchoClient::UpdateImpl()
{

}

// 停止实现
void EchoClient::StopImpl()
{
    m_Socket.Close();

    m_RecvThread.join();
}

// 重载配置实现
void EchoClient::ReloadImpl()
{

}

// 处理输入实现
void EchoClient::HandleInputImpl(const std::string& input)
{
    if (!input.empty())
    {
        if (m_Socket.GetSocketState() != SOCKET_STATE_ESTABLISHED)
        {
            m_Socket.Connect();
        }

        if (m_Socket.GetSocketState() == SOCKET_STATE_ESTABLISHED)
        {
            m_Socket.Send(input.c_str(), (int)input.size());
        }
    }
}

// 套接字
SocketProxy& EchoClient::GetSocket()
{
    return m_Socket;
}
