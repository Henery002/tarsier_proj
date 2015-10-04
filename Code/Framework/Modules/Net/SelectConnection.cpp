#include "Net/SelectConnection.h"
#include "Net/Connector.h"

// select ����
SelectConnection::SelectConnection()
    : m_Connector(NULL)
{

}

SelectConnection::~SelectConnection()
{

}

// �Ͽ�����, NOTE: �ᱣ֤ȫ�����ݷ�����ɲ������Ͽ�
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

// ����ɷ�������
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

// ����Ͽ�����
void SelectConnection::HandleDisconnect()
{
    BOOST_ASSERT(m_Connector);

    // NOTE: select ģ��, �׽���ֻ���� select �����߳��ͷ�, ��������ֻ��״̬
    m_ConnState = CONNECTION_STATE_CLOSED;
}

// ���������
Connector* SelectConnection::GetConnector()
{
    return m_Connector;
}

// ����������
void SelectConnection::SetConnector(Connector* conn)
{
    m_Connector = conn;
}
