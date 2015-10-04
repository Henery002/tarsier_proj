#include "Net/Connection.h"
#include "Net/Acceptor.h"
#include "Net/ProtoHandler.h"

// 连接 ID 种子
static uint64_t s_ConnIdSeed = 0;

// 连接 ID 种子互斥体
static boost::mutex s_ConnIdSeedMutex;

// 连接
Connection::Connection()
    : m_Socket(INVALID_SOCKET)
    , m_ConnId(0)
    , m_ConnState(CONNECTION_STATE_INVALID)
    , m_LocalIp()
    , m_LocalPort(0)
    , m_LocalAddr()
    , m_RemoteIp()
    , m_RemotePort(0)
    , m_RemoteAddr()
    , m_RecvPacketDataQueue(DEFAULT_RECV_PACKET_DATA_QUEUE_CAPACITY)
    , m_RecvPacketDataMutex()
    , m_SendPacketDataQueue(DEFAULT_SEND_PACKET_DATA_QUEUE_CAPACITY)
    , m_SendPacketDataMutex()
    , m_ProtoHandler(NULL)
    , m_ConnContext()
{
    m_ProtoHandler = NEW(ProtoHandler, MEM_TAG_NET);
}

Connection::~Connection()
{
    SAFE_DELETE(ProtoHandler, m_ProtoHandler);

    BOOST_ASSERT(m_Socket == INVALID_SOCKET);
    BOOST_ASSERT(m_ConnState == CONNECTION_STATE_INVALID || m_ConnState == CONNECTION_STATE_CLOSED);
}

// 发送数据包
void Connection::SendPacket(int cmd, const std::string* body, int err, const std::string* extra)
{
    Packet packet;

    packet.set_size(0);
    packet.set_cmd(cmd);
    packet.set_err(err);

    if (body)
    {
        packet.set_body(*body);
    }

    if (extra)
    {
        packet.set_extra(*extra);
    }

    std::string bufPacket;
    if (!packet.SerializeToString(&bufPacket))
    {
        LOG_ERROR("packet.SerializeToString failed|%s|%d", GetRemoteAddr().c_str(), packet.cmd());;
        return;
    }

    PacketSize packetSize;
    packetSize.set_size(bufPacket.size());

    std::string bufSize;
    if (!packetSize.SerializeToString(&bufSize))
    {
        LOG_ERROR("packetSize.SerializeToString failed|%s|%d|%u", GetRemoteAddr().c_str(), packet.cmd(), bufPacket.size());;
        return;
    }

    BOOST_ASSERT(bufSize.size() == PACKET_SIZE_LEN);

    // NOTE: 这种做法是建立在 fixed32 特性上
    bufPacket.replace(0, bufSize.size(), bufSize);

    Send(bufPacket.c_str(), bufPacket.size());
}

// 发送数据
void Connection::Send(const char* data, uint32_t size)
{
    BOOST_ASSERT(data);
    BOOST_ASSERT(size > 0);
    BOOST_ASSERT(m_ConnState == CONNECTION_STATE_ESTABLISHED);

    LOG_TRACE("Connection::Send(%d)|%s", size, m_RemoteAddr.c_str());

    char* buf = (char*)MALLOC(size, MEM_TAG_USR);
    memcpy(buf, data, size);

    PacketData packetData;

    packetData.data = buf;
    packetData.size = size;
    packetData.offset = 0;

    boost::lock_guard<boost::mutex> lockGuard(m_SendPacketDataMutex);

    if (m_SendPacketDataQueue.full())
    {
        m_SendPacketDataQueue.set_capacity(m_SendPacketDataQueue.capacity() * 2);
    }

    m_SendPacketDataQueue.push_back(packetData);

    PostSendOp();
}

// 断开连接, NOTE: 会保证全部数据发送完成才真正断开
void Connection::Disconnect()
{
    BOOST_ASSERT(m_ConnState == CONNECTION_STATE_ESTABLISHED);

    LOG_TRACE("Disconnect()|%s", m_RemoteAddr.c_str());

    boost::lock_guard<boost::mutex> lockGuard(m_SendPacketDataMutex);

    if (m_SendPacketDataQueue.size() <= 0)
    {
        HandleDisconnect();
    }
    else
    {
        m_ConnState = CONNECTION_STATE_CLOSE_PENDING;
    }
}

