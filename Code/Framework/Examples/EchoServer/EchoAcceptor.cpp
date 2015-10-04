#include "EchoServer/EchoAcceptor.h"

// 回显接收器
EchoAcceptor::EchoAcceptor(const std::string& ip, int port, bool enable)
    : AcceptorProxy(ip, port, enable)
{

}

EchoAcceptor::~EchoAcceptor()
{

}

// 当接收连接
void EchoAcceptor::OnAccept(Connection* conn)
{
    LOG_INFO("Echo|OnAccept|%s", conn->GetRemoteAddr().c_str());
}

// 当断开连接
void EchoAcceptor::OnDisconnect(Connection* conn)
{
    LOG_INFO("Echo|OnDisconnect|%s", conn->GetRemoteAddr().c_str());
}

// 当接收数据
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
