#ifndef NET_CONNECTORPROXY_H_
#define NET_CONNECTORPROXY_H_

#include "Net/Common.h"

// 连接器代理
class NET_API ConnectorProxy
{
public:
    ConnectorProxy(const std::string& ip = "", int port = 0, int timeout = -1, bool enable = false);
    virtual ~ConnectorProxy();

    // 创建连接器
    bool CreateConnector();

    // 销毁连接器
    void DestroyConnector(bool distconn = true);

    // 重启连接器
    bool RestartConnector();

    // 是否启用连接器
    bool IsEnable();
    void SetEnable(bool enable);

    // 连接器
    Connector* GetConnector();

    // 连接器 IP
    const std::string& GetIp();
    void SetIp(const std::string& ip);

    // 连接器端口
    int GetPort();
    void SetPort(int port);

    // 超时时间
    int GetTimeout();
    void SetTimeout(int timeout);

    // 重置
    void Reset(const std::string& ip, int port, int timeout, bool enable, bool reload);

protected:

    // 是否启用连接器
    bool m_Enable;

    // 连接器
    Connector* m_Connector;

    // 连接器 IP
    std::string m_Ip;

    // 连接器端口
    int m_Port;

    // 超时时间
    int m_Timeout;
};

#endif // NET_CONNECTORPROXY_H_
