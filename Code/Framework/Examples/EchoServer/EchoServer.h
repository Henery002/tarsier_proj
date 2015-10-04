#ifndef ECHOSERVER_H_
#define ECHOSERVER_H_

#include "EchoServer/Common.h"
#include "EchoServer/EchoAcceptor.h"

// ���Է�����
class EchoServer : public SingletonDynamicDerived<EchoServer, App>
{
public:
    EchoServer();
    virtual ~EchoServer();

    // ��ʼ��ʵ��
    virtual bool InitImpl();

    // ע��ѡ��ʵ��
    virtual bool RegisterOptionsImpl();

    // ����ѡ��ʵ��
    virtual bool ParseOptionsImpl();

    // �����������ص�ѡ��ʵ��
    virtual bool ParseReloadableOptionsImpl(bool reload);

    // �ر�ʵ��
    virtual void ShutdownImpl();

    // ����ʵ��
    virtual void UpdateImpl();

    // ֹͣʵ��
    virtual void StopImpl();

    // ��������ʵ��
    virtual void ReloadImpl();

protected:

    // ���Խ�����
    EchoAcceptor m_EchoAcceptor;
};

// ���Է�����ʵ��
extern EchoServer* g_EchoServer;

#endif // ECHOSERVER_H_
