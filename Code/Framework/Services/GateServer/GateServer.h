#ifndef GATESERVER_GATESERVER_H_
#define GATESERVER_GATESERVER_H_

#include "GateServer/Common.h"

// ���ط�����
class GateServer : public SingletonDynamicDerived<GateServer, App>
{
public:
    GateServer();
    virtual ~GateServer();
};

// ���ط�����ʵ��
extern GateServer* g_GateServer;

#endif // GATESERVER_GATESERVER_H_
