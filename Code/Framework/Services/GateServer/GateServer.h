#ifndef GATESERVER_GATESERVER_H_
#define GATESERVER_GATESERVER_H_

#include "GateServer/Common.h"

// 网关服务器
class GateServer : public SingletonDynamicDerived<GateServer, App>
{
public:
    GateServer();
    virtual ~GateServer();
};

// 网关服务器实例
extern GateServer* g_GateServer;

#endif // GATESERVER_GATESERVER_H_
