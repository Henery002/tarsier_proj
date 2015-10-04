#include "BackdoorClient/BackdoorClient.h"

// 后门客户端实例
BackdoorClient* g_BackdoorClient = NULL;

// 后门客户端
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

// 初始化实现
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

// 注册选项实现
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

// 解析选项实现
bool BackdoorClient::ParseOptionsImpl()
{
    if (m_VariablesMap.count("remote-input"))
    {
        m_BackdoorBackend.SetInput(m_VariablesMap["remote-input"].as<std::string>());
    }

    return true;
}

// 解析可以重载的选项实现
bool BackdoorClient::ParseReloadableOptionsImpl(bool reload)
{
    // 后门后端
    ParseConnectorOptions(m_BackdoorBackend, "remote-ip", "remote-port", "remote-timeout", "remote-enable", reload);

    // 后门服务密码
    if (m_VariablesMap.count("remote-pwd"))
    {
        std::string newlyPwd = m_VariablesMap["remote-pwd"].as<std::string>();
        LogChangedOption("remote-pwd", m_BackdoorBackend.GetPwd(), newlyPwd, reload);
        m_BackdoorBackend.SetPwd(newlyPwd);
    }

    return true;
}

// 关闭实现
void BackdoorClient::ShutdownImpl()
{
    m_BackdoorBackend.DestroyConnector();
}

// 更新实现
void BackdoorClient::UpdateImpl()
{

}

// 停止实现
void BackdoorClient::StopImpl()
{

}

// 重载配置实现
void BackdoorClient::ReloadImpl()
{

}

// 处理输入实现
void BackdoorClient::HandleInputImpl(const std::string& input)
{

}
