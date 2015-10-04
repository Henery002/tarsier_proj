#ifndef NET_ACCEPTORPROXY_H_
#define NET_ACCEPTORPROXY_H_

#include "Net/Common.h"

// ����������
class NET_API AcceptorProxy
{
public:
    AcceptorProxy(const std::string& ip = "", int port = 0, bool enable = false);
    virtual ~AcceptorProxy();

    // ����������
    bool CreateAcceptor();

    // ���ٽ�����
    void DestroyAcceptor(bool stop = true);

    // ����������
    bool RestartAcceptor();

    // �Ƿ����ý�����
    bool IsEnable();
    void SetEnable(bool enable);

    // ������
    Acceptor* GetAcceptor();

    // ������ IP
    const std::string& GetIp();
    void SetIp(const std::string& ip);

    // �������˿�
    int GetPort();
    void SetPort(int port);

    // ����
    void Reset(const std::string& ip, int port, bool enable, bool reload);

protected:

    // �Ƿ����ý�����
    bool m_Enable;

    // ������
    Acceptor* m_Acceptor;

    // ������ IP
    std::string m_Ip;

    // �������˿�
    int m_Port;
};

#endif // NET_ACCEPTORPROXY_H_
