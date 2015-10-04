#ifndef NET_EPOLLCONNECTION_H_
#define NET_EPOLLCONNECTION_H_

#include "Net/Connection.h"

// Epoll 连接
class NET_API EpollConnection : public Connection
{
public:
    EpollConnection();
    virtual ~EpollConnection();

    // 断开连接, NOTE: 会保证全部数据发送完成才真正断开
    virtual void Disconnect();

    // 当完成发送数据
    virtual void OnSendComplete();

    // 投递接收操作
    virtual void PostRecvOp();

    // 投递发送操作
    virtual void PostSendOp();

    // 获得接收器
    virtual Acceptor* GetAcceptor();

    // Epoll 接收器
    EpollAcceptor* GetEpollAcceptor();
    void SetEpollAcceptor(EpollAcceptor* epollAcceptor);

protected:

    // Epoll 接收器
    EpollAcceptor* m_EpollAcceptor;
};

#endif // NET_EPOLLCONNECTION_H_
