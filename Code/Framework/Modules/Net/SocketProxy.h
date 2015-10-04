#ifndef NET_SOCKETPROXY_H_
#define NET_SOCKETPROXY_H_

#include "Net/Common.h"

// 套接字状态类型
enum SOCKET_STATE_TYPE
{
    SOCKET_STATE_INVALID = -1,
    SOCKET_STATE_CLOSED, // 已关闭
    SOCKET_STATE_LISTEN, // 监听
    SOCKET_STATE_ESTABLISHED, // 已连接
    NUM_SOCKET_STATE_TYPE
};

// 套接字代理
class NET_API SocketProxy
{
public:
    SocketProxy();
    SocketProxy(const std::string& ip, int port);
    ~SocketProxy();

    // 连接
    bool Connect();
    bool Connect(const std::string& ip, int port);

    // 发送
    void Send(const char* buf, int len);

    // 接收
    int Recv(char* buf, int len);

    // 创建
    SOCKET Create();

    // 关闭
    void Close();

    // IP
    const std::string& GetIp();
    void SetIp(const std::string ip);

    // 端口
    int GetPort();
    void SetPort(int port);

    // 套接字
    SOCKET GetSocket();

    // 套接字状态
    SOCKET_STATE_TYPE GetSocketState();

protected:

    // IP
    std::string m_Ip;

    // 端口
    int m_Port;

    // 套接字
    SOCKET m_Socket;

    // 套接字状态
    SOCKET_STATE_TYPE m_SocketState;
};

#endif // NET_SOCKETPROXY_H_
