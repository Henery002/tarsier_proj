#ifndef NET_CONNECTOR_H_
#define NET_CONNECTOR_H_

#include "Net/Common.h"
#include "Net/SelectConnection.h"

// 连接器状态类型
enum CONNECTOR_STATE_TYPE
{
    CONNECTOR_STATE_INVALID = -1,
    CONNECTOR_STATE_CONNECTING, // 连接中
    CONNECTOR_STATE_CONNECTED, // 已连接
    CONNECTOR_STATE_CLOSED, // 已关闭
    CONNECTOR_STATE_CLOSE_PENDING, // 关闭待处理
    NUM_CONNECTOR_STATE_TYPE
};

// 连接失败类型
enum CONNECT_FAILED_TYPE
{
    CONNECT_FAILED_INVALID = -1,
    CONNECT_FAILED_TIMEDOUT, // 超时
    CONNECT_FAILED_CONNREFUSED, // 目标主动拒绝
    CONNECT_FAILED_UNREACHABLE, // 目标不可达
    CONNECT_FAILED_OTHERS, // 其他
    NUM_CONNECT_FAILED_TYPE
};

// 连接器操作类型
enum CONNECTOR_OPERATION_TYPE
{
    CONNECTOR_OPERATION_INVALID = -1,
    CONNECTOR_OPERATION_CONNECTED, // 连接成功
    CONNECTOR_OPERATION_CONNECT_FAILED, // 连接失败
    CONNECTOR_OPERATION_DISCONNECT, // 断开连接
    CONNECTOR_OPERATION_RECV, // 接收数据
    NUM_CONNECTOR_OPERATION_TYPE
};

// 连接器操作数据
struct NET_API ConnectorOperationData
{
    CONNECTOR_OPERATION_TYPE opType;
    Connection* conn;
    char* data;
    uint32_t size;
    CONNECT_FAILED_TYPE failedType;

    ConnectorOperationData()
        : opType(CONNECTOR_OPERATION_INVALID)
        , conn(NULL)
        , data(NULL)
        , size(0)
        , failedType(CONNECT_FAILED_INVALID)
    {

    }
};

// 连接监听器
class NET_API ConnectorListener
{
public:
    ConnectorListener() {}
    virtual ~ConnectorListener() {}

    // 当连接成功
    virtual void OnConnected(Connection* conn) = 0;

    // 当连接失败
    virtual void OnConnectFailed(CONNECT_FAILED_TYPE failedType) = 0;

    // 当断开连接
    virtual void OnDisconnect(Connection* conn) = 0;

    // 当接收数据
    virtual void OnRecv(Connection* conn, char* data, uint32_t size) = 0;
};

// 连接器
class NET_API Connector
{
public:
    Connector();
    ~Connector();

    // 连接, timeout 单位为秒, -1表示使用系统超时时间(75秒)
    void Connect();
    void Connect(const std::string& ip, int port, int timeout = -1);

    // 断开连接, NOTE: 不会保证全部数据发送完成才真正断开
    void Disconnect();

    // 更新
    void Update();

    // 发送数据包
    void SendPacket(int cmd, const std::string* body = NULL, int err = 0, const std::string* extra = NULL);

    template<typename T>
    void SendPacket(int cmd, const T* proto = NULL, int err = 0, const std::string* extra = NULL)
    {
        BOOST_ASSERT(m_Connection);
        BOOST_ASSERT(m_ConnectorState == CONNECTOR_STATE_CONNECTED);

        m_Connection->SendPacket(cmd, proto, err, extra);
    }

    // 发送数据
    void Send(const char* data, uint32_t size);

    // 获得IP
    const std::string& GetIp();

    // 获得端口
    int GetPort();

    // 获得超时时间
    int GetTimeout();

    // 连接监听器
    ConnectorListener* GetConnectorListener();
    void SetConnectorListener(ConnectorListener* connectorListener);

    // 获得连接器状态
    CONNECTOR_STATE_TYPE GetConnectorState();

    // 获得接收句柄集
    fd_set* GetRecvFdSet();

    // 获得发送句柄集
    fd_set* GetSendFdSet();

    // 获得异常句柄集
    fd_set* GetExceptFdSet();

    // 获得连接
    Connection* GetConnection();

    // 压入操作数据
    void PushOperation(const ConnectorOperationData& opData);

    // 处理断开连接
    void HandleDisconnect(Connection* conn);

protected:

    // 断开连接实现
    void DisconnectImpl();

protected:

    // IP
    std::string m_Ip;

    // 端口
    int m_Port;

    // 超时时间
    int m_Timeout;

    // 连接
    SelectConnection* m_Connection;

    // 连接监听器
    ConnectorListener* m_ConnectorListener;

    // 连接器状态
    CONNECTOR_STATE_TYPE m_ConnectorState;

    // 工作线程
    boost::thread m_WorkerThread;

    // 接收句柄集
    fd_set m_RecvFdSet;

    // 发送句柄集
    fd_set m_SendFdSet;

    // 异常句柄集
    fd_set m_ExceptFdSet;

    // 操作队列
    std::vector<ConnectorOperationData> m_ConnectorOperationData;

    // 操作队列互斥体
    boost::mutex m_ConnectorOperationDataMutex;
};

#endif // NET_CONNECTOR_H_
