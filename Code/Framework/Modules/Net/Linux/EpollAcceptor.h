#ifndef NET_EPOLLACCEPTOR_H_
#define NET_EPOLLACCEPTOR_H_

#include "Net/Acceptor.h"

// Epoll 接收器操作类型
enum EPOLL_ACCEPTOR_OPERATION_TYPE
{
    EPOLL_ACCEPTOR_OPERATION_INVALID = -1,
    EPOLL_ACCEPTOR_OPERATION_ACCEPT,
    EPOLL_ACCEPTOR_OPERATION_DISCONNECT,
    EPOLL_ACCEPTOR_OPERATION_RECV,
    NUM_EPOLL_ACCEPTOR_OPERATION_TYPE
};

// Epoll 接收器操作数据
struct EpollAcceptorOperationData
{
    EPOLL_ACCEPTOR_OPERATION_TYPE opType;
    EpollConnection* conn;
    char* data;
    uint32_t size;
    SOCKET sock;
    sockaddr_in addr;

    EpollAcceptorOperationData()
        : opType(EPOLL_ACCEPTOR_OPERATION_INVALID)
        , conn(NULL)
        , data(NULL)
        , size(0)
        , sock(INVALID_SOCKET)
        , addr()
    {

    }
};

typedef std::map<uint64_t, EpollConnection*> EpollConnectionMap;

// Epoll 接收器
class NET_API EpollAcceptor : public Acceptor
{
public:
    EpollAcceptor();
    virtual ~EpollAcceptor();

    // 启动
    virtual bool Start(const std::string& ip, int port);

    // 关闭
    virtual void Stop();

    // 更新
    virtual void Update();

    // 分配 Epoll 连接
    EpollConnection* AllocEpollConnection();

    // 释放 Epoll 连接
    void FreeEpollConnection(EpollConnection* conn);

    // 压入 Epoll 操作数据
    void PushEpollAcceptorOperation(const EpollAcceptorOperationData& opData);

    // 获得 Epoll 实例句柄
    int GetEpollFd();

    // 处理断开连接
    void HandleDisconnect(EpollConnection* conn);

protected:

    // 监听套接字
    SOCKET m_ListenSocket;

    // 本地地址
    sockaddr_in m_LocalAddr;

    // 工作线程
    boost::thread m_WorkerThread;

    // Epoll 实例句柄
    int m_EpollFd;

    // Epoll 事件列表
    epoll_event m_EpollEventList[EPOLL_WAIT_MAXEVENTS];

    // Epoll 操作队列
    std::vector<EpollAcceptorOperationData> m_EpollAcceptorOperactions;

    // Epoll 操作队列互斥体
    boost::mutex m_EpollAcceptorOperactionsMutex;

    // 工作中的 Epoll 连接
    EpollConnectionMap m_WorkingEpollConnections;

    // 空闲的 Epoll 连接
    std::stack<EpollConnection*> m_FreeEpollConnections;

    // Epoll 连接队列互斥体
    boost::mutex m_EpollConnectionsMutex;
};

#endif // NET_EPOLLACCEPTOR_H_
