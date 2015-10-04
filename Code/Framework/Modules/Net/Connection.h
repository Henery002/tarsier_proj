#ifndef NET_CONNECTION_H_
#define NET_CONNECTION_H_

#include "Net/Common.h"

// 连接状态类型
enum CONNECTION_STATE_TYPE
{
    CONNECTION_STATE_INVALID = -1,
    CONNECTION_STATE_CLOSED, // 已关闭
    CONNECTION_STATE_CLOSE_PENDING, // 关闭待处理
    CONNECTION_STATE_ESTABLISHED, // 已连接
    NUM_CONNECTION_STATE_TYPE
};

// 连接上下文
struct NET_API ConnectionContext
{
    bool ok;
    int n;
    void* ptr;
    uint32_t id;
    uint64_t id64;
    uint32_t flags;
    std::string buf;

    ConnectionContext()
        : ok(false)
        , n(0)
        , ptr(NULL)
        , id(0)
        , id64(0)
        , flags(0)
        , buf()
    {

    }

    void Reset()
    {
        ok = false;
        n = 0;
        ptr = NULL;
        id = 0;
        id64 = 0;
        flags = 0;
        buf.clear();
    }
};

// 连接
class NET_API Connection
{
public:
    Connection();
    virtual ~Connection();

    // 发送数据包
    void SendPacket(int cmd, const std::string* body = NULL, int err = 0, const std::string* extra = NULL);

    template<typename T>
    void SendPacket(int cmd, const T* proto = NULL, int err = 0, const std::string* extra = NULL)
    {
        if (proto)
        {
            std::string body;

            if (!proto->SerializeToString(&body))
            {
                LOG_ERROR("proto.SerializeToString failed|%s|%d", GetRemoteAddr().c_str(), cmd);
            }

            SendPacket(cmd, &body, err, extra);
        }
        else
        {
            SendPacket(cmd, NULL, err, extra);
        }
    }

    // 发送数据
    virtual void Send(const char* data, uint32_t size);

    // 断开连接, NOTE: 会保证全部数据发送完成才真正断开
    virtual void Disconnect();

    // 当分配
    virtual void OnAlloc();

    // 当释放
    virtual void OnFree();

    // 当接收连接
    virtual void OnAccept();

    // 当连接成功
    virtual void OnConnected();

    // 当接收数据
    virtual void OnRecv(char* data, uint32_t size);

    // 当发送数据
    virtual void OnSend(uint32_t size);

    // 当完成发送数据
    virtual void OnSendComplete();

    // 当断开连接
    virtual void OnDisconnect();

    // 投递接收操作
    virtual void PostRecvOp();

    // 投递发送操作
    virtual void PostSendOp();

    // 处理断开连接
    virtual void HandleDisconnect();

    // 获得接收器
    virtual Acceptor* GetAcceptor();

    // 获得连接器
    virtual Connector* GetConnector();

    // 套接字
    SOCKET GetSocket();
    void SetSocket(SOCKET socket);

    // 获得连接 ID
    uint64_t GetConnId();

    // 获得连接状态
    CONNECTION_STATE_TYPE GetConnState();

    // 获得本地 IP
    const std::string& GetLocalIp();

    // 获得本地端口
    int GetLocalPort();

    // 本地地址, IP:PORT
    const std::string& GetLocalAddr();
    void SetLocalSockaddr(const sockaddr_in& addr);

    // 获得远端 IP
    const std::string& GetRemoteIp();

    // 获得远端端口
    int GetRemotePort();

    // 获得远端地址, IP:PORT
    const std::string& GetRemoteAddr();
    void SetRemoteSockaddr(const sockaddr_in& addr);

    // 获得发送数据
    PacketData* GetSendPacket();

    // 数据包大小限制
    uint32_t GetPacketSizeLimit();

    // 处理协议
    bool HandleProto(const char* data, uint32_t size);

    // 设置协议监听器
    void SetProtoListener(ProtoListener* protoListener);

    // 获得连接上下文
    ConnectionContext& GetContext();

    // 释放套接字
    void FreeSocket();

protected:

    // 套接字
    SOCKET m_Socket;

    // 连接 ID
    uint64_t m_ConnId;

    // 连接状态
    CONNECTION_STATE_TYPE m_ConnState;

    // 本地 IP
    std::string m_LocalIp;

    // 本地端口
    int m_LocalPort;

    // 本地地址, IP:PORT
    std::string m_LocalAddr;

    // 远端 IP
    std::string m_RemoteIp;

    // 远端端口
    int m_RemotePort;

    // 远端地址, IP:PORT
    std::string m_RemoteAddr;

    // 数据包信息接收队列
    PacketDataQueue m_RecvPacketDataQueue;

    // 数据包信息接收队列互斥体
    boost::mutex m_RecvPacketDataMutex;

    // 数据包信息发送队列
    PacketDataQueue m_SendPacketDataQueue;

    // 数据包信息发送队列互斥体
    boost::mutex m_SendPacketDataMutex;

    // 协议处理器
    ProtoHandler* m_ProtoHandler;

    // 连接上下文
    ConnectionContext m_ConnContext;
};

#endif // NET_CONNECTION_H_
