#include "BackdoorClient/BackdoorClient.h"

// ���ſͻ���ʵ��
BackdoorClient* g_BackdoorClient = NULL;

// ���ſͻ���
BackdoorClient::BackdoorClient()
    : m_BackdoorBackend("127.0.0.1", BACKDOORCLIENT_DEFAULT_SERVICE_PORT, -1, true)
{
    g_BackdoorClient = this;

    m_AppName = "BackdoorClient";
}

BackdoorClient::~BackdoorClient()
{
    g_BackdoorClient = NULL;
}

// ��ʼ��ʵ��
bool BackdoorClient::InitImpl()
{
    if (m_BackdoorBackend.IsEnable())
    {
        if (!m_BackdoorBackend.CreateConnector())
        {
            return false;
        }

        m_BackdoorBackend.GetConnector()->SetConnectorListener(&m_BackdoorBackend);
    }

    return true;
}

// ע��ѡ��ʵ��
bool BackdoorClient::RegisterOptionsImpl()
{
    m_OptionsDesc.add_options()
    ("remote-ip", boost::program_options::value<std::string>(), "Specify remote ip.")
    ("remote-port", boost::program_options::value<int>(), "Specify remote port.")
    ("remote-timeout", boost::program_options::value<int>(), "Specify remote timeout.")
    ("remote-enable", boost::program_options::value<bool>(), "Specify remote is enabled.")
    ("remote-pwd", boost::program_options::value<bool>(), "Specify remote password.")
    ("remote-input", boost::program_options::value<std::string>(), "Specify remote input.")
    ;

    return true;
}

// ����ѡ��ʵ��
bool BackdoorClient::ParseOptionsImpl()
{
    if (m_VariablesMap.count("remote-input"))
    {
        m_BackdoorBackend.SetInput(m_VariablesMap["remote-input"].as<std::string>());
    }

    return true;
}

// �����������ص�ѡ��ʵ��
bool BackdoorClient::ParseReloadableOptionsImpl(bool reload)
{
    // ���ź��
    ParseConnectorOptions(m_BackdoorBackend, "remote-ip", "remote-port", "remote-timeout", "remote-enable", reload);

    // ���ŷ�������
    if (m_VariablesMap.count("remote-pwd"))
    {
        std::string newlyPwd = m_VariablesMap["remote-pwd"].as<std::string>();
        LogChangedOption("remote-pwd", m_BackdoorBackend.GetPwd(), newlyPwd, reload);
        m_BackdoorBackend.SetPwd(newlyPwd);
    }

    return true;
}

// �ر�ʵ��
void BackdoorClient::ShutdownImpl()
{
    m_BackdoorBackend.DestroyConnector();
}

// ����ʵ��
void BackdoorClient::UpdateImpl()
{

}

// ֹͣʵ��
void BackdoorClient::StopImpl()
{

}

// ��������ʵ��
void BackdoorClient::ReloadImpl()
{

}

// ��������ʵ��
void BackdoorClient::HandleInputImpl(const std::string& input)
{

}
