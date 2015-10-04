#ifndef NET_NET_H_
#define NET_NET_H_

#include "Net/Common.h"
#include "Net/Acceptor.h"
#include "Net/AcceptorProxy.h"
#include "Net/Connector.h"
#include "Net/ConnectorProxy.h"
#include "Net/Connection.h"
#include "Net/SocketProxy.h"
#include "Net/ProtoHandler.h"
#include "Net/RpcObject.h"
#include "Net/RpcServer.h"
#include "Net/RpcChannel.h"
#include "Net/RpcController.h"

// 网络
class Net : public SingletonDynamic<Net>
{
public:
    Net();
    virtual ~Net();

    // 初始化
    bool Init();

    // 关闭
    void Shutdown();

    // 更新
    void Update();

    // 接收器
    Acceptor* CreateAcceptor();
    void DestroyAcceptor(Acceptor* acceptor, bool stop = true);

    // 连接器
    Connector* CreateConnector();
    void DestroyConnector(Connector* connector, bool distconn = true);

private:

    // 接收器数组
    std::vector<Acceptor*> m_Acceptors;

    // 连接器数组
    std::vector<Connector*> m_Connectors;
};

// 网络实例
extern Net* g_Net;

#endif // NET_NET_H_
