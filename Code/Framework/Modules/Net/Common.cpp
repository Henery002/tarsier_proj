#include "Net/Common.h"

#if TARGET_PLATFORM == PLATFORM_WINDOWS

// 获得套接字错误字符串
boost::shared_ptr<std::string> GetSockStrError(DWORD* errorCode)
{
    char buf1[4096];
    char buf2[4096];

    DWORD curErrorCode = WSAGetLastError();
    if (errorCode)
    {
        curErrorCode = *errorCode;
    }

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, curErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf1, sizeof(buf1), NULL);

    // 去掉 \r\n
    int len = strlen(buf1);
    if (len >= 2 && buf1[len - 2] == '\r' && buf1[len - 1] == '\n')
    {
        buf1[len - 2] = '\0';
    }

    len = snprintf(buf2, sizeof(buf2) - 1, "ErrorCode=%u, ErrorString=%s", curErrorCode, buf1);
    buf2[len] = '\0';

    return boost::shared_ptr<std::string>(new std::string(&buf2[0]));
}

// 设置非阻塞
void setnonblocking(SOCKET fd)
{
    u_long nonblocking = 1;
    if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR)
    {
        LOG_ERROR("ioctlsocket() failed, %s", GetSockStrError()->c_str());
    }
}

#elif TARGET_PLATFORM == PLATFORM_LINUX

// 获得套接字错误字符串
boost::shared_ptr<std::string> GetSockStrError(int* errorCode)
{
    return GetStrError(errorCode);
}

// 设置重用地址
void setreuseaddr(int fd)
{
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(reuse)) == -1)
    {
        LOG_ERROR("setsockopt() failed, %s", GetSockStrError()->c_str());
    }
}

// 设置保持存活
void setkeepalive(int fd)
{
    int keepalive = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepalive, sizeof(keepalive)) == -1)
    {
        LOG_ERROR("setsockopt() failed, %s", GetSockStrError()->c_str());
    }
}

// 设置非阻塞
void setnonblocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
    {
        LOG_ERROR("fcntl() failed, %s", GetSockStrError()->c_str());
    }
    else
    {
        if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) == -1)
        {
            LOG_ERROR("fcntl() failed, %s", GetSockStrError()->c_str());
        }
    }
}

#endif // TARGET_PLATFORM

// 获得地址, IP:PORT
std::string GetAddr(const std::string& ip, int port)
{
    char buf[64] = { '\0' };
    snprintf(buf, sizeof(buf) - 1, "%s:%d", ip.c_str(), port);
    return buf;
}

// 获得套接字错误
int getsockerror(SOCKET sock)
{
    int err = 0;
#if TARGET_PLATFORM == PLATFORM_WINDOWS
    int len = sizeof(len);
#elif TARGET_PLATFORM == PLATFORM_LINUX
    socklen_t len = sizeof(len);
#endif // TARGET_PLATFORM

    if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len) == SOCKET_ERROR)
    {
        LOG_ERROR("getsockopt() failed, %s", GetSockStrError()->c_str());
    }

    return err;
}

// 获得套接字本地地址
sockaddr_in getlocalsockaddr(SOCKET sock)
{
    sockaddr_in addr;
#if TARGET_PLATFORM == PLATFORM_WINDOWS
    int addrLen = sizeof(addr);
#elif TARGET_PLATFORM == PLATFORM_LINUX
    socklen_t addrLen = sizeof(addr);
#endif // TARGET_PLATFORM

    if (getsockname(sock, (sockaddr*)&addr, &addrLen) == SOCKET_ERROR)
    {
        LOG_ERROR("getsockname() failed, %s", GetSockStrError()->c_str());
    }

    return addr;
}
