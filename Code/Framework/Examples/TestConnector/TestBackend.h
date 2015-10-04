#ifndef TESTBACKEND_H_
#define TESTBACKEND_H_

#include "TestConnector/Common.h"

// ���Ժ��
class TestBackend : public ConnectorProxy, public ConnectorListener
{
public:
    TestBackend(const std::string& ip = "", int port = 0, int timeout = -1, bool enable = false);
    virtual ~TestBackend();

    // �����ӳɹ�
    virtual void OnConnected(Connection* conn);

    // ������ʧ��
    virtual void OnConnectFailed(CONNECT_FAILED_TYPE failedType);

    // ���Ͽ�����
    virtual void OnDisconnect(Connection* conn);

    // ����������
    virtual void OnRecv(Connection* conn, char* data, uint32_t size);
};

#endif // TESTBACKEND_H_
