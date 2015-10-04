#ifndef BACKDOORCLIENT_H_
#define BACKDOORCLIENT_H_

#include "BackdoorClient/Common.h"
#include "BackdoorClient/BackdoorBackend.h"

// ���ſͻ���
class BackdoorClient : public SingletonDynamicDerived<BackdoorClient, App>
{
public:
    BackdoorClient();
    virtual ~BackdoorClient();

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

protected:

    // ���Ժ��
    BackdoorBackend m_BackdoorBackend;
};

// ���ſͻ���ʵ��
extern BackdoorClient* g_BackdoorClient;

#endif // BACKDOORCLIENT_H_
