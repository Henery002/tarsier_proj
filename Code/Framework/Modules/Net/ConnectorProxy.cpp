#include "Net/ConnectorProxy.h"
#include "Net/Connector.h"
#include "Net/Net.h"

// 连接器代理
ConnectorProxy::ConnectorProxy(const std::string& ip, int port, int timeout, bool enable)
    : m_Enable(enable)
    , m_Connector(NULL)
    , m_Ip(ip)
    , m_Timeout(-1)
    , m_Port(port)
{

}

ConnectorProxy::~ConnectorProxy()
{
    BOOST_ASSERT(!m_Connector);
}

// 创建连接器
bool ConnectorProxy::CreateConnector()
{
    BOOST_ASSERT(!m_Connector);

    m_Connector = Net::Inst()->CreateConnector();
    m_Connector->Connect(m_Ip, m_Port, m_Timeout);

    return true;
}

// 销毁连接器
void ConnectorProxy::DestroyConnector(bool distconn)
{
    if (m_Connector)
    {
        Net::Inst()->DestroyConnector(m_Connector, distconn);
        m_Connector = NULL;
    }
}

// 重启连接器
bool ConnectorProxy::RestartConnector()
{
    if (m_Connector)
    {
        m_Connector->Disconnect();
        m_Connector->Connect(m_Ip, m_Port, m_Timeout);
        return true;
    }
    else
    {
        return CreateConnector();
    }
}

// 是否启用连接器
bool ConnectorProxy::IsEnable()
{
    return m_Enable;
}

void ConnectorProxy::SetEnable(bool enable)
{
    m_Enable = enable;
}

// 连接器
Connector* ConnectorProxy::GetConnector()
{
    return m_Connector;
}

// 连接器 IP
const std::string& ConnectorProxy::GetIp()
{
    return m_Ip;
}

void ConnectorProxy::SetIp(const std::string& ip)
{
    m_Ip = ip;
}

// 连接器端口
int ConnectorProxy::GetPort()
{
    return m_Port;
}

void ConnectorProxy::SetPort(int port)
{
    m_Port = port;
}

// 超时时间
int ConnectorProxy::GetTimeout()
{
    return m_Timeout;
}

void ConnectorProxy::SetTimeout(int timeout)
{
    m_Timeout = timeout;
}

// 重置
void ConnectorProxy::Reset(const std::string& ip, int port, int timeout, bool enable, bool reload)
{
    if (ip != m_Ip || port != m_Port || timeout != m_Timeout || enable != m_Enable)
    {
        SetIp(ip);
        SetPort(port);
        SetTimeout(timeout);
        SetEnable(enable);

        if (reload)
        {
            if (m_Enable)
            {
                RestartConnector();
            }
            else if (m_Connector)
            {
                m_Connector->Disconnect();
            }
        }
        else if (m_Connector)
        {
            RestartConnector();
        }
    }
}
