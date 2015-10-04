#ifndef NET_IOCPACCEPTOR_H_
#define NET_IOCPACCEPTOR_H_

#include "Net/Acceptor.h"

// 完成端口接收器操作类型
enum IOCP_ACCEPTOR_OPERATION_TYPE
{
    IOCP_ACCEPTOR_OPERATION_INVALID = -1,
    IOCP_ACCEPTOR_OPERATION_ACCEPT, // 接收连接
    IOCP_ACCEPTOR_OPERATION_DISCONNECT, // 断开连接
    IOCP_ACCEPTOR_OPERATION_RECV, // 接收数据
    NUM_IOCP_ACCEPTOR_OPERATION_TYPE
};

// 完成端口接收器操作数据
struct IocpAcceptorOperationData
{
    IOCP_ACCEPTOR_OPERATION_TYPE opType;
    IocpConnection* conn;
    char* data;
    uint32_t size;

    IocpAcceptorOperationData()
        : opType(IOCP_ACCEPTOR_OPERATION_INVALID)
        , conn(NULL)
        , data(NULL)
        , size(0)
    {

    }
};

typedef std::map<uint64_t, IocpConnection*> IocpConnectionMap;

// 完成端口接收器
class NET_API IocpAcceptor : public Acceptor
{
public:
    IocpAcceptor();
    virtual ~IocpAcceptor();

    // 启动
    virtual bool Start(const std::string& ip, int port);

    // 关闭
    virtual void Stop();

    // 更新
    virtual void Update();

    // 投递接收操作
    void PostAcceptOp();

    // 分配完成端口连接
    IocpConnection* AllocIocpConnection();

    // 释放完成端口连接
    void FreeIocpConnection(IocpConnection* conn);

    // 压入完成端口操作数据
    void PushIocpAcceptorOperation(const IocpAcceptorOperationData& opData);

protected:

    // 监听套接字
    SOCKET m_ListenSocket;

    // AcceptEx 函数指针
    LPFN_ACCEPTEX m_AcceptEx;

    // GetAcceptExSockaddrs 函数指针
    LPFN_GETACCEPTEXSOCKADDRS m_GetAcceptExSockaddrs;

    // 完成端口句柄
    HANDLE m_IocpHandle;

    // 工作线程数组
    std::vector<boost::thread> m_WorkerThreads;

    // 工作中的连接
    IocpConnectionMap m_WorkingIocpConnections;

    // 空闲的完成端口连接
    std::stack<IocpConnection*> m_FreeIocpConnections;

    // 完成端口操作队列
    std::vector<IocpAcceptorOperationData> m_IocpAcceptorOperations;

    // 完成端口操作队列互斥体
    boost::mutex m_IocpAcceptorOperationsMutex;
};

#endif // NET_IOCPACCEPTOR_H_
