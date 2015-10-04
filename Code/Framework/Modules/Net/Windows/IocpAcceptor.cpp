#include "Net/Windows/IocpAcceptor.h"
#include "Net/Windows/IocpConnection.h"

// 工作线程参数
struct IocpWorkerThreadArgs
{
    HANDLE iocpHandle;
    IocpAcceptor* iocpAcceptor;

    IocpWorkerThreadArgs()
        : iocpHandle(NULL)
        , iocpAcceptor(NULL)
    {

    }
};

// 工作线程
static void _IocpWorkerThread(IocpWorkerThreadArgs args)
{
    BOOST_ASSERT(args.iocpHandle);
    BOOST_ASSERT(args.iocpAcceptor);

    LOG_TRACE("_IocpWorkerThread Start");

    DWORD bytesTransfered = 0;
    PULONG_PTR completionKey = NULL;
    IocpOverlappedContext* overlapped = NULL;

    while (args.iocpAcceptor->GetAcceptorState() == ACCEPTOR_STATE_STARTED)
    {
        if (!GetQueuedCompletionStatus(args.iocpHandle, &bytesTransfered, (PULONG_PTR)&completionKey, (OVERLAPPED**)&overlapped, INFINITE))
        {
            // NOTE: 客户端不主动 closesocket 会产生 ERROR_NETNAME_DELETED 错误
            DWORD errorCode = GetLastError();
            if (errorCode == ERROR_NETNAME_DELETED)
            {
                IocpAcceptorOperationData opData;

                opData.opType = IOCP_ACCEPTOR_OPERATION_DISCONNECT;
                opData.conn = overlapped->conn;
                opData.size = bytesTransfered;

                args.iocpAcceptor->PushIocpAcceptorOperation(opData);
            }
            else
            {
                LOG_ERROR("GetQueuedCompletionStatus() failed, %s.", GetStrError(&errorCode));
            }
        }
        else if (overlapped)
        {
            switch (overlapped->opType)
            {
            case IOCP_OPERATION_ACCEPT: {
                IocpAcceptorOperationData opData;

                opData.opType = IOCP_ACCEPTOR_OPERATION_ACCEPT;
                opData.conn = overlapped->conn;
                opData.size = bytesTransfered;

                args.iocpAcceptor->PushIocpAcceptorOperation(opData);
            }
            break;
            case IOCP_OPERATION_RECV:
                if (bytesTransfered <= 0)
                {
                    IocpAcceptorOperationData opData;

                    opData.opType = IOCP_ACCEPTOR_OPERATION_DISCONNECT;
                    opData.conn = overlapped->conn;
                    opData.size = bytesTransfered;

                    args.iocpAcceptor->PushIocpAcceptorOperation(opData);
                }
                else
                {
                    BOOST_ASSERT(bytesTransfered <= RECV_BUF_SIZE);

                    IocpAcceptorOperationData opData;

                    opData.opType = IOCP_ACCEPTOR_OPERATION_RECV;
                    opData.conn = overlapped->conn;
                    opData.data = (char*)MALLOC(bytesTransfered, MEM_TAG_NET);
                    memcpy(opData.data, overlapped->conn->GetRecvBuf(), bytesTransfered);
                    opData.size = bytesTransfered;

                    args.iocpAcceptor->PushIocpAcceptorOperation(opData);

                    overlapped->conn->PostRecvOp();
                }
                break;
            case IOCP_OPERATION_SEND:
                overlapped->conn->OnSend(bytesTransfered);
                break;
            default:
                LOG_ERROR("Wrong overlapped->opType %d.", overlapped->opType);
                break;
            }
        }
    }

    LOG_TRACE("_IocpWorkerThread Stop");
}

// 完成端口接收器
IocpAcceptor::IocpAcceptor()
    : m_ListenSocket(INVALID_SOCKET)
    , m_AcceptEx(NULL)
    , m_GetAcceptExSockaddrs(NULL)
    , m_IocpHandle(NULL)
    , m_WorkingIocpConnections()
    , m_FreeIocpConnections()
    , m_IocpAcceptorOperations()
    , m_IocpAcceptorOperationsMutex()
{

}

IocpAcceptor::~IocpAcceptor()
{
    BOOST_ASSERT(m_ListenSocket == INVALID_SOCKET);
    BOOST_ASSERT(m_IocpHandle == NULL);
    BOOST_ASSERT(m_WorkerThreads.empty());
}

