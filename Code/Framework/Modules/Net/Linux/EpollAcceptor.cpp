#include "Net/Linux/EpollAcceptor.h"
#include "Net/Linux/EpollConnection.h"

// 工作线程参数
struct EpollWorkerThreadArgs
{
    int epollFd;
    SOCKET listenSocket;
    epoll_event* eventList;
    EpollAcceptor* epollAcceptor;

    EpollWorkerThreadArgs()
        : epollFd(-1)
        , listenSocket(INVALID_SOCKET)
        , eventList(NULL)
        , epollAcceptor(NULL)
    {

    }
};

// 工作线程
static void _EpollWorkerThread(EpollWorkerThreadArgs args)
{
    BOOST_ASSERT(args.epollFd != -1);
    BOOST_ASSERT(args.listenSocket != INVALID_SOCKET);
    BOOST_ASSERT(args.eventList);
    BOOST_ASSERT(args.epollAcceptor);

    LOG_TRACE("_EpollWorkerThread Start");

    while (args.epollAcceptor->GetAcceptorState() == ACCEPTOR_STATE_STARTED)
    {
        int nfds = epoll_wait(args.epollFd, args.eventList, EPOLL_WAIT_MAXEVENTS, EPOLL_WAIT_TIMEOUT);
        if (nfds == -1)
        {
            LOG_ERROR("epoll_wait() failed, %s", GetSockStrError()->c_str());
        }
        else
        {
            for (int i = 0; i < nfds; ++i)
            {
                EpollConnection* conn = (EpollConnection*)args.eventList[i].data.ptr;

                if (!conn)
                {
                    sockaddr_in addr;
                    socklen_t addrLen = sizeof(addr);

                    memset(&addr, 0, sizeof(addr));

                    SOCKET sock = accept(args.listenSocket, (sockaddr*)&addr, &addrLen);
                    if (sock == INVALID_SOCKET)
                    {
                        LOG_ERROR("accept() failed, %s", GetSockStrError()->c_str());
                    }
                    else
                    {
                        conn = args.epollAcceptor->AllocEpollConnection();
                        conn->SetSocket(sock);

                        setkeepalive(sock);
                        setnonblocking(sock);

                        EpollAcceptorOperationData opData;
                        opData.opType = EPOLL_ACCEPTOR_OPERATION_ACCEPT;
                        opData.conn = conn;
                        opData.addr = addr;
                        args.epollAcceptor->PushEpollAcceptorOperation(opData);
                    }
                }
                else
                {
                    // 可接收数据
                    if (args.eventList[i].events & EPOLLIN)
                    {
                        bool disconn = true;
                        unsigned char buf[RECV_BUF_SIZE];

                        int n = recv(conn->GetSocket(), buf, sizeof(buf), 0);

                        if (n > 0)
                        {
                            disconn = false;

                            EpollAcceptorOperationData opData;
                            opData.opType = EPOLL_ACCEPTOR_OPERATION_RECV;
                            opData.conn = conn;
                            opData.data = (char*)MALLOC(n, MEM_TAG_NET);
                            memcpy(opData.data, buf, n);
                            opData.size = n;
                            args.epollAcceptor->PushEpollAcceptorOperation(opData);
                        }
                        else if (n == 0)
                        {
                            // 正常断开
                        }
                        else
                        {
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
                                LOG_ERROR("recv() failed, %s", GetSockStrError(&errno)->c_str());
                                break;
                            }
                        }

                        if (disconn)
                        {
                            if (epoll_ctl(args.epollFd, EPOLL_CTL_DEL, conn->GetSocket(), NULL) == -1)
                            {
                                LOG_ERROR("epoll_ctl() failed, %s", GetSockStrError()->c_str());
                            }

                            EpollAcceptorOperationData opData;
                            opData.opType = EPOLL_ACCEPTOR_OPERATION_DISCONNECT;
                            opData.conn = conn;
                            args.epollAcceptor->PushEpollAcceptorOperation(opData);
                        }
                    }
                    // 可发送数据
                    else if (args.eventList[i].events & EPOLLOUT)
                    {
                        PacketData* packetData = conn->GetSendPacket();
                        if (!packetData)
                        {
                            epoll_event ev;

                            ev.events = EPOLLIN;
                            ev.data.ptr = conn;

                            if (epoll_ctl(args.epollFd, EPOLL_CTL_MOD, conn->GetSocket(), &ev) == -1)
                            {
                                LOG_ERROR("epoll_ctl() failed, %s", GetSockStrError()->c_str());
                            }
                        }
                        else
                        {
                            void* buf = packetData->data + packetData->offset;
                            int len = packetData->size - packetData->offset;
                            int n = send(conn->GetSocket(), buf, len, 0);

                            if (n < 0)
                            {
                                bool disconn = true;

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

                                if (disconn)
                                {

                                    if (epoll_ctl(args.epollFd, EPOLL_CTL_DEL, conn->GetSocket(), NULL) == -1)
                                    {
                                        LOG_ERROR("epoll_ctl() failed, %s", GetSockStrError()->c_str());
                                    }

                                    EpollAcceptorOperationData opData;
                                    opData.opType = EPOLL_ACCEPTOR_OPERATION_DISCONNECT;
                                    opData.conn = conn;
                                    args.epollAcceptor->PushEpollAcceptorOperation(opData);
                                }
                            }
                            else if (n > 0)
                            {
                                conn->OnSend(n);
                            }
                        }
                    }
                }
            }
        }
    }

    LOG_TRACE("_EpollWorkerThread Stop");
}

