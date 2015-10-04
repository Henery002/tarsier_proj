#ifndef ECHOCLIENT_H_
#define ECHOCLIENT_H_

#include "EchoClient/Common.h"

// ����Ӧ�ó���
class EchoClient : public SingletonDynamicDerived<EchoClient, App>
{
public:
    EchoClient();
    virtual ~EchoClient();

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

    // ��������ʵ��
    virtual void HandleInputImpl(const std::string& input);

    // �׽���
    SocketProxy& GetSocket();

protected:

    // �׽���
    SocketProxy m_Socket;

    // �����߳�
    boost::thread m_RecvThread;
};

// ����Ӧ�ó���ʵ��
extern EchoClient* g_EchoClient;

#endif // ECHOCLIENT_H_
