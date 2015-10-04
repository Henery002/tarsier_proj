#include "Net/Connector.h"

// 工作线程参数
struct SelectWorkerThreadArgs
{
    Connector* connector;

    SelectWorkerThreadArgs()
        : connector(NULL)
    {

    }
};

// 工作线程
static void _SelectWorkerThread(SelectWorkerThreadArgs args)
{
    BOOST_ASSERT(args.connector);

    LOG_TRACE("_SelectWorkerThread Start");

    Connector* connector = args.connector;
    BOOST_ASSERT(connector);

    Connection* conn = connector->GetConnection();
    BOOST_ASSERT(conn);

    fd_set* recvFdSet = connector->GetRecvFdSet();
    fd_set* sendFdSet = connector->GetSendFdSet();
    fd_set* exceptFdSet = connector->GetExceptFdSet();
    SOCKET sock = conn->GetSocket();

    BOOST_ASSERT(recvFdSet);
    BOOST_ASSERT(sendFdSet);
    BOOST_ASSERT(exceptFdSet);
    BOOST_ASSERT(sock != INVALID_SOCKET);

#if TARGET_PLATFORM == PLATFORM_LINUX
    exceptFdSet = NULL;
#endif // TARGET_PLATFORM

    int elapsedTime = 0;
    int timeout = connector->GetTimeout() * 1000;

    // NOTE: 工作线程里记录的连接器状态, 避免了线程同步
    CONNECTOR_STATE_TYPE connState = connector->GetConnectorState();

    while (connector->GetConnectorState() == CONNECTOR_STATE_CONNECTING || connector->GetConnectorState() == CONNECTOR_STATE_CONNECTED)
    {
        FD_ZERO(recvFdSet);
        FD_ZERO(sendFdSet);
#if TARGET_PLATFORM == PLATFORM_WINDOWS
        FD_ZERO(exceptFdSet);
#endif // TARGET_PLATFORM

        FD_SET(sock, recvFdSet);

        if (connState == CONNECTOR_STATE_CONNECTING)
        {
            FD_SET(sock, sendFdSet);
#if TARGET_PLATFORM == PLATFORM_WINDOWS
            FD_SET(sock, exceptFdSet);
#endif // TARGET_PLATFORM
        }
        else
        {
            BOOST_ASSERT(connState == CONNECTOR_STATE_CONNECTED);

            PacketData* packetData = conn->GetSendPacket();
            if (packetData)
            {
                FD_SET(sock, sendFdSet);
            }
        }

        timeval tv;
        tv.tv_sec = SELECT_WAIT_TIMEOUT / 1000;
        tv.tv_usec = (SELECT_WAIT_TIMEOUT % 1000) * 1000;

        int ready = select(sock + 1, recvFdSet, sendFdSet, exceptFdSet, &tv);

        if (ready > 0)
        {
#if TARGET_PLATFORM == PLATFORM_WINDOWS

            // 异常, NOTE: Linux 检测连接成功与否通过 recv
            if (FD_ISSET(sock, exceptFdSet))
            {
                BOOST_ASSERT(connState == CONNECTOR_STATE_CONNECTING);

                ConnectorOperationData opData;
                opData.opType = CONNECTOR_OPERATION_CONNECT_FAILED;

                int err = getsockerror(sock);

                switch (err)
                {
                case WSAECONNREFUSED:
                    opData.failedType = CONNECT_FAILED_CONNREFUSED;
                    break;
                case WSAENETUNREACH:
                case WSAEHOSTUNREACH:
                    opData.failedType = CONNECT_FAILED_UNREACHABLE;
                    break;
                case WSAETIMEDOUT:
                    opData.failedType = CONNECT_FAILED_TIMEDOUT;
                    break;
                default:
                    opData.failedType = CONNECT_FAILED_OTHERS;
                    break;
                }

                connector->PushOperation(opData);

                connState = CONNECTOR_STATE_CLOSED;
                break;
            }

#endif // TARGET_PLATFORM

            // 可发送数据
            if (FD_ISSET(sock, sendFdSet))
            {
#if TARGET_PLATFORM == PLATFORM_LINUX

                if (FD_ISSET(sock, recvFdSet))
                {
                    // 可读且可写则有错误产生
                    goto _RECVSET;
                }

#endif // TARGET_PLATFORM

                if (connState == CONNECTOR_STATE_CONNECTING)
                {
                    ConnectorOperationData opData;
                    opData.opType = CONNECTOR_OPERATION_CONNECTED;
                    opData.conn = connector->GetConnection();
                    connector->PushOperation(opData);

                    connState = CONNECTOR_STATE_CONNECTED;
                }
                else
                {
                    BOOST_ASSERT(connState == CONNECTOR_STATE_CONNECTED);

                    PacketData* packetData = conn->GetSendPacket();
                    if (packetData)
                    {
                        char* buf = packetData->data + packetData->offset;
                        int len = packetData->size - packetData->offset;
                        int n = send(sock, buf, len, 0);

                        if (n < 0)
                        {
                            bool disconn = true;

#if TARGET_PLATFORM == PLATFORM_WINDOWS

                            DWORD err = WSAGetLastError();
                            switch (err)
                            {
                            case WSAECONNRESET:
                                // 正常断开
                                break;
                            default:
                                LOG_ERROR("send() failed, %s", GetSockStrError(&err)->c_str());
                                break;
                            }

#elif TARGET_PLATFORM == PLATFORM_LINUX

                            switch (errno)
                            {
                            case EAGAIN:
                            case EINTR:
                                disconn = false;
                                break;
                            case ECONNRESET:
                                // 正常断开
                                break;
                            default:
                                LOG_ERROR("send() failed, %s", GetSockStrError(&errno)->c_str());
                                break;
                            }

#endif // TARGET_PLATFORM
                            if (disconn)
                            {
                                ConnectorOperationData opData;
                                opData.opType = CONNECTOR_OPERATION_DISCONNECT;
                                opData.conn = conn;
                                connector->PushOperation(opData);

                                connState = CONNECTOR_STATE_CLOSED;
                                break;
                            }
                        }
                        else if (n > 0)
                        {
                            conn->OnSend(n);
                        }
                    }
                }
            }

            // 可接收数据
            if (FD_ISSET(sock, recvFdSet))
            {
#if TARGET_PLATFORM == PLATFORM_LINUX
_RECVSET:
#endif // TARGET_PLATFORM

                char buf[RECV_BUF_SIZE];
                int n = recv(sock, buf, sizeof(buf), 0);

                if (n > 0)
                {
                    ConnectorOperationData opData;
                    opData.opType = CONNECTOR_OPERATION_RECV;
                    opData.conn = conn;
                    opData.data = (char*)MALLOC(n, MEM_TAG_NET);
                    memcpy(opData.data, buf, n);
                    opData.size = n;
                    connector->PushOperation(opData);
                }
                else if (n == 0)
                {
                    ConnectorOperationData opData;
                    opData.opType = CONNECTOR_OPERATION_DISCONNECT;
                    opData.conn = conn;
                    connector->PushOperation(opData);

                    connState = CONNECTOR_STATE_CLOSED;
                    break;
                }
                else
                {
                    bool disconn = true;

                    ConnectorOperationData opData;
                    opData.opType = CONNECTOR_OPERATION_DISCONNECT;
                    opData.conn = conn;

#if TARGET_PLATFORM == PLATFORM_WINDOWS

                    DWORD errorCode = WSAGetLastError();
                    switch (errorCode)
                    {
                    case WSAECONNABORTED:
                    case WSAECONNRESET:
                        // 正常断开
                        break;
                    default:
                        LOG_ERROR("recv() failed, %s", GetSockStrError(&errorCode)->c_str());
                        break;
                    }

#elif TARGET_PLATFORM == PLATFORM_LINUX

                    switch (errno)
                    {
                    case EAGAIN:
                    case EINTR:
                        disconn = false;
                        break;
                    case ECONNRESET:
                        // 正常断开
                        break;
                    case ECONNREFUSED: {
                        if (connState == CONNECTOR_STATE_CONNECTING)
                        {
                            opData.opType = CONNECTOR_OPERATION_CONNECT_FAILED;
                            opData.failedType = CONNECT_FAILED_CONNREFUSED;
                        }
                    }
                    break;
                    case EHOSTUNREACH: {
                        if (connState == CONNECTOR_STATE_CONNECTING)
                        {
                            opData.opType = CONNECTOR_OPERATION_CONNECT_FAILED;
                            opData.failedType = CONNECT_FAILED_UNREACHABLE;
                        }
                    }
                    break;
                    default:
                        LOG_ERROR("recv() failed, %s", GetSockStrError(&errno)->c_str());
                        break;
                    }

#endif // TARGET_PLATFORM

                    if (disconn)
                    {
                        connector->PushOperation(opData);

                        connState = CONNECTOR_STATE_CLOSED;
                        break;
                    }
                }
            }
        }
        else if (ready == 0)
        {
            elapsedTime += SELECT_WAIT_TIMEOUT;

            if (connState == CONNECTOR_STATE_CONNECTING && elapsedTime >= timeout && timeout > 0)
            {
                ConnectorOperationData opData;
                opData.opType = CONNECTOR_OPERATION_CONNECT_FAILED;
                opData.failedType = CONNECT_FAILED_TIMEDOUT;
                connector->PushOperation(opData);

                connState = CONNECTOR_STATE_CLOSED;
                break;
            }
        }
        else
        {
            LOG_ERROR("select() failed, %s", GetSockStrError()->c_str());

            if (connState == CONNECTOR_STATE_CONNECTING)
            {
                ConnectorOperationData opData;
                opData.opType = CONNECTOR_OPERATION_CONNECT_FAILED;
                opData.failedType = CONNECT_FAILED_OTHERS;
                connector->PushOperation(opData);
            }
            else
            {
                BOOST_ASSERT(connState == CONNECTOR_STATE_CONNECTED);

                ConnectorOperationData opData;
                opData.opType = CONNECTOR_OPERATION_DISCONNECT;
                opData.conn = conn;
                connector->PushOperation(opData);
            }

            break;
        }
    }

    // NOTE: 统一在这里释放套接字
    conn->FreeSocket();

    LOG_TRACE("_SelectWorkerThread Stop");
}