// Epoll 接收器
EpollAcceptor::EpollAcceptor()
    : m_ListenSocket(INVALID_SOCKET)
    , m_LocalAddr()
    , m_WorkerThread()
    , m_EpollFd(-1)
    , m_EpollAcceptorOperactions()
    , m_EpollAcceptorOperactionsMutex()
    , m_WorkingEpollConnections()
    , m_FreeEpollConnections()
    , m_EpollConnectionsMutex()
{

}

EpollAcceptor::~EpollAcceptor()
{

}

// 启动
bool EpollAcceptor::Start(const std::string& ip, int port)
{
    BOOST_ASSERT(m_ListenSocket == INVALID_SOCKET);

    if (!Acceptor::Start(ip, port))
    {
        return false;
    }

    m_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_ListenSocket == INVALID_SOCKET)
    {
        LOG_ERROR("socket() failed, %s", GetSockStrError()->c_str());
        Stop();
        return false;
    }

    // NOTE: 解决 TIME_WAIT 的连接导致的重启失败
    setreuseaddr(m_ListenSocket);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (bind(m_ListenSocket, (sockaddr*)&addr, sizeof(addr)) == -1)
    {
        LOG_ERROR("bind(\"%s\", %d) failed, %s", ip.c_str(), port, GetSockStrError()->c_str());
        Stop();
        return false;
    }

    // NOTE: backlog 的值是参考 nginx 的代码(NGX_LISTEN_BACKLOG)
    if (listen(m_ListenSocket, 511) == -1)
    {
        LOG_ERROR("listen() failed, %s", GetSockStrError()->c_str());
        Stop();
        return false;
    }

    // NOTE: 从 Linux 的 2.6.8 内核开始, size 参数只要大于 0 就行
    m_EpollFd = epoll_create(1024);
    if (m_EpollFd == -1)
    {
        LOG_ERROR("epoll_create() failed, %s", GetSockStrError()->c_str());
        Stop();
        return false;
    }

    epoll_event ev;

    ev.events = EPOLLIN;
    ev.data.ptr = NULL;

    if (epoll_ctl(m_EpollFd, EPOLL_CTL_ADD, m_ListenSocket, &ev) == -1)
    {
        LOG_ERROR("epoll_ctl() failed, %s", GetSockStrError()->c_str());
        Stop();
        return false;
    }

    for (int i = 0; i < m_MaxAcceptNum; ++i)
    {
        AllocEpollConnection();
    }

    EpollWorkerThreadArgs args;

    args.epollFd = m_EpollFd;
    args.listenSocket = m_ListenSocket;
    args.eventList = m_EpollEventList;
    args.epollAcceptor = this;

    m_WorkerThread = boost::thread(_EpollWorkerThread, args);

    return true;
}

