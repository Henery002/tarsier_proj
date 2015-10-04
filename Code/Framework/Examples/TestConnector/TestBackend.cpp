#include "TestConnector/TestBackend.h"

// 测试后端
TestBackend::TestBackend(const std::string& ip, int port, int timeout, bool enable)
    : ConnectorProxy(ip, port, timeout, enable)
{

}

TestBackend::~TestBackend()
{

}

// 当连接成功
void TestBackend::OnConnected(Connection* conn)
{
    LOG_INFO("Test|OnConnected|%s", conn->GetRemoteAddr().c_str());
}

// 当连接失败
void TestBackend::OnConnectFailed(CONNECT_FAILED_TYPE failedType)
{
    LOG_INFO("Test|OnConnectFailed|%d", failedType);
}

// 当断开连接
void TestBackend::OnDisconnect(Connection* conn)
{
    LOG_INFO("Test|OnDisconnect|%s", conn->GetRemoteAddr().c_str());
}

// 当接收数据
void TestBackend::OnRecv(Connection* conn, char* data, uint32_t size)
{
    std::string packet(data, size);

    LOG_INFO("Test|OnRecv|%s|%s", conn->GetRemoteAddr().c_str(), packet.c_str());

    if (packet == "bye")
    {
        conn->Disconnect();
    }
}
