#include "Net/ProtoHandler.h"

// Э�鴦����
ProtoHandler::ProtoHandler()
    : m_HandledLen(0)
    , m_MaxPacketSize(0)
    , m_PacketSize(0)
    , m_PacketData(NULL)
    , m_ProtoListener(NULL)
{

}

ProtoHandler::~ProtoHandler()
{
    FREE(m_PacketData);
}

// ����Э��
bool ProtoHandler::HandleProto(Connection* conn, const char* data, uint32_t size)
{
    BOOST_ASSERT(conn);
    BOOST_ASSERT(data);

    uint32_t dataOffset = 0;

    while (dataOffset < size)
    {
        if (m_HandledLen < PACKET_SIZE_LEN)
        {
            uint32_t copyLen = std::min<uint32_t>(size, PACKET_SIZE_LEN - m_HandledLen);

            memcpy(m_PacketSizeBuf + m_HandledLen, data, copyLen);

            dataOffset += copyLen;
            m_HandledLen += copyLen;

            // ��ȡ�����ݰ���С
            if (m_HandledLen == PACKET_SIZE_LEN)
            {
                // �������ݰ���С
                PacketSize packetSize;
                if (!packetSize.ParseFromArray(m_PacketSizeBuf, PACKET_SIZE_LEN))
                {
                    LOG_WARN("packetSize.ParseFromArray failed|%s|%u", conn->GetRemoteAddr().c_str(), m_PacketSize);
                    conn->Disconnect();
                    return false;
                }

                m_PacketSize = packetSize.size();

                if (m_PacketSize <= PACKET_SIZE_LEN)
                {
                    LOG_WARN("Packet size is too small|%s|%u", conn->GetRemoteAddr().c_str(), m_PacketSize);
                    conn->Disconnect();
                    return false;
                }

                // �Ƿ񳬹����ݰ���С����
                if (m_PacketSize > conn->GetPacketSizeLimit())
                {
                    LOG_WARN("Packet size is too large|%s|%u", conn->GetRemoteAddr().c_str(), m_PacketSize);
                    conn->Disconnect();
                    return false;
                }

                // ��������������, �����ڴ�������
                if (m_PacketSize > m_MaxPacketSize)
                {
                    FREE(m_PacketData);
                    m_PacketData = (char*)MALLOC(m_PacketSize, MEM_TAG_NET);
                    m_MaxPacketSize = m_PacketSize;
                }

                memcpy(m_PacketData, &m_PacketSizeBuf, PACKET_SIZE_LEN);

                LOG_TRACE("Got packet size|%s|%u", conn->GetRemoteAddr().c_str(), m_PacketSize);
            }
        }

        if (m_HandledLen >= PACKET_SIZE_LEN)
        {
            BOOST_ASSERT(m_PacketData);

            uint32_t copyLen = std::min<uint32_t>(size, m_PacketSize - m_HandledLen);

            memcpy(m_PacketData + m_HandledLen, data + dataOffset, copyLen);

            dataOffset += copyLen;
            m_HandledLen += copyLen;

            // ��ȡ�����������ݰ�
            if (m_HandledLen == m_PacketSize)
            {
                Packet packet;

                if (!packet.ParseFromArray(m_PacketData, m_PacketSize))
                {
                    LOG_WARN("packet.ParseFromArray failed|%s", conn->GetRemoteAddr().c_str());
                    conn->Disconnect();
                    return false;
                }

                if (m_ProtoListener)
                {
                    LOG_TRACE("Got full packet|%s|%u|%d", conn->GetRemoteAddr().c_str(), packet.size(), packet.cmd());

                    m_ProtoListener->OnPacket(conn, &packet);
                }

                m_HandledLen = 0;
            }
        }
    }

    return true;
}

// Э�������
ProtoListener* ProtoHandler::GetProtoListener()
{
    return m_ProtoListener;
}

void ProtoHandler::SetProtoListener(ProtoListener* protoListener)
{
    m_ProtoListener = protoListener;
}

// ����
void ProtoHandler::Reset()
{
    m_HandledLen = 0;
    m_ProtoListener = NULL;
}
