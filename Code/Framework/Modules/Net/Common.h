#ifndef NET_COMMON_H_
#define NET_COMMON_H_

#include "Core/Core.h"
#include "Net/NetProto.pb.h"

#include "boost/circular_buffer.hpp"

#if TARGET_PLATFORM == PLATFORM_WINDOWS

#include <WinSock2.h>
#include <MSWSock.h>

#pragma comment(lib, "ws2_32.lib")

#elif TARGET_PLATFORM == PLATFORM_LINUX

#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

// epoll_wait 最大事件数量, 参考 nginx
#define EPOLL_WAIT_MAXEVENTS 512

// epoll_wait 超时时间(毫秒), 参考 nginx
#define EPOLL_WAIT_TIMEOUT 500

#endif // TARGET_PLATFORM

// 是否生成为静态库
#define NET_STATIC 1

// 导出宏
#if TARGET_PLATFORM == PLATFORM_WINDOWS
#   ifdef NET_STATIC
#	    define NET_API
#   else
#	    ifdef NET_EXPORTS
#		    define NET_API __declspec(dllexport)
#	    else
#		    define NET_API __declspec(dllimport)
#	    endif // NET_EXPORTS
#   endif // NET_STATIC
#else
#	define NET_API
#endif // TARGET_PLATFORM

// 接收缓冲区大小
#define RECV_BUF_SIZE 4096

// 默认数据包接收队列容量
#define DEFAULT_RECV_PACKET_DATA_QUEUE_CAPACITY 100

// 默认数据包发送队列容量
#define DEFAULT_SEND_PACKET_DATA_QUEUE_CAPACITY 100

// 默认的最大接收连接数
#define DEFAULT_MAX_ACCPET_NUM 10

// select 超时时间(毫秒)
#define SELECT_WAIT_TIMEOUT 1

class Acceptor;
class Connector;
class Connection;
class SelectConnection;
class ProtoHandler;
class ProtoListener;

// 数据包信息
struct PacketData
{
    char* data;
    uint32_t size;
    uint32_t offset;

    PacketData()
        : data(NULL)
        , size(0)
        , offset(0)
    {

    }
};

typedef boost::circular_buffer_space_optimized<PacketData> PacketDataQueue;

#if TARGET_PLATFORM == PLATFORM_WINDOWS

class IocpAcceptor;
class IocpConnection;

// 完成端口操作类型
enum IOCP_OPERATION_TYPE
{
    IOCP_OPERATION_INVALID = -1,
    IOCP_OPERATION_ACCEPT, // 接收连接
    IOCP_OPERATION_RECV, // 接收数据
    IOCP_OPERATION_SEND, // 发送数据
    NUM_IOCP_OPERATION_TYPE
};

// 完成端口上下文
struct IocpOverlappedContext
{
    OVERLAPPED overlapped;
    IOCP_OPERATION_TYPE opType;
    IocpConnection* conn;

    IocpOverlappedContext()
        : overlapped()
        , opType(IOCP_OPERATION_INVALID)
        , conn(NULL)
    {

    }
};

#elif TARGET_PLATFORM == PLATFORM_LINUX

class EpollAcceptor;
class EpollConnection;

typedef int SOCKET;

#endif // TARGET_PLATFORM

#if TARGET_PLATFORM == PLATFORM_WINDOWS

// 获得套接字错误字符串
boost::shared_ptr<std::string> GetSockStrError(DWORD* errorCode = NULL);

// 设置非阻塞
void setnonblocking(SOCKET fd);

#elif TARGET_PLATFORM == PLATFORM_LINUX

// 获得套接字错误字符串
boost::shared_ptr<std::string> GetSockStrError(int* errorCode = NULL);

// 设置重用地址
void setreuseaddr(int fd);

// 设置保持存活
void setkeepalive(int fd);

// 设置非阻塞
void setnonblocking(int fd);

#endif // TARGET_PLATFORM

// 获得地址, IP:PORT
std::string GetAddr(const std::string& ip, int port);

// 获得套接字错误
int getsockerror(SOCKET sock);

// 获得套接字本地地址
sockaddr_in getlocalsockaddr(SOCKET sock);

#endif // NET_COMMON_H_
