#ifndef ECHOACCEPTOR_H_
#define ECHOACCEPTOR_H_

#include "EchoServer/Common.h"

// ���Խ�����
class EchoAcceptor : public AcceptorProxy, public AcceptorListener
{
public:
    EchoAcceptor(const std::string& ip = "", int port = 0, bool enable = false);
    virtual ~EchoAcceptor();

    // ����������
    virtual void OnAccept(Connection* conn);

    // ���Ͽ�����
    virtual void OnDisconnect(Connection* conn);

    // ����������
    virtual void OnRecv(Connection* conn, char* data, uint32_t size);
};

#endif // ECHOACCEPTOR_H_
