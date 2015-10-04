#include "BackdoorClient/BackdoorBackend.h"
#include "BackdoorClient/BackdoorClient.h"

// 后门后端
BackdoorBackend::BackdoorBackend(const std::string& ip, int port, int timeout, bool enable)
    : ConnectorProxy(ip, port, timeout, enable)
    , m_Pwd()
    , m_Input()
{

}

BackdoorBackend::~BackdoorBackend()
{

}

// 当连接成功
void BackdoorBackend::OnConnected(Connection* conn)
{
    LOG_INFO("Backdoor|OnConnected|%s", conn->GetRemoteAddr().c_str());

    conn->SetProtoListener(this);

    BackdoorAuthReq req;
    req.set_pwd(m_Pwd);
    conn->SendPacket(CMD_BACKDOOR_AUTH_REQ, &req);
}

// 当连接失败
void BackdoorBackend::OnConnectFailed(CONNECT_FAILED_TYPE failedType)
{
    LOG_INFO("Backdoor|OnConnectFailed|%d", failedType);

    BackdoorClient::Inst()->OnStopSignal();
}

// 当断开连接
void BackdoorBackend::OnDisconnect(Connection* conn)
{
    LOG_INFO("Backdoor|OnDisconnect|%s", conn->GetRemoteAddr().c_str());

    BackdoorClient::Inst()->OnStopSignal();
}

// 当接收数据
void BackdoorBackend::OnRecv(Connection* conn, char* data, uint32_t size)
{
    conn->HandleProto(data, size);
}

// 当接收数据包
void BackdoorBackend::OnPacket(Connection* conn, Packet* packet)
{
    if (packet->err())
    {
        switch (packet->err())
        {
        case ERR_BACKDOOR_INCORRECT_PWD:
            LOG_ERROR("Incorrect password");
            break;
        case ERR_BACKDOOR_UNAUTHORIZED:
            LOG_ERROR("Unauthorized");
            break;
        default:
            LOG_ERROR("Err|%d", packet->err());
            break;
        }

        BackdoorClient::Inst()->OnStopSignal();
        return;
    }

    bool ok = true;

    switch (packet->cmd())
    {
    case CMD_BACKDOOR_AUTH_RSP:
        ok = OnAuthRsp(conn, packet, UnpackPacket<BackdoorAuthRsp>(conn, packet));
        break;
    case CMD_BACKDOOR_INPUT_RSP:
        ok = OnInputRsp(conn, packet, UnpackPacket<BackdoorInputRsp>(conn, packet));
        break;
    default:
        LOG_WARN("Backdoor|Invalid cmd|%s|%d", conn->GetRemoteAddr().c_str(), packet->cmd());
        break;
    }
}

// 当收到授权回应
bool BackdoorBackend::OnAuthRsp(Connection* conn, Packet* packet, BackdoorAuthRsp* rsp)
{
    if (!m_Input.empty())
    {
        BackdoorInputReq req;
        req.set_input(m_Input);
        conn->SendPacket(CMD_BACKDOOR_INPUT_REQ, &req);
    }

    return true;
}

// 当收到输入回应
bool BackdoorBackend::OnInputRsp(Connection* conn, Packet* packet, BackdoorInputRsp* rsp)
{
    if (!m_Input.empty())
    {
        BackdoorClient::Inst()->OnStopSignal();
    }

    return true;
}

// 密码
const std::string& BackdoorBackend::GetPwd()
{
    return m_Pwd;
}

void BackdoorBackend::SetPwd(const std::string& pwd)
{
    m_Pwd = pwd;
}

// 输入
const std::string& BackdoorBackend::GetInput()
{
    return m_Input;
}

void BackdoorBackend::SetInput(const std::string& input)
{
    m_Input = input;
}