// 当分配
void Connection::OnAlloc()
{
    BOOST_ASSERT(m_ProtoHandler);

    boost::lock_guard<boost::mutex> lockGuard(s_ConnIdSeedMutex);

    m_ConnId = ++s_ConnIdSeed;

    if (m_ConnId == 0)
    {
        LOG_ERROR("ConnId not enough.");
    }

    m_ProtoHandler->Reset();
    m_ConnContext.Reset();
}

// 当释放
void Connection::OnFree()
{
    FreeSocket();

    do
    {
        boost::lock_guard<boost::mutex> lockGuard(m_RecvPacketDataMutex);

        for (std::size_t i = 0; i < m_RecvPacketDataQueue.size(); ++i)
        {
            FREE(m_RecvPacketDataQueue[i].data);
        }

        m_RecvPacketDataQueue.clear();
    }
    while (0);

    do
    {
        boost::lock_guard<boost::mutex> lockGuard(m_SendPacketDataMutex);

        for (std::size_t i = 0; i < m_SendPacketDataQueue.size(); ++i)
        {
            FREE(m_SendPacketDataQueue[i].data);
        }

        m_SendPacketDataQueue.clear();
    }
    while (0);
}

// 当接收连接
void Connection::OnAccept()
{
    LOG_TRACE("Connection::OnAccept()|%s", m_RemoteAddr.c_str());

    m_ConnState = CONNECTION_STATE_ESTABLISHED;

    PostRecvOp();
}

// 当连接成功
void Connection::OnConnected()
{
    LOG_TRACE("Connection::OnConnected()|%s", m_RemoteAddr.c_str());

    m_ConnState = CONNECTION_STATE_ESTABLISHED;
}

// 当接收数据
void Connection::OnRecv(char* data, uint32_t size)
{
    BOOST_ASSERT(m_ConnState == CONNECTION_STATE_ESTABLISHED);

    LOG_TRACE("Connection::OnRecv(%u)|%s", size, m_RemoteAddr.c_str());
}

// 当发送数据
void Connection::OnSend(uint32_t size)
{
    BOOST_ASSERT(m_ConnState == CONNECTION_STATE_ESTABLISHED || m_ConnState == CONNECTION_STATE_CLOSE_PENDING);

    LOG_TRACE("Connection::OnSend(%u)|%s", size, m_RemoteAddr.c_str());

    boost::lock_guard<boost::mutex> lockGuard(m_SendPacketDataMutex);

    BOOST_ASSERT(m_SendPacketDataQueue.size() > 0);

    PacketData& packetData = m_SendPacketDataQueue.front();

    packetData.offset += size;

    if (packetData.offset >= packetData.size)
    {
        FREE(packetData.data);
        m_SendPacketDataQueue.pop_front();
    }

    if (m_SendPacketDataQueue.size() > 0)
    {
        PostSendOp();
    }
    else
    {
        OnSendComplete();

        if (m_ConnState == CONNECTION_STATE_CLOSE_PENDING)
        {
            HandleDisconnect();
        }
    }
}

// 当完成发送数据
void Connection::OnSendComplete()
{
    LOG_TRACE("Connection::OnSendComplete()|%s", m_RemoteAddr.c_str());
}

// 当断开连接
void Connection::OnDisconnect()
{
    LOG_TRACE("Connection::OnDisconnect()|%s", m_RemoteAddr.c_str());
}

// 投递接收操作
void Connection::PostRecvOp()
{
    LOG_TRACE("Connection::PostRecvOp()|%s", m_RemoteAddr.c_str());
}

// 投递发送操作
void Connection::PostSendOp()
{
    LOG_TRACE("Connection::PostSendOp()|%s", m_RemoteAddr.c_str());
}

// 处理断开连接
void Connection::HandleDisconnect()
{
    FreeSocket();
}

// 获得接收器
Acceptor* Connection::GetAcceptor()
{
    return NULL;
}

// 获得连接器
Connector* Connection::GetConnector()
{
    return NULL;
}

// 套接字
SOCKET Connection::GetSocket()
{
    return m_Socket;
}

void Connection::SetSocket(SOCKET socket)
{
    m_Socket = socket;
}

// 获得连接 ID
uint64_t Connection::GetConnId()
{
    return m_ConnId;
}