// 连接器
Connector::Connector()
    : m_Ip("127.0.0.1")
    , m_Port(0)
    , m_Timeout(-1)
    , m_Connection(NULL)
    , m_ConnectorListener(NULL)
    , m_ConnectorState(CONNECTOR_STATE_INVALID)
    , m_WorkerThread()
    , m_RecvFdSet()
    , m_SendFdSet()
    , m_ExceptFdSet()
    , m_ConnectorOperationData()
    , m_ConnectorOperationDataMutex()
{
    LOG_TRACE("Connector::Connector()");
}

Connector::~Connector()
{
    LOG_TRACE("Connector::~Connector()");

    BOOST_ASSERT(!m_Connection);
}

// 连接, timeout 单位为秒, -1表示使用系统超时时间(75秒)
void Connector::Connect()
{
    if (m_ConnectorState == CONNECTOR_STATE_CONNECTING)
    {
        return;
    }

    LOG_TRACE("Connect::Connect()|%s|%d", GetAddr(m_Ip, m_Port).c_str(), m_Timeout);

    DisconnectImpl();

    m_Connection = NEW(SelectConnection, MEM_TAG_NET);
    m_Connection->OnAlloc();
    m_Connection->SetConnector(this);

    int protocol = 0;

#if TARGET_PLATFORM == PLATFORM_WINDOWS
    protocol = IPPROTO_TCP;
#endif // TARGET_PLATFORM

    SOCKET sock = socket(AF_INET, SOCK_STREAM, protocol);

    if (sock == INVALID_SOCKET)
    {
        LOG_ERROR("socket() failed, %s", GetSockStrError()->c_str());
        DisconnectImpl();
        return;
    }

    m_Connection->SetSocket(sock);

    setnonblocking(sock);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(m_Ip.c_str());
    addr.sin_port = htons(m_Port);

    m_Connection->SetRemoteSockaddr(addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        bool pending = false;

#if TARGET_PLATFORM == PLATFORM_WINDOWS

        DWORD errorCode = WSAGetLastError();
        switch (errorCode)
        {
        case WSAEWOULDBLOCK:
            pending = true;
            break;
        };

#elif TARGET_PLATFORM == PLATFORM_LINUX

        switch (errno)
        {
        case EINPROGRESS:
            pending = true;
            break;
        }

#endif // TARGET_PLATFORM

        if (!pending)
        {
            LOG_ERROR("connect() failed, %s", GetSockStrError()->c_str());

            ConnectorOperationData opData;
            opData.opType = CONNECTOR_OPERATION_CONNECT_FAILED;
            opData.failedType = CONNECT_FAILED_OTHERS;
            PushOperation(opData);

            DisconnectImpl();
            return;
        }
    }
    else
    {
        ConnectorOperationData opData;
        opData.opType = CONNECTOR_OPERATION_CONNECTED;
        opData.conn = m_Connection;
        PushOperation(opData);
    }

    m_ConnectorState = CONNECTOR_STATE_CONNECTING;

    SelectWorkerThreadArgs args;
    args.connector = this;
    m_WorkerThread = boost::thread(_SelectWorkerThread, args);
}

