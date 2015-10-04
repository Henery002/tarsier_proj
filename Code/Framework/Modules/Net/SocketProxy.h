#ifndef NET_SOCKETPROXY_H_
#define NET_SOCKETPROXY_H_

#include "Net/Common.h"

// �׽���״̬����
enum SOCKET_STATE_TYPE
{
    SOCKET_STATE_INVALID = -1,
    SOCKET_STATE_CLOSED, // �ѹر�
    SOCKET_STATE_LISTEN, // ����
    SOCKET_STATE_ESTABLISHED, // ������
    NUM_SOCKET_STATE_TYPE
};

// �׽��ִ���
class NET_API SocketProxy
{
public:
    SocketProxy();
    SocketProxy(const std::string& ip, int port);
    ~SocketProxy();

    // ����
    bool Connect();
    bool Connect(const std::string& ip, int port);

    // ����
    void Send(const char* buf, int len);

    // ����
    int Recv(char* buf, int len);

    // ����
    SOCKET Create();

    // �ر�
    void Close();

    // IP
    const std::string& GetIp();
    void SetIp(const std::string ip);

    // �˿�
    int GetPort();
    void SetPort(int port);

    // �׽���
    SOCKET GetSocket();

    // �׽���״̬
    SOCKET_STATE_TYPE GetSocketState();

protected:

    // IP
    std::string m_Ip;

    // �˿�
    int m_Port;

    // �׽���
    SOCKET m_Socket;

    // �׽���״̬
    SOCKET_STATE_TYPE m_SocketState;
};

#endif // NET_SOCKETPROXY_H_
