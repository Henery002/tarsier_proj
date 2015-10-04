#ifndef NET_SELECTCONNECTION_H_
#define NET_SELECTCONNECTION_H_

#include "Net/Connection.h"

// select ����
class NET_API SelectConnection : public Connection
{
public:
    SelectConnection();
    virtual ~SelectConnection();

    // �Ͽ�����, NOTE: �ᱣ֤ȫ�����ݷ�����ɲ������Ͽ�
    virtual void Disconnect();

    // ����ɷ�������
    virtual void OnSendComplete();

    // ����Ͽ�����
    virtual void HandleDisconnect();

    // ���������
    virtual Connector* GetConnector();

    // ����������
    void SetConnector(Connector* conn);

protected:

    // ������
    Connector* m_Connector;
};

#endif // NET_SELECTCONNECTION_H_