void Connector::Connect(const std::string& ip, int port, int timeout)
{
    LOG_TRACE("Connect::Connect(%s, %d, %d)", ip.c_str(), port, timeout);

    m_Ip = ip;
    m_Port = port;
    m_Timeout = timeout;

    Connect();
}

// 断开连接, NOTE: 不会保证全部数据发送完成才真正断开
void Connector::Disconnect()
{
    LOG_TRACE("Disconnect::Disconnect()|%s|%d", GetAddr(m_Ip, m_Port).c_str(), m_Timeout);

    DisconnectImpl();
}

// 更新
void Connector::Update()
{
    if (!m_ConnectorOperationData.empty())
    {
        boost::lock_guard<boost::mutex> lockGuard(m_ConnectorOperationDataMutex);

        for (std::size_t i = 0; i < m_ConnectorOperationData.size(); ++i)
        {
            ConnectorOperationData& opData = m_ConnectorOperationData[i];

            switch (opData.opType)
            {
            case CONNECTOR_OPERATION_CONNECTED: {
                BOOST_ASSERT(opData.conn);

                opData.conn->SetLocalSockaddr(getlocalsockaddr(opData.conn->GetSocket()));
                opData.conn->OnConnected();

                m_ConnectorState = CONNECTOR_STATE_CONNECTED;

                if (m_ConnectorListener)
                {
                    m_ConnectorListener->OnConnected(opData.conn);
                }
            }
            break;
            case CONNECTOR_OPERATION_CONNECT_FAILED:
                BOOST_ASSERT(opData.failedType != CONNECT_FAILED_INVALID);

                m_ConnectorState = CONNECTOR_STATE_CLOSED;

                if (m_ConnectorListener)
                {
                    m_ConnectorListener->OnConnectFailed(opData.failedType);
                }
                break;
            case CONNECTOR_OPERATION_DISCONNECT:
                BOOST_ASSERT(opData.conn);

                HandleDisconnect(opData.conn);
                break;
            case CONNECTOR_OPERATION_RECV:
                BOOST_ASSERT(opData.conn);
                BOOST_ASSERT(opData.data);
                BOOST_ASSERT(opData.size > 0);

                if (opData.conn->GetConnState() == CONNECTION_STATE_ESTABLISHED)
                {
                    opData.conn->OnRecv(opData.data, opData.size);

                    if (m_ConnectorListener)
                    {
                        m_ConnectorListener->OnRecv(opData.conn, opData.data, opData.size);
                    }
                }

                FREE(opData.data);
                break;
            default:
                LOG_ERROR("Wrong opData.opType %d.", opData.opType);
                break;
            }
        }

        m_ConnectorOperationData.clear();
    }
}

