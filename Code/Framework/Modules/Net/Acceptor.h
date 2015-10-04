#ifndef NET_ACCEPTOR_H_
#define NET_ACCEPTOR_H_

#include "Net/Common.h"

// 接收器状态类型
enum ACCEPTOR_STATE_TYPE
{
    ACCEPTOR_STATE_INVALID = -1,
    ACCEPTOR_STATE_STARTED, // 已启动
    ACCEPTOR_STATE_STOPED, // 已关闭
    NUM_ACCEPTOR_STATE_TYPE
};

// 接收监听器
class NET_API AcceptorListener
{
public:
    AcceptorListener() {}
    virtual ~AcceptorListener() {}

    // 当接收连接
    virtual void OnAccept(Connection* conn) = 0;

    // 当断开连接
    virtual void OnDisconnect(Connection* conn) = 0;

    // 当接收数据
    virtual void OnRecv(Connection* conn, char* data, uint32_t size) = 0;
};

// 接收器
class NET_API Acceptor
{
public:
    Acceptor();
    virtual ~Acceptor();

    // 启动
    virtual bool Start(const std::string& ip, int port);

    // 关闭
    virtual void Stop();

    // 更新
    virtual void Update();

    // 最大接收连接数
    int GetMaxAcceptNum();
    void SetMaxAcceptNum(int num);

    // 接收监听器
    AcceptorListener* GetAcceptorListener();
    void SetAcceptorListener(AcceptorListener* acceptorListener);

    // 数据包大小限制
    uint32_t GetPacketSizeLimit();
    void SetPacketSizeLimit(uint32_t size);

    // 获得接收器状态
    ACCEPTOR_STATE_TYPE GetAcceptorState();

protected:

    // 最大接收连接数
    int m_MaxAcceptNum;

    // 接收监听器
    AcceptorListener* m_AcceptorListener;

    // 数据包大小限制
    uint32_t m_PacketSizeLimit;

    // 连接器状态
    ACCEPTOR_STATE_TYPE m_AcceptorState;
};

#endif // NET_ACCEPTOR_H_
