#ifndef BACKDOORBACKEND_H_
#define BACKDOORBACKEND_H_

#include "BackdoorClient/Common.h"

// 后门后端
class BackdoorBackend : public ConnectorProxy, public ConnectorListener, public ProtoListener
{
public:
    BackdoorBackend(const std::string& ip = "", int port = 0, int timeout = -1, bool enable = false);
    virtual ~BackdoorBackend();

    // 当连接成功
    virtual void OnConnected(Connection* conn);

    // 当连接失败
    virtual void OnConnectFailed(CONNECT_FAILED_TYPE failedType);

    // 当断开连接
    virtual void OnDisconnect(Connection* conn);

    // 当接收数据
    virtual void OnRecv(Connection* conn, char* data, uint32_t size);

    // 当接收数据包
    virtual void OnPacket(Connection* conn, Packet* packet);

    // 当收到授权回应
    bool OnAuthRsp(Connection* conn, Packet* packet, BackdoorAuthRsp* rsp);

    // 当收到输入回应
    bool OnInputRsp(Connection* conn, Packet* packet, BackdoorInputRsp* rsp);

    // 密码
    const std::string& GetPwd();
    void SetPwd(const std::string& pwd);

    // 输入
    const std::string& GetInput();
    void SetInput(const std::string& input);

protected:

    // 密码
    std::string m_Pwd;

    // 输入
    std::string m_Input;
};

#endif // BACKDOORBACKEND_H_
