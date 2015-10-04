#include "TestConnector/TestConnector.h"

// ���������Գ���ʵ��
TestConnector* g_TestConnector = NULL;

// ���������Գ���
TestConnector::TestConnector()
    : m_TestBackend("127.0.0.1", TESTBACKEND_DEFAULT_SERVICE_PORT, -1, true)
    , m_InputQueue()
{
    g_TestConnector = this;

    m_AppName = "TestConnector";
}

TestConnector::~TestConnector()
{
    g_TestConnector = NULL;
}

// ��ʼ��ʵ��
bool TestConnector::InitImpl()
{
    if (m_TestBackend.IsEnable())
    {
        if (!m_TestBackend.CreateConnector())
        {
            return false;
        }

        m_TestBackend.GetConnector()->SetConnectorListener(&m_TestBackend);
    }

    return true;
}

// ע��ѡ��ʵ��
bool TestConnector::RegisterOptionsImpl()
{
    m_OptionsDesc.add_options()
    ("remote-ip", boost::program_options::value<std::string>(), "Specify remote ip.")
    ("remote-port", boost::program_options::value<int>(), "Specify remote port.")
    ("remote-timeout", boost::program_options::value<int>(), "Specify remote timeout.")
    ("remote-enable", boost::program_options::value<bool>(), "Specify remote is enabled.")
    ;

    return true;
}

// ����ѡ��ʵ��
bool TestConnector::ParseOptionsImpl()
{
    return true;
}

// �����������ص�ѡ��ʵ��
bool TestConnector::ParseReloadableOptionsImpl(bool reload)
{
    // ���Ժ��
    ParseConnectorOptions(m_TestBackend, "remote-ip", "remote-port", "remote-timeout", "remote-enable", reload);

    return true;
}

// �ر�ʵ��
void TestConnector::ShutdownImpl()
{
    m_TestBackend.DestroyConnector();
}

// ����ʵ��
void TestConnector::UpdateImpl()
{
    while (!m_InputQueue.empty())
    {
        if (!m_TestBackend.GetConnector() || m_TestBackend.GetConnector()->GetConnectorState() != CONNECTOR_STATE_CONNECTED)
        {
            break;
        }

        std::string& input = m_InputQueue.front();

        m_TestBackend.GetConnector()->Send(input.c_str(), (int)input.size());

        if (input == "88")
        {
            m_TestBackend.GetConnector()->GetConnection()->Disconnect();
        }

        m_InputQueue.pop();
    }
}

// ֹͣʵ��
void TestConnector::StopImpl()
{
    while (!m_InputQueue.empty())
    {
        m_InputQueue.pop();
    }
}

// ��������ʵ��
void TestConnector::ReloadImpl()
{

}

// ��������ʵ��
void TestConnector::HandleInputImpl(const std::string& input)
{
    if (!input.empty())
    {
        if (!m_TestBackend.GetConnector())
        {
            m_TestBackend.CreateConnector();
        }

        if (m_TestBackend.GetConnector())
        {
            if (m_TestBackend.GetConnector()->GetConnectorState() != CONNECTOR_STATE_CONNECTED)
            {
                m_TestBackend.GetConnector()->Connect();
            }

            if (input == "quit")
            {
                m_TestBackend.GetConnector()->Disconnect();
            }
            else
            {
                m_InputQueue.push(input);
            }
        }
    }
}