// 关闭
void EpollAcceptor::Stop()
{
    // 需要先执行基类方法改变状态, 否则线程无法正常退出
    Acceptor::Stop();

    m_WorkerThread.join();

    if (m_ListenSocket != INVALID_SOCKET)
    {
        if (close(m_ListenSocket) == -1)
        {
            LOG_ERROR("close() failed, %s", GetSockStrError()->c_str());
        }

        m_ListenSocket = INVALID_SOCKET;
    }

    if (m_EpollFd != -1)
    {
        if (close(m_EpollFd) == -1)
        {
            LOG_ERROR("close() failed, %s", GetSockStrError()->c_str());
        }

        m_EpollFd = -1;
    }

    boost::lock_guard<boost::mutex> localGuard(m_EpollConnectionsMutex);

    if (!m_WorkingEpollConnections.empty())
    {
        for (EpollConnectionMap::iterator it = m_WorkingEpollConnections.begin(); it != m_WorkingEpollConnections.end(); ++it)
        {
            EpollConnection* conn = it->second;

            if (conn->GetConnState() == CONNECTION_STATE_ESTABLISHED || conn->GetConnState() == CONNECTION_STATE_CLOSE_PENDING)
            {
                HandleDisconnect(conn);
            }

            conn->OnFree();

            SAFE_DELETE(EpollConnection, conn);
        }

        m_WorkingEpollConnections.clear();
    }

    while (!m_FreeEpollConnections.empty())
    {
        EpollConnection* conn = m_FreeEpollConnections.top();

        conn->OnFree();

        SAFE_DELETE(EpollConnection, conn);

        m_FreeEpollConnections.pop();
    }

    if (!m_EpollAcceptorOperactions.empty())
    {
        boost::lock_guard<boost::mutex> lockGuard(m_EpollAcceptorOperactionsMutex);

        for (std::size_t i = 0; i < m_EpollAcceptorOperactions.size(); ++i)
        {
            FREE(m_EpollAcceptorOperactions[i].data);
        }

        m_EpollAcceptorOperactions.clear();
    }
}

// 更新
void EpollAcceptor::Update()
{
    Acceptor::Update();

    if (!m_EpollAcceptorOperactions.empty())
    {
        boost::lock_guard<boost::mutex> localGuard(m_EpollAcceptorOperactionsMutex);

        for (size_t i = 0; i < m_EpollAcceptorOperactions.size(); ++i)
        {
            EpollAcceptorOperationData& opData = m_EpollAcceptorOperactions[i];

            switch (opData.opType)
            {
            case EPOLL_ACCEPTOR_OPERATION_ACCEPT:
                BOOST_ASSERT(opData.conn);

                opData.conn->SetLocalSockaddr(m_LocalAddr);
                opData.conn->SetRemoteSockaddr(opData.addr);

                opData.conn->OnAccept();

                if (m_AcceptorListener)
                {
                    m_AcceptorListener->OnAccept(opData.conn);
                }
                break;
            case EPOLL_ACCEPTOR_OPERATION_DISCONNECT:
                BOOST_ASSERT(opData.conn);

                HandleDisconnect(opData.conn);

                FreeEpollConnection(opData.conn);
                break;
            case EPOLL_ACCEPTOR_OPERATION_RECV:
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

        m_EpollAcceptorOperactions.clear();
    }
}

// 分配 Epoll 连接
EpollConnection* EpollAcceptor::AllocEpollConnection()
{
    boost::lock_guard<boost::mutex> localGuard(m_EpollConnectionsMutex);

    EpollConnection* conn = NULL;

    if (!m_FreeEpollConnections.empty())
    {
        conn = m_FreeEpollConnections.top();
        m_FreeEpollConnections.pop();
    }
    else
    {
        conn = NEW(EpollConnection, MEM_TAG_NET);
    }

    BOOST_ASSERT(conn);

    conn->SetEpollAcceptor(this);
    conn->OnAlloc();

    if (!m_WorkingEpollConnections.insert(std::make_pair(conn->GetConnId(), conn)).second)
    {
        LOG_ERROR("Duplicate connId %u", conn->GetConnId());
    }

    return conn;
}

// 释放 Epoll 连接
void EpollAcceptor::FreeEpollConnection(EpollConnection* conn)
{
    BOOST_ASSERT(conn);

    boost::lock_guard<boost::mutex> localGuard(m_EpollConnectionsMutex);

    m_FreeEpollConnections.push(conn);
    m_WorkingEpollConnections.erase(conn->GetConnId());

    conn->OnFree();
}

// 压入 Epoll 操作数据
void EpollAcceptor::PushEpollAcceptorOperation(const EpollAcceptorOperationData& opData)
{
    boost::lock_guard<boost::mutex> localGuard(m_EpollAcceptorOperactionsMutex);

    m_EpollAcceptorOperactions.push_back(opData);
}

// 获得 Epoll 实例句柄
int EpollAcceptor::GetEpollFd()
{
    return m_EpollFd;
}

// 处理断开连接
void EpollAcceptor::HandleDisconnect(EpollConnection* conn)
{
    BOOST_ASSERT(conn);

    if (m_AcceptorListener)
    {
        m_AcceptorListener->OnDisconnect(conn);
    }

    conn->OnDisconnect();
}
