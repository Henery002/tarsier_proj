#ifndef NET_IOCPCONNECTION_H_
#define NET_IOCPCONNECTION_H_

#include "Net/Connection.h"

// 完成端口连接
class NET_API IocpConnection : public Connection
{
public:
    IocpConnection();
    virtual ~IocpConnection();

    // 当分配
    virtual void OnAlloc();

    // 投递接收操作
    virtual void PostRecvOp();

    // 投递发送操作
    virtual void PostSendOp();

    // 获得接收器
    virtual Acceptor* GetAcceptor();

    // 获得完成端口上下文
    IocpOverlappedContext* GetIocpOverlappedContext(IOCP_OPERATION_TYPE opType);

    // 完成端口接收器
    IocpAcceptor* GetIocpAcceptor();
    void SetIocpAcceptor(IocpAcceptor* icopAcceptor);

    // 获得接收缓冲区
    char* GetRecvBuf();

protected:

    // 完成端口上下文
    IocpOverlappedContext m_IocpOverlappedContext[NUM_IOCP_OPERATION_TYPE];

    // 完成端口接收器
    IocpAcceptor* m_IocpAcceptor;

    // 接收缓冲区
    char m_RecvBuf[RECV_BUF_SIZE];
};

#endif // NET_IOCPCONNECTION_H_
