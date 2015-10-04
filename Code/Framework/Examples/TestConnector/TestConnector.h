#ifndef TESTCONNECTOR_H_
#define TESTCONNECTOR_H_

#include "TestConnector/Common.h"
#include "TestConnector/TestBackend.h"

// ���������Գ���
class TestConnector : public SingletonDynamicDerived<TestConnector, App>
{
public:
    TestConnector();
    virtual ~TestConnector();

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
    TestBackend m_TestBackend;

    // �������
    std::queue<std::string> m_InputQueue;
};

// ���������Գ���ʵ��
extern TestConnector* g_TestConnector;

#endif // TESTCONNECTOR_H_