// 获得连接状态
CONNECTION_STATE_TYPE Connection::GetConnState()
{
    return m_ConnState;
}

// 获得本地 IP
const std::string& Connection::GetLocalIp()
{
    return m_LocalIp;
}

// 获得本地端口
int Connection::GetLocalPort()
{
    return m_LocalPort;
}

// 本地地址, IP:PORT
const std::string& Connection::GetLocalAddr()
{
    return m_LocalAddr;
}

void Connection::SetLocalSockaddr(const sockaddr_in& addr)
{
#if TARGET_PLATFORM == PLATFORM_WINDOWS

    m_LocalIp = inet_ntoa(addr.sin_addr);
    m_LocalPort = ntohs(addr.sin_port);
    m_LocalAddr = GetAddr(m_LocalIp, m_LocalPort);

#elif TARGET_PLATFORM == PLATFORM_LINUX

    char buf[64] = { '\0' };

    if (!inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf)))
    {
        LOG_ERROR("inet_ntop() failed, %s", GetSockStrError()->c_str());
    }
    else
    {
        m_LocalIp = buf;
    }

    m_LocalPort = ntohs(addr.sin_port);
    m_LocalAddr = GetAddr(m_LocalIp, m_LocalPort);

#endif // TARGET_PLATFORM
}

// 获得远端 IP
const std::string& Connection::GetRemoteIp()
{
    return m_RemoteIp;
}

// 获得远端端口
int Connection::GetRemotePort()
{
    return m_RemotePort;
}

// 远端地址, IP:PORT
const std::string& Connection::GetRemoteAddr()
{
    return m_RemoteAddr;
}

// 远端地址
void Connection::SetRemoteSockaddr(const sockaddr_in& addr)
{
#if TARGET_PLATFORM == PLATFORM_WINDOWS

    m_RemoteIp = inet_ntoa(addr.sin_addr);
    m_RemotePort = ntohs(addr.sin_port);
    m_RemoteAddr = GetAddr(m_RemoteIp, m_RemotePort);

#elif TARGET_PLATFORM == PLATFORM_LINUX

    char buf[64] = { '\0' };

    if (!inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf)))
    {
        LOG_ERROR("inet_ntop() failed, %s", GetSockStrError()->c_str());
    }
    else
    {
        m_RemoteIp = buf;
    }

    m_RemotePort = ntohs(addr.sin_port);
    m_RemoteAddr = GetAddr(m_RemoteIp, m_RemotePort);

#endif // TARGET_PLATFORM
}

// 获得发送数据
PacketData* Connection::GetSendPacket()
{
    boost::lock_guard<boost::mutex> lockGuard(m_SendPacketDataMutex);

    if (m_SendPacketDataQueue.size() <= 0)
    {
        return NULL;
    }
    else
    {
        return &m_SendPacketDataQueue.front();
    }
}

// 数据包大小限制
uint32_t Connection::GetPacketSizeLimit()
{
    Acceptor* acceptor = GetAcceptor();
    if (acceptor)
    {
        return acceptor->GetPacketSizeLimit();
    }
    return UINT_MAX;
}

// 处理协议
bool Connection::HandleProto(const char* data, uint32_t size)
{
    BOOST_ASSERT(data);
    BOOST_ASSERT(m_ProtoHandler);

    return m_ProtoHandler->HandleProto(this, data, size);
}

// 设置协议监听器
void Connection::SetProtoListener(ProtoListener* protoListener)
{
    BOOST_ASSERT(m_ProtoHandler);

    m_ProtoHandler->SetProtoListener(protoListener);
}

// 释放套接字
void Connection::FreeSocket()
{
    if (m_Socket != INVALID_SOCKET)
    {
#if TARGET_PLATFORM == PLATFORM_WINDOWS
        closesocket(m_Socket);
#elif TARGET_PLATFORM == PLATFORM_LINUX
        shutdown(m_Socket, SHUT_RDWR);
        close(m_Socket);
#else
        BOOST_STATIC_ASSERT(false);
#endif // TARGET_PLATFORM
        m_Socket = INVALID_SOCKET;
    }

    m_ConnState = CONNECTION_STATE_CLOSED;
}

// 获得连接上下文
ConnectionContext& Connection::GetContext()
{
    return m_ConnContext;
}
