#ifndef NET_PROTOHANDLER_H_
#define NET_PROTOHANDLER_H_

#include "Net/Common.h"
#include "Net/Connection.h"

// 数据包大小长度, NOTE: wire_type + uint32_t
#define PACKET_SIZE_LEN 5

// 协议监听器
class NET_API ProtoListener
{
public:
    ProtoListener() {}
    virtual ~ProtoListener() {}

    // 当接收数据包
    virtual void OnPacket(Connection* conn, Packet* packet) = 0;

    // 解开数据包
    template<typename T>
    T* UnpackPacket(Connection* conn, Packet* packet)
    {
        BOOST_ASSERT(conn);
        BOOST_ASSERT(packet);

        // NOTE: 为了避免返回值拷贝, 这里使用静态变量, 因为是单线程处理所以没问题
        static T proto;

        if (!proto.ParseFromString(packet->body()))
        {
            LOG_ERROR("proto.ParseFromString failed|%s|%d", conn->GetRemoteAddr().c_str(), packet->cmd());
        }

        return &proto;
    }
};

// 协议处理器
class NET_API ProtoHandler
{
public:
    ProtoHandler();
    ~ProtoHandler();

    // 处理协议
    bool HandleProto(Connection* conn, const char* data, uint32_t size);

    // 协议监听器
    ProtoListener* GetProtoListener();
    void SetProtoListener(ProtoListener* protoListener);

    // 重置
    void Reset();

protected:

    // 已处理的长度
    uint32_t m_HandledLen;

    // 目前处理最大的数据包大小
    uint32_t m_MaxPacketSize;

    // 数据包大小
    uint32_t m_PacketSize;

    // 数据包大小缓冲区
    char m_PacketSizeBuf[PACKET_SIZE_LEN];

    // 数据包数据
    char* m_PacketData;

    // 协议监听器
    ProtoListener* m_ProtoListener;
};

#endif // NET_PROTOHANDLER_H_
