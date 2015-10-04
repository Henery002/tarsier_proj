#ifndef ECHOACCEPTOR_H_
#define ECHOACCEPTOR_H_

#include "EchoServer/Common.h"

// 回显接收器
class EchoAcceptor : public AcceptorProxy, public AcceptorListener
{
public:
    EchoAcceptor(const std::string& ip = "", int port = 0, bool enable = false);
    virtual ~EchoAcceptor();

    // 当接收连接
    virtual void OnAccept(Connection* conn);

    // 当断开连接
    virtual void OnDisconnect(Connection* conn);

    // 当接收数据
    virtual void OnRecv(Connection* conn, char* data, uint32_t size);
};

#endif // ECHOACCEPTOR_H_