// 发送数据包
void Connector::SendPacket(int cmd, const std::string* body, int err, const std::string* extra)
{
    BOOST_ASSERT(m_Connection);
    BOOST_ASSERT(m_ConnectorState == CONNECTOR_STATE_CONNECTED);

    m_Connection->SendPacket(cmd, body, err, extra);
}

// 发送数据
void Connector::Send(const char* data, uint32_t size)
{
    BOOST_ASSERT(data);
    BOOST_ASSERT(size > 0);
    BOOST_ASSERT(m_Connection);
    BOOST_ASSERT(m_ConnectorState == CONNECTOR_STATE_CONNECTED);

    m_Connection->Send(data, size);
}

// 获得IP
const std::string& Connector::GetIp()
{
    return m_Ip;
}

// 获得端口
int Connector::GetPort()
{
    return m_Port;
}

// 获得超时时间
int Connector::GetTimeout()
{
    return m_Timeout;
}

// 连接监听器
ConnectorListener* Connector::GetConnectorListener()
{
    return m_ConnectorListener;
}

void Connector::SetConnectorListener(ConnectorListener* connectorListener)
{
    m_ConnectorListener = connectorListener;
}

// 获得连接器状态
CONNECTOR_STATE_TYPE Connector::GetConnectorState()
{
    return m_ConnectorState;
}

