#include "GateServer/GateServer.h"

// ���ط�����ʵ��
GateServer* g_GateServer = NULL;

// ���ط�����
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
