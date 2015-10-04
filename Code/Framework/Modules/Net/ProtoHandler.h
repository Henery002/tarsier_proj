#ifndef NET_PROTOHANDLER_H_
#define NET_PROTOHANDLER_H_

#include "Net/Common.h"
#include "Net/Connection.h"

// ���ݰ���С����, NOTE: wire_type + uint32_t
#define PACKET_SIZE_LEN 5

// Э�������
class NET_API ProtoListener
{
public:
    ProtoListener() {}
    virtual ~ProtoListener() {}

    // ���������ݰ�
    virtual void OnPacket(Connection* conn, Packet* packet) = 0;

    // �⿪���ݰ�
    template<typename T>
    T* UnpackPacket(Connection* conn, Packet* packet)
    {
        BOOST_ASSERT(conn);
        BOOST_ASSERT(packet);

        // NOTE: Ϊ�˱��ⷵ��ֵ����, ����ʹ�þ�̬����, ��Ϊ�ǵ��̴߳�������û����
        static T proto;

        if (!proto.ParseFromString(packet->body()))
        {
            LOG_ERROR("proto.ParseFromString failed|%s|%d", conn->GetRemoteAddr().c_str(), packet->cmd());
        }

        return &proto;
    }
};

// Э�鴦����
class NET_API ProtoHandler
{
public:
    ProtoHandler();
    ~ProtoHandler();

    // ����Э��
    bool HandleProto(Connection* conn, const char* data, uint32_t size);

    // Э�������
    ProtoListener* GetProtoListener();
    void SetProtoListener(ProtoListener* protoListener);

    // ����
    void Reset();

protected:

    // �Ѵ���ĳ���
    uint32_t m_HandledLen;

    // Ŀǰ�����������ݰ���С
    uint32_t m_MaxPacketSize;

    // ���ݰ���С
    uint32_t m_PacketSize;

    // ���ݰ���С������
    char m_PacketSizeBuf[PACKET_SIZE_LEN];

    // ���ݰ�����
    char* m_PacketData;

    // Э�������
    ProtoListener* m_ProtoListener;
};

#endif // NET_PROTOHANDLER_H_
