#include "EchoServer/EchoServer.h"

// ���Է�����ʵ��
EchoServer* g_EchoServer = NULL;

// ���Է�����
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

// ��ʼ��ʵ��
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

// ע��ѡ��ʵ��
bool EchoServer::RegisterOptionsImpl()
{
    m_OptionsDesc.add_options()
    ("echo-ip", boost::program_options::value<std::string>(), "Specify echo bind ip.")
    ("echo-port", boost::program_options::value<int>(), "Specify echo bind port.")
    ("echo-enable", boost::program_options::value<bool>(), "Specify echo is enabled.")
    ;

    return true;
}

// ����ѡ��ʵ��
bool EchoServer::ParseOptionsImpl()
{
    return true;
}

// �����������ص�ѡ��ʵ��
bool EchoServer::ParseReloadableOptionsImpl(bool reload)
{
    // ���Խ�����
    ParseAcceptorOptions(m_EchoAcceptor, "echo-ip", "echo-port", "echo-enable", reload);

    return true;
}

// �ر�ʵ��
void EchoServer::ShutdownImpl()
{

}

// ����ʵ��
void EchoServer::UpdateImpl()
{

}

// ֹͣʵ��
void EchoServer::StopImpl()
{
    m_EchoAcceptor.DestroyAcceptor();
}

// ��������ʵ��
void EchoServer::ReloadImpl()
{

}
