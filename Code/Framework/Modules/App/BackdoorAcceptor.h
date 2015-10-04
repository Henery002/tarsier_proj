#ifndef BACKDOORACCEPTOR_H_
#define BACKDOORACCEPTOR_H_

#include "App/Common.h"

// 后门接收器
class BackdoorAcceptor : public AcceptorProxy, public AcceptorListener, public ProtoListener, public RpcServer
{
public:
    BackdoorAcceptor(const std::string& ip = "", int port = 0, bool enable = false);
    virtual ~BackdoorAcceptor();

    // 当接收连接
    virtual void OnAccept(Connection* conn);

    // 当断开连接
    virtual void OnDisconnect(Connection* conn);

    // 当接收数据
    virtual void OnRecv(Connection* conn, char* data, uint32_t size);

    // 当接收数据包
    virtual void OnPacket(Connection* conn, Packet* packet);

    // 当收到授权请求
    bool OnAuthReq(Connection* conn, Packet* packet, BackdoorAuthReq* req);

    // 当收到输入请求
    bool OnInputReq(Connection* conn, Packet* packet, BackdoorInputReq* req);

    // 密码
    const std::string& GetPwd();
    void SetPwd(const std::string& pwd);

protected:

    // 密码
    std::string m_Pwd;
};

#endif // BACKDOORACCEPTOR_H_
