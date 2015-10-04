#include "EchoServer/EchoAcceptor.h"

// ���Խ�����
EchoAcceptor::EchoAcceptor(const std::string& ip, int port, bool enable)
    : AcceptorProxy(ip, port, enable)
{

}

EchoAcceptor::~EchoAcceptor()
{

}

// ����������
void EchoAcceptor::OnAccept(Connection* conn)
{
    LOG_INFO("Echo|OnAccept|%s", conn->GetRemoteAddr().c_str());
}

// ���Ͽ�����
void EchoAcceptor::OnDisconnect(Connection* conn)
{
    LOG_INFO("Echo|OnDisconnect|%s", conn->GetRemoteAddr().c_str());
}

// ����������
void EchoAcceptor::OnRecv(Connection* conn, char* data, uint32_t size)
{
    std::string packet(data, size);

    LOG_INFO("Echo|OnRecv|%s|%s", conn->GetRemoteAddr().c_str(), packet.c_str());

    conn->Send(data, size);

    if (packet == "exit")
    {
        conn->Disconnect();
    }
}
