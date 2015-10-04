#ifndef TESTBACKEND_H_
#define TESTBACKEND_H_

#include "TestConnector/Common.h"

// 测试后端
class TestBackend : public ConnectorProxy, public ConnectorListener
{
public:
    TestBackend(const std::string& ip = "", int port = 0, int timeout = -1, bool enable = false);
    virtual ~TestBackend();

    // 当连接成功
    virtual void OnConnected(Connection* conn);

    // 当连接失败
    virtual void OnConnectFailed(CONNECT_FAILED_TYPE failedType);

    // 当断开连接
    virtual void OnDisconnect(Connection* conn);

    // 当接收数据
    virtual void OnRecv(Connection* conn, char* data, uint32_t size);
};

#endif // TESTBACKEND_H_
