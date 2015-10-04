#include "GateServer/GateServer.h"

// 网关服务器实例
GateServer* g_GateServer = NULL;

// 网关服务器
GateServer::GateServer()
{
    g_GateServer = this;

    m_AppName = "GateServer";
    m_BackdoorAcceptor.SetPort(GATESERVER_DEFAULT_BACKDOOR_PORT);
}

GateServer::~GateServer()
{
    g_GateServer = NULL;
}
