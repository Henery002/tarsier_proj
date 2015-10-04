#include "TestConnector/TestBackend.h"

// ���Ժ��
TestBackend::TestBackend(const std::string& ip, int port, int timeout, bool enable)
    : ConnectorProxy(ip, port, timeout, enable)
{

}

TestBackend::~TestBackend()
{

}

// �����ӳɹ�
void TestBackend::OnConnected(Connection* conn)
{
    LOG_INFO("Test|OnConnected|%s", conn->GetRemoteAddr().c_str());
}

// ������ʧ��
void TestBackend::OnConnectFailed(CONNECT_FAILED_TYPE failedType)
{
    LOG_INFO("Test|OnConnectFailed|%d", failedType);
}

// ���Ͽ�����
void TestBackend::OnDisconnect(Connection* conn)
{
    LOG_INFO("Test|OnDisconnect|%s", conn->GetRemoteAddr().c_str());
}

// ����������
void TestBackend::OnRecv(Connection* conn, char* data, uint32_t size)
{
    std::string packet(data, size);

    LOG_INFO("Test|OnRecv|%s|%s", conn->GetRemoteAddr().c_str(), packet.c_str());

    if (packet == "bye")
    {
        conn->Disconnect();
    }
}
