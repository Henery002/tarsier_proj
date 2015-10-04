#include "Net/SocketProxy.h"

// 套接字代理
SocketProxy::SocketProxy()
    : m_Ip("127.0.0.1")
    , m_Port(0)
    , m_Socket(INVALID_SOCKET)
    , m_SocketState(SOCKET_STATE_INVALID)
{

}

SocketProxy::SocketProxy(const std::string& ip, int port)
    : m_Ip(ip)
    , m_Port(port)
    , m_Socket(INVALID_SOCKET)
    , m_SocketState(SOCKET_STATE_INVALID)
{

}

SocketProxy::~SocketProxy()
{

}

// 连接
bool SocketProxy::Connect()
{
    return Connect(m_Ip, m_Port);
}

bool SocketProxy::Connect(const std::string& ip, int port)
{
    Close();
    Create();

    m_Ip = ip;
    m_Port = port;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    LOG_TRACE("Connecting|%s", GetAddr(ip, port).c_str());

    if (connect(m_Socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        LOG_ERROR("connect() failed, %s", GetSockStrError()->c_str());

        Close();

        return false;
    }
    else
    {
        m_SocketState = SOCKET_STATE_ESTABLISHED;

        LOG_TRACE("Connected|%s", GetAddr(ip, port).c_str());

        return true;
    }
}

// 发送
void SocketProxy::Send(const char* buf, int len)
{
    BOOST_ASSERT(m_Socket != INVALID_SOCKET);

    if (send(m_Socket, buf, len, 0) == SOCKET_ERROR)
    {
        LOG_ERROR("send() failed, %s", GetSockStrError()->c_str());
    }
}

// 接收
int SocketProxy::Recv(char* buf, int len)
{
    BOOST_ASSERT(m_Socket != INVALID_SOCKET);

    len = recv(m_Socket, buf, len, 0);

    if (len > 0)
    {

    }
    else if (len == 0)
    {
        LOG_TRACE("Connection closed.");

        Close();
    }
    else
    {
#if TARGET_PLATFORM == PLATFORM_WINDOWS

        DWORD errorCode = WSAGetLastError();
        switch (errorCode)
        {
        case WSAECONNABORTED:
            LOG_TRACE("Connection closed by local.");
            break;
        case WSAECONNRESET:
            LOG_TRACE("Connection closed by remote.");
            break;
        default:
            LOG_ERROR("recv() failed, %s", GetSockStrError(&errorCode)->c_str());
            break;
        }

#elif TARGET_PLATFORM == PLATFORM_LINUX

        switch (errno)
        {
        case ECONNRESET:
            LOG_TRACE("Connection closed by remote.");
            break;
        default:
            LOG_ERROR("recv() failed, %s", GetSockStrError(&errno)->c_str());
            break;
        }

#endif // TARGET_PLATFORM

        Close();
    }

    return len;
}

// 创建
SOCKET SocketProxy::Create()
{
    int protocol = 0;

#if TARGET_PLATFORM == PLATFORM_WINDOWS
    protocol = IPPROTO_TCP;
#endif // TARGET_PLATFORM

    m_Socket = socket(AF_INET, SOCK_STREAM, protocol);

    if (m_Socket == INVALID_SOCKET)
    {
        LOG_ERROR("socket() failed, %s", GetSockStrError()->c_str());
    }

    return m_Socket;
}

// 关闭
void SocketProxy::Close()
{
    if (m_Socket != INVALID_SOCKET)
    {
#if TARGET_PLATFORM == PLATFORM_WINDOWS
        closesocket(m_Socket);
#elif TARGET_PLATFORM == PLATFORM_LINUX
        // NOTE: 不调用 shutdown 则无法唤醒其他线程里阻塞的 recv
        // http://stackoverflow.com/questions/3589723/can-a-socket-be-closed-from-another-thread-when-a-send-recv-on-the-same-socket?rq=1
        shutdown(m_Socket, SHUT_RDWR);
        close(m_Socket);
#else
        BOOST_STATIC_ASSERT(false);
#endif // TARGET_PLATFORM
        m_Socket = INVALID_SOCKET;
    }

    m_SocketState = SOCKET_STATE_INVALID;
}

// IP
const std::string& SocketProxy::GetIp()
{
    return m_Ip;
}

void SocketProxy::SetIp(const std::string ip)
{
    m_Ip = ip;
}

// 端口
int SocketProxy::GetPort()
{
    return m_Port;
}

void SocketProxy::SetPort(int port)
{
    m_Port = port;
}

// 套接字
SOCKET SocketProxy::GetSocket()
{
    return m_Socket;
}

// 套接字状态
SOCKET_STATE_TYPE SocketProxy::GetSocketState()
{
    return m_SocketState;
}