// 启动
bool IocpAcceptor::Start(const std::string& ip, int port)
{
    BOOST_ASSERT(m_ListenSocket == INVALID_SOCKET);

    if (!Acceptor::Start(ip, port))
    {
        return false;
    }

    m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_ListenSocket == INVALID_SOCKET)
    {
        LOG_ERROR("WSASocket() failed, %s", GetSockStrError()->c_str());
        Stop();
        return false;
    }

    DWORD bytesReceived = 0;

    GUID guidAcceptEx = WSAID_ACCEPTEX;
    if (WSAIoctl(m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                 &guidAcceptEx, sizeof(guidAcceptEx),
                 &m_AcceptEx, sizeof(m_AcceptEx),
                 &bytesReceived, NULL, NULL) == SOCKET_ERROR)
    {
        LOG_ERROR("WSAIoctl(WSAID_ACCEPTEX) failed, %s", GetSockStrError()->c_str());
        Stop();
        return false;
    }

    GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
    if (WSAIoctl(m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                 &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs),
                 &m_GetAcceptExSockaddrs, sizeof(m_GetAcceptExSockaddrs),
                 &bytesReceived, NULL, NULL) == SOCKET_ERROR)
    {
        LOG_ERROR("WSAIoctl(WSAID_GETACCEPTEXSOCKADDRS) failed, %s", GetSockStrError()->c_str());
        Stop();
        return false;
    }

    SYSTEM_INFO si;
    GetSystemInfo(&si);

    DWORD threadNum = si.dwNumberOfProcessors * 2;

    m_IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, threadNum);
    if (!m_IocpHandle)
    {
        LOG_ERROR("CreateIoCompletionPort() failed, %s.", GetStrError()->c_str());
        Stop();
        return false;
    }

    HANDLE newlyIocpHandle = CreateIoCompletionPort((HANDLE)m_ListenSocket, m_IocpHandle, 0, 0);
    if (!newlyIocpHandle)
    {
        LOG_ERROR("CreateIoCompletionPort() failed, %s.", GetStrError()->c_str());
        Stop();
        return false;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (bind(m_ListenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        LOG_ERROR("bind(\"%s\", %d) failed, %s.", ip.c_str(), port, GetSockStrError()->c_str());
        Stop();
        return false;
    }

    if (listen(m_ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        LOG_ERROR("listen() failed, %s", GetSockStrError()->c_str());
        Stop();
        return false;
    }

    IocpWorkerThreadArgs args;

    args.iocpHandle = m_IocpHandle;
    args.iocpAcceptor = this;

    for (DWORD i = 0; i < threadNum; ++i)
    {
        m_WorkerThreads.push_back(boost::thread(_IocpWorkerThread, args));
    }

    for (int i = 0; i < m_MaxAcceptNum; ++i)
    {
        PostAcceptOp();
    }

    return true;
}

// 关闭
void IocpAcceptor::Stop()
{
    // 需要先执行基类方法改变状态, 否则线程无法正常退出
    Acceptor::Stop();

    if (!m_WorkerThreads.empty())
    {
        for (std::size_t i = 0; i < m_WorkerThreads.size(); ++i)
        {
            PostQueuedCompletionStatus(m_IocpHandle, 0, NULL, NULL);
        }

        for (std::size_t i = 0; i < m_WorkerThreads.size(); ++i)
        {
            m_WorkerThreads[i].join();
        }

        m_WorkerThreads.clear();
    }

    if (m_IocpHandle)
    {
        CloseHandle(m_IocpHandle);
        m_IocpHandle = NULL;
    }

    if (m_ListenSocket != INVALID_SOCKET)
    {
        if (closesocket(m_ListenSocket) == SOCKET_ERROR)
        {
            LOG_ERROR("closesocket() failed, %s", GetSockStrError()->c_str());
        }

        m_ListenSocket = INVALID_SOCKET;
    }

    if (!m_WorkingIocpConnections.empty())
    {
        for (IocpConnectionMap::iterator it = m_WorkingIocpConnections.begin(); it != m_WorkingIocpConnections.end(); ++it)
        {
            IocpConnection* conn = it->second;

            if (conn->GetConnState() == CONNECTION_STATE_ESTABLISHED || conn->GetConnState() == CONNECTION_STATE_CLOSE_PENDING)
            {
                if (m_AcceptorListener)
                {
                    m_AcceptorListener->OnDisconnect(conn);
                }

                conn->OnDisconnect();
            }

            conn->OnFree();

            SAFE_DELETE(IocpConnection, conn);
        }

        m_WorkingIocpConnections.clear();
    }

    while (!m_FreeIocpConnections.empty())
    {
        IocpConnection* conn = m_FreeIocpConnections.top();

        conn->OnFree();

        SAFE_DELETE(IocpConnection, conn);

        m_FreeIocpConnections.pop();
    }

    if (!m_IocpAcceptorOperations.empty())
    {
        boost::lock_guard<boost::mutex> lockGuard(m_IocpAcceptorOperationsMutex);

        for (std::size_t i = 0; i < m_IocpAcceptorOperations.size(); ++i)
        {
            FREE(m_IocpAcceptorOperations[i].data);
        }

        m_IocpAcceptorOperations.clear();
    }
}

// 更新
void IocpAcceptor::Update()
{
    Acceptor::Update();

    if (!m_IocpAcceptorOperations.empty())
    {
        boost::lock_guard<boost::mutex> lockGuard(m_IocpAcceptorOperationsMutex);

        for (std::size_t i = 0; i < m_IocpAcceptorOperations.size(); ++i)
        {
            IocpAcceptorOperationData& opData = m_IocpAcceptorOperations[i];

            switch (opData.opType)
            {
            case IOCP_ACCEPTOR_OPERATION_ACCEPT: {
                BOOST_ASSERT(opData.conn);
                BOOST_ASSERT(m_GetAcceptExSockaddrs);

                sockaddr_in* localSockaddr = NULL;
                sockaddr_in* remoteSockaddr = NULL;
                int localSockaddrLen = sizeof(sockaddr_in);
                int remoteSockaddrLen = sizeof(sockaddr_in);

                m_GetAcceptExSockaddrs(opData.conn->GetRecvBuf(), 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, (sockaddr**)&localSockaddr, &localSockaddrLen, (sockaddr**)&remoteSockaddr, &remoteSockaddrLen);

                opData.conn->SetLocalSockaddr(*localSockaddr);
                opData.conn->SetRemoteSockaddr(*remoteSockaddr);

                opData.conn->OnAccept();

                if (m_AcceptorListener)
                {
                    m_AcceptorListener->OnAccept(opData.conn);
                }
            }
            break;
            case IOCP_ACCEPTOR_OPERATION_DISCONNECT:
                BOOST_ASSERT(opData.conn);

                if (m_AcceptorListener)
                {
                    m_AcceptorListener->OnDisconnect(opData.conn);
                }

                opData.conn->OnDisconnect();

                FreeIocpConnection(opData.conn);
                PostAcceptOp();
                break;
            case IOCP_ACCEPTOR_OPERATION_RECV:
                BOOST_ASSERT(opData.conn);
                BOOST_ASSERT(opData.data);
                BOOST_ASSERT(opData.size > 0);

                if (opData.conn->GetConnState() == CONNECTION_STATE_ESTABLISHED)
                {
                    opData.conn->OnRecv(opData.data, opData.size);

                    if (m_AcceptorListener)
                    {
                        m_AcceptorListener->OnRecv(opData.conn, opData.data, opData.size);
                    }
                }

                FREE(opData.data);
                break;
            default:
                LOG_ERROR("Wrong opData.opType %d.", opData.opType);
                break;
            }
        }

        m_IocpAcceptorOperations.clear();
    }
}

// 投递接收操作
void IocpAcceptor::PostAcceptOp()
{
    BOOST_ASSERT(m_AcceptEx);
    BOOST_ASSERT(m_IocpHandle);
    BOOST_ASSERT(m_ListenSocket);

    SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (acceptSocket == INVALID_SOCKET)
    {
        LOG_ERROR("WSASocket() failed, %s.", GetStrError()->c_str());
        return;
    }

    HANDLE newlyIocpHandle = CreateIoCompletionPort((HANDLE)acceptSocket, m_IocpHandle, 0, 0);
    if (!newlyIocpHandle)
    {
        LOG_ERROR("CreateIoCompletionPort() failed, %s.", GetStrError()->c_str());

        if (closesocket(acceptSocket) == SOCKET_ERROR)
        {
            LOG_ERROR("closesocket() failed, %s", GetSockStrError()->c_str());
        }
        return;
    }

    DWORD bytesReceived = 0;
    IocpConnection* conn = AllocIocpConnection();

    conn->SetSocket(acceptSocket);

    if (!m_AcceptEx(m_ListenSocket, acceptSocket, conn->GetRecvBuf(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, (OVERLAPPED*)conn->GetIocpOverlappedContext(IOCP_OPERATION_ACCEPT)))
    {
        DWORD errorCode = WSAGetLastError();
        if (errorCode != ERROR_IO_PENDING)
        {
            LOG_ERROR("AcceptEx() failed, %s.", GetSockStrError(&errorCode)->c_str());

            FreeIocpConnection(conn);
            return;
        }
    }
}

// 分配完成端口连接
IocpConnection* IocpAcceptor::AllocIocpConnection()
{
    IocpConnection* conn = NULL;

    if (!m_FreeIocpConnections.empty())
    {
        conn = m_FreeIocpConnections.top();
        m_FreeIocpConnections.pop();
    }
    else
    {
        conn = NEW(IocpConnection, MEM_TAG_NET);
    }

    BOOST_ASSERT(conn);

    conn->SetIocpAcceptor(this);
    conn->OnAlloc();

    if (!m_WorkingIocpConnections.insert(std::make_pair(conn->GetConnId(), conn)).second)
    {
        LOG_ERROR("Duplicate connId %u", conn->GetConnId());
    }

    return conn;
}

// 释放完成端口连接
void IocpAcceptor::FreeIocpConnection(IocpConnection* conn)
{
    BOOST_ASSERT(conn);

    m_FreeIocpConnections.push(conn);
    m_WorkingIocpConnections.erase(conn->GetConnId());

    conn->OnFree();
}

// 压入完成端口操作数据
void IocpAcceptor::PushIocpAcceptorOperation(const IocpAcceptorOperationData& opData)
{
    boost::lock_guard<boost::mutex> lockGuard(m_IocpAcceptorOperationsMutex);

    m_IocpAcceptorOperations.push_back(opData);
}
