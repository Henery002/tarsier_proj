#ifndef NET_CONNECTORPROXY_H_
#define NET_CONNECTORPROXY_H_

#include "Net/Common.h"

// ����������
class NET_API ConnectorProxy
{
public:
    ConnectorProxy(const std::string& ip = "", int port = 0, int timeout = -1, bool enable = false);
    virtual ~ConnectorProxy();

    // ����������
    bool CreateConnector();

    // ����������
    void DestroyConnector(bool distconn = true);

    // ����������
    bool RestartConnector();

    // �Ƿ�����������
    bool IsEnable();
    void SetEnable(bool enable);

    // ������
    Connector* GetConnector();

    // ������ IP
    const std::string& GetIp();
    void SetIp(const std::string& ip);

    // �������˿�
    int GetPort();
    void SetPort(int port);

    // ��ʱʱ��
    int GetTimeout();
    void SetTimeout(int timeout);

    // ����
    void Reset(const std::string& ip, int port, int timeout, bool enable, bool reload);

protected:

    // �Ƿ�����������
    bool m_Enable;

    // ������
    Connector* m_Connector;

    // ������ IP
    std::string m_Ip;

    // �������˿�
    int m_Port;

    // ��ʱʱ��
    int m_Timeout;
};

#endif // NET_CONNECTORPROXY_H_