// 获得接收句柄集
fd_set* Connector::GetRecvFdSet()
{
    return &m_RecvFdSet;
}

// 获得发送句柄集
fd_set* Connector::GetSendFdSet()
{
    return &m_SendFdSet;
}

// 获得异常句柄集
fd_set* Connector::GetExceptFdSet()
{
    return &m_ExceptFdSet;
}

// 获得连接
Connection* Connector::GetConnection()
{
    return m_Connection;
}

// 压入操作数据
void Connector::PushOperation(const ConnectorOperationData& opData)
{
    boost::lock_guard<boost::mutex> lockGuard(m_ConnectorOperationDataMutex);

    m_ConnectorOperationData.push_back(opData);
}

// 断开连接实现
void Connector::DisconnectImpl()
{
    CONNECTION_STATE_TYPE preConnState = CONNECTION_STATE_INVALID;
    if (m_Connection)
    {
        preConnState = m_Connection->GetConnState();
    }

    m_ConnectorState = CONNECTOR_STATE_CLOSE_PENDING;

    m_WorkerThread.join();

    if (m_Connection)
    {
        if (preConnState == CONNECTION_STATE_ESTABLISHED || preConnState == CONNECTION_STATE_CLOSE_PENDING)
        {
            HandleDisconnect(m_Connection);
        }

        m_Connection->OnFree();
        SAFE_DELETE(Connection, m_Connection);
    }

    if (!m_ConnectorOperationData.empty())
    {
        boost::lock_guard<boost::mutex> lockGuard(m_ConnectorOperationDataMutex);

        for (std::size_t i = 0; i < m_ConnectorOperationData.size(); ++i)
        {
            FREE(m_ConnectorOperationData[i].data);
        }

        m_ConnectorOperationData.clear();
    }

    m_ConnectorState = CONNECTOR_STATE_CLOSED;
}

// 处理断开连接
void Connector::HandleDisconnect(Connection* conn)
{
    BOOST_ASSERT(conn);

    m_ConnectorState = CONNECTOR_STATE_CLOSED;

    if (m_ConnectorListener)
    {
        m_ConnectorListener->OnDisconnect(conn);
    }

    conn->OnDisconnect();
}
