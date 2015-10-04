#include "App/BackdoorAcceptor.h"
#include "App/App.h"

// ���Ž�����
BackdoorAcceptor::BackdoorAcceptor(const std::string& ip, int port, bool enable)
    : AcceptorProxy(ip, port, enable)
    , m_Pwd()
{

}

BackdoorAcceptor::~BackdoorAcceptor()
{

}

// ����������
void BackdoorAcceptor::OnAccept(Connection* conn)
{
    LOG_INFO("Backdoor|OnAccept|%s", conn->GetRemoteAddr().c_str());

    conn->SetProtoListener(this);
}

// ���Ͽ�����
void BackdoorAcceptor::OnDisconnect(Connection* conn)
{
    LOG_INFO("Backdoor|OnDisconnect|%s", conn->GetRemoteAddr().c_str());
}

// ����������
void BackdoorAcceptor::OnRecv(Connection* conn, char* data, uint32_t size)
{
    conn->HandleProto(data, size);
}

// ���������ݰ�
void BackdoorAcceptor::OnPacket(Connection* conn, Packet* packet)
{
    bool ok = true;

    switch (packet->cmd())
    {
    case CMD_BACKDOOR_AUTH_REQ:
        ok = OnAuthReq(conn, packet, UnpackPacket<BackdoorAuthReq>(conn, packet));
        break;
    case CMD_BACKDOOR_INPUT_REQ:
        ok = OnInputReq(conn, packet, UnpackPacket<BackdoorInputReq>(conn, packet));
        break;
    default:
        LOG_WARN("Backdoor|Invalid cmd|%s|%d", conn->GetRemoteAddr().c_str(), packet->cmd());
        conn->Disconnect();
        break;
    }

    if (!ok)
    {
        LOG_WARN("Backdoor|Handle cmd failed|%s|%d", conn->GetRemoteAddr().c_str(), packet->cmd());
        conn->Disconnect();
    }
}

// ���յ���Ȩ����
bool BackdoorAcceptor::OnAuthReq(Connection* conn, Packet* packet, BackdoorAuthReq* req)
{
    if (req->pwd() != m_Pwd)
    {
        LOG_INFO("Backdoor|Auth wrong pwd|%s", conn->GetRemoteAddr().c_str());
        conn->SendPacket(CMD_BACKDOOR_AUTH_RSP, NULL, ERR_BACKDOOR_INCORRECT_PWD);
        return false;
    }

    LOG_INFO("Backdoor|Auth ok|%s", conn->GetRemoteAddr().c_str());

    conn->GetContext().ok = true;

    conn->SendPacket(CMD_BACKDOOR_AUTH_RSP);

    return true;
}

// ���յ���������
bool BackdoorAcceptor::OnInputReq(Connection* conn, Packet* packet, BackdoorInputReq* req)
{
    if (!conn->GetContext().ok)
    {
        LOG_INFO("Backdoor|Input not auth|%s|%s", conn->GetRemoteAddr().c_str(), req->input().c_str());
        conn->SendPacket(CMD_BACKDOOR_INPUT_RSP, NULL, ERR_BACKDOOR_UNAUTHORIZED);
        return false;
    }

    LOG_INFO("Backdoor|Input ok|%s|%s", conn->GetRemoteAddr().c_str(), req->input().c_str());

    g_App->AddInput(req->input());

    conn->SendPacket(CMD_BACKDOOR_INPUT_RSP);

    return true;
}

// ����
const std::string& BackdoorAcceptor::GetPwd()
{
    return m_Pwd;
}

void BackdoorAcceptor::SetPwd(const std::string& pwd)
{
    m_Pwd = pwd;
}
