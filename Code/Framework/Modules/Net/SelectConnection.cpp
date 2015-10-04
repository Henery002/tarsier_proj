#include "Net/SelectConnection.h"
#include "Net/Connector.h"

// select 连接
SelectConnection::SelectConnection()
    : m_Connector(NULL)
{

}

SelectConnection::~SelectConnection()
{

}

// 断开连接, NOTE: 会保证全部数据发送完成才真正断开
void SelectConnection::Disconnect()
{
    BOOST_ASSERT(m_Connector);

    CONNECTION_STATE_TYPE preConnState = m_ConnState;

    Connection::Disconnect();

    if (preConnState != m_ConnState && m_ConnState == CONNECTION_STATE_CLOSED)
    {
        m_Connector->HandleDisconnect(this);
    }
}

// 当完成发送数据
void SelectConnection::OnSendComplete()
{
    BOOST_ASSERT(m_Connector);

    Connection::OnSendComplete();

    if (m_ConnState == CONNECTION_STATE_CLOSE_PENDING)
    {
        ConnectorOperationData opData;
        opData.opType = CONNECTOR_OPERATION_DISCONNECT;
        opData.conn = this;
        m_Connector->PushOperation(opData);
    }
}

// 处理断开连接
void SelectConnection::HandleDisconnect()
{
    BOOST_ASSERT(m_Connector);

    // NOTE: select 模型, 套接字只能在 select 所在线程释放, 所以这里只改状态
    m_ConnState = CONNECTION_STATE_CLOSED;
}

// 获得连接器
Connector* SelectConnection::GetConnector()
{
    return m_Connector;
}

// 设置连接器
void SelectConnection::SetConnector(Connector* conn)
{
    m_Connector = conn;
}
