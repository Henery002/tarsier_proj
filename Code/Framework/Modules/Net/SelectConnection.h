#ifndef NET_SELECTCONNECTION_H_
#define NET_SELECTCONNECTION_H_

#include "Net/Connection.h"

// select 连接
class NET_API SelectConnection : public Connection
{
public:
    SelectConnection();
    virtual ~SelectConnection();

    // 断开连接, NOTE: 会保证全部数据发送完成才真正断开
    virtual void Disconnect();

    // 当完成发送数据
    virtual void OnSendComplete();

    // 处理断开连接
    virtual void HandleDisconnect();

    // 获得连接器
    virtual Connector* GetConnector();

    // 设置连接器
    void SetConnector(Connector* conn);

protected:

    // 连接器
    Connector* m_Connector;
};

#endif // NET_SELECTCONNECTION_H_
