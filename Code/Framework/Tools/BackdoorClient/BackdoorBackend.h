#ifndef BACKDOORBACKEND_H_
#define BACKDOORBACKEND_H_

#include "BackdoorClient/Common.h"

// ���ź��
class BackdoorBackend : public ConnectorProxy, public ConnectorListener, public ProtoListener
{
public:
    BackdoorBackend(const std::string& ip = "", int port = 0, int timeout = -1, bool enable = false);
    virtual ~BackdoorBackend();

    // �����ӳɹ�
    virtual void OnConnected(Connection* conn);

    // ������ʧ��
    virtual void OnConnectFailed(CONNECT_FAILED_TYPE failedType);

    // ���Ͽ�����
    virtual void OnDisconnect(Connection* conn);

    // ����������
    virtual void OnRecv(Connection* conn, char* data, uint32_t size);

    // ���������ݰ�
    virtual void OnPacket(Connection* conn, Packet* packet);

    // ���յ���Ȩ��Ӧ
    bool OnAuthRsp(Connection* conn, Packet* packet, BackdoorAuthRsp* rsp);

    // ���յ������Ӧ
    bool OnInputRsp(Connection* conn, Packet* packet, BackdoorInputRsp* rsp);

    // ����
    const std::string& GetPwd();
    void SetPwd(const std::string& pwd);

    // ����
    const std::string& GetInput();
    void SetInput(const std::string& input);

protected:

    // ����
    std::string m_Pwd;

    // ����
    std::string m_Input;
};

#endif // BACKDOORBACKEND_H_
