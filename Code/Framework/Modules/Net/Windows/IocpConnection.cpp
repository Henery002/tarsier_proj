#include "Net/Windows/IocpConnection.h"
#include "Net/Windows/IocpAcceptor.h"

// 完成端口连接
IocpConnection::IocpConnection()
    : m_IocpAcceptor(NULL)
{

}

IocpConnection::~IocpConnection()
{

}

// 当分配
void IocpConnection::OnAlloc()
{
    Connection::OnAlloc();

    m_IocpOverlappedContext[IOCP_OPERATION_ACCEPT].conn = this;
    m_IocpOverlappedContext[IOCP_OPERATION_ACCEPT].opType = IOCP_OPERATION_ACCEPT;
    memset(&m_IocpOverlappedContext[IOCP_OPERATION_ACCEPT].overlapped, 0, sizeof(m_IocpOverlappedContext[IOCP_OPERATION_ACCEPT].overlapped));

    m_IocpOverlappedContext[IOCP_OPERATION_RECV].conn = this;
    m_IocpOverlappedContext[IOCP_OPERATION_RECV].opType = IOCP_OPERATION_RECV;
    memset(&m_IocpOverlappedContext[IOCP_OPERATION_RECV].overlapped, 0, sizeof(m_IocpOverlappedContext[IOCP_OPERATION_RECV].overlapped));

    m_IocpOverlappedContext[IOCP_OPERATION_SEND].conn = this;
    m_IocpOverlappedContext[IOCP_OPERATION_SEND].opType = IOCP_OPERATION_SEND;
    memset(&m_IocpOverlappedContext[IOCP_OPERATION_SEND].overlapped, 0, sizeof(m_IocpOverlappedContext[IOCP_OPERATION_SEND].overlapped));
}

// 投递接收操作
void IocpConnection::PostRecvOp()
{
    if (m_ConnState == CONNECTION_STATE_ESTABLISHED)
    {
        BOOST_ASSERT(m_IocpAcceptor);
        BOOST_ASSERT(m_Socket != INVALID_SOCKET);

        Connection::PostRecvOp();

        WSABUF buf;

        buf.buf = m_RecvBuf;
        buf.len = sizeof(m_RecvBuf);

        DWORD flags = 0;
        DWORD numberOfBytesRecvd = 0;

        if (WSARecv(m_Socket, &buf, 1, &numberOfBytesRecvd, &flags, (OVERLAPPED*)&m_IocpOverlappedContext[IOCP_OPERATION_RECV], NULL) == SOCKET_ERROR)
        {
            DWORD errorCode = WSAGetLastError();
            switch (errorCode)
            {
            case WSA_IO_PENDING:
                break;
            case WSAECONNRESET: {
                IocpAcceptorOperationData opData;

                opData.opType = IOCP_ACCEPTOR_OPERATION_DISCONNECT;
                opData.conn = this;
                opData.size = numberOfBytesRecvd;

                m_IocpAcceptor->PushIocpAcceptorOperation(opData);
            }
            break;
            default:
                LOG_ERROR("WSARecv() failed, %s.", GetSockStrError(&errorCode)->c_str());
                return;
            }
        }
    }
}

// 投递发送操作
void IocpConnection::PostSendOp()
{
    BOOST_ASSERT(m_Socket != INVALID_SOCKET);
    BOOST_ASSERT(m_SendPacketDataQueue.size() > 0);

    Connection::PostSendOp();

    PacketData& packetData = m_SendPacketDataQueue.front();

    WSABUF buf;

    buf.buf = packetData.data + packetData.offset;
    buf.len = packetData.size - packetData.offset;

    DWORD flags = 0;
    DWORD numberOfBytesSent = 0;

    if (WSASend(m_Socket, &buf, 1, &numberOfBytesSent, flags, (OVERLAPPED*)&m_IocpOverlappedContext[IOCP_OPERATION_SEND], NULL) == SOCKET_ERROR)
    {
        DWORD errorCode = WSAGetLastError();
        switch (errorCode)
        {
        case WSA_IO_PENDING:
            break;
        case WSAECONNRESET:
            break;
        default:
            LOG_ERROR("WSASend() failed, %s.", GetSockStrError(&errorCode)->c_str());
            return;
        }
    }
}

// 获得接收器
Acceptor* IocpConnection::GetAcceptor()
{
    return m_IocpAcceptor;
}

// 获得完成端口上下文
IocpOverlappedContext* IocpConnection::GetIocpOverlappedContext(IOCP_OPERATION_TYPE opType)
{
    return &m_IocpOverlappedContext[opType];
}

// 完成端口接收器
IocpAcceptor* IocpConnection::GetIocpAcceptor()
{
    return m_IocpAcceptor;
}

void IocpConnection::SetIocpAcceptor(IocpAcceptor* icopAcceptor)
{
    m_IocpAcceptor = icopAcceptor;
}

// 获得接收缓冲区
char* IocpConnection::GetRecvBuf()
{
    return m_RecvBuf;
}
