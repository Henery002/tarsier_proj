#include "Net/AcceptorProxy.h"
#include "Net/Acceptor.h"
#include "Net/Net.h"

// 接收器代理
AcceptorProxy::AcceptorProxy(const std::string& ip, int port, bool enable)
    : m_Enable(enable)
    , m_Acceptor(NULL)
    , m_Ip(ip)
    , m_Port(port)
{

}

AcceptorProxy::~AcceptorProxy()
{
    BOOST_ASSERT(!m_Acceptor);
}

// 创建接收器
bool AcceptorProxy::CreateAcceptor()
{
    BOOST_ASSERT(!m_Acceptor);

    m_Acceptor = Net::Inst()->CreateAcceptor();

    if (m_Acceptor->Start(m_Ip, m_Port))
    {
        return true;
    }
    else
    {
        DestroyAcceptor(false);
        return false;
    }
}

// 销毁接收器
void AcceptorProxy::DestroyAcceptor(bool stop)
{
    if (m_Acceptor)
    {
        Net::Inst()->DestroyAcceptor(m_Acceptor, stop);
        m_Acceptor = NULL;
    }
}

// 重启接收器
bool AcceptorProxy::RestartAcceptor()
{
    if (m_Acceptor)
    {
        m_Acceptor->Stop();
        return m_Acceptor->Start(m_Ip, m_Port);
    }
    else
    {
        return CreateAcceptor();
    }
}

// 是否启用接收器
bool AcceptorProxy::IsEnable()
{
    return m_Enable;
}

void AcceptorProxy::SetEnable(bool enable)
{
    m_Enable = enable;
}

// 接收器
Acceptor* AcceptorProxy::GetAcceptor()
{
    return m_Acceptor;
}

// 接收器 IP
const std::string& AcceptorProxy::GetIp()
{
    return m_Ip;
}

void AcceptorProxy::SetIp(const std::string& ip)
{
    m_Ip = ip;
}

// 接收器端口
int AcceptorProxy::GetPort()
{
    return m_Port;
}

void AcceptorProxy::SetPort(int port)
{
    m_Port = port;
}

// 重置
void AcceptorProxy::Reset(const std::string& ip, int port, bool enable, bool reload)
{
    if (ip != m_Ip || port != m_Port || enable != m_Enable)
    {
        SetIp(ip);
        SetPort(port);
        SetEnable(enable);

        if (reload)
        {
            if (m_Enable)
            {
                RestartAcceptor();
            }
            else if (m_Acceptor)
            {
                m_Acceptor->Stop();
            }
        }
        else if (m_Acceptor)
        {
            RestartAcceptor();
        }
    }
}
