#ifndef BACKDOORACCEPTOR_H_
#define BACKDOORACCEPTOR_H_

#include "App/Common.h"

// ���Ž�����
class BackdoorAcceptor : public AcceptorProxy, public AcceptorListener, public ProtoListener, public RpcServer
{
public:
    BackdoorAcceptor(const std::string& ip = "", int port = 0, bool enable = false);
    virtual ~BackdoorAcceptor();

    // ����������
    virtual void OnAccept(Connection* conn);

    // ���Ͽ�����
    virtual void OnDisconnect(Connection* conn);

    // ����������
    virtual void OnRecv(Connection* conn, char* data, uint32_t size);

    // ���������ݰ�
    virtual void OnPacket(Connection* conn, Packet* packet);

    // ���յ���Ȩ����
    bool OnAuthReq(Connection* conn, Packet* packet, BackdoorAuthReq* req);

    // ���յ���������
    bool OnInputReq(Connection* conn, Packet* packet, BackdoorInputReq* req);

    // ����
    const std::string& GetPwd();
    void SetPwd(const std::string& pwd);

protected:

    // ����
    std::string m_Pwd;
};

#endif // BACKDOORACCEPTOR_H_
