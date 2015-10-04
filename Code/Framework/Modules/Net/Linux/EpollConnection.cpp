#include "Net/Linux/EpollConnection.h"
#include "Net/Linux/EpollAcceptor.h"

// Epoll 连接
EpollConnection::EpollConnection()
    : m_EpollAcceptor(NULL)
{

}

EpollConnection::~EpollConnection()
{

}

// 断开连接, NOTE: 会保证全部数据发送完成才真正断开
void EpollConnection::Disconnect()
{
    BOOST_ASSERT(m_EpollAcceptor);

    CONNECTION_STATE_TYPE preConnState = m_ConnState;

    Connection::Disconnect();

    if (preConnState != m_ConnState && m_ConnState == CONNECTION_STATE_CLOSED)
    {
        m_EpollAcceptor->HandleDisconnect(this);
    }
}

// 当完成发送数据
void EpollConnection::OnSendComplete()
{
    BOOST_ASSERT(m_Socket != INVALID_SOCKET);
    BOOST_ASSERT(m_EpollAcceptor);

    Connection::OnSendComplete();

    epoll_event ev;

    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.ptr = this;

    if (epoll_ctl(m_EpollAcceptor->GetEpollFd(), EPOLL_CTL_MOD, m_Socket, &ev) == -1)
    {
        LOG_ERROR("epoll_ctl() failed, %s", GetSockStrError()->c_str());
    }

    if (m_ConnState == CONNECTION_STATE_CLOSE_PENDING)
    {
        EpollAcceptorOperationData opData;
        opData.opType = EPOLL_ACCEPTOR_OPERATION_DISCONNECT;
        opData.conn = this;
        m_EpollAcceptor->PushEpollAcceptorOperation(opData);
    }
}

// 投递接收操作
void EpollConnection::PostRecvOp()
{
    if (m_ConnState == CONNECTION_STATE_ESTABLISHED)
    {
        BOOST_ASSERT(m_Socket != INVALID_SOCKET);

        Connection::PostRecvOp();

        epoll_event ev;

        ev.events = EPOLLIN;
        ev.data.ptr = this;

        if (epoll_ctl(m_EpollAcceptor->GetEpollFd(), EPOLL_CTL_ADD, m_Socket, &ev) == -1)
        {
            LOG_ERROR("epoll_ctl() failed, %s", GetSockStrError()->c_str());
        }
    }
}

// 投递发送操作
void EpollConnection::PostSendOp()
{
    BOOST_ASSERT(m_Socket != INVALID_SOCKET);

    Connection::PostSendOp();

    epoll_event ev;

    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.ptr = this;

    if (epoll_ctl(m_EpollAcceptor->GetEpollFd(), EPOLL_CTL_MOD, m_Socket, &ev) == -1)
    {
        LOG_ERROR("epoll_ctl() failed, %s", GetSockStrError()->c_str());
    }
}

// 获得接收器
Acceptor* EpollConnection::GetAcceptor()
{
    return m_EpollAcceptor;
}

// Epoll 接收器
EpollAcceptor* EpollConnection::GetEpollAcceptor()
{
    return m_EpollAcceptor;
}

void EpollConnection::SetEpollAcceptor(EpollAcceptor* epollAcceptor)
{
    m_EpollAcceptor = epollAcceptor;
}
