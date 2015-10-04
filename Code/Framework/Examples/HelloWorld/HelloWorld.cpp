#include "HelloWorld/HelloWorld.h"

// 例子应用程序实例
HelloWorld* g_HelloWorld = NULL;

// 例子应用程序
HelloWorld::HelloWorld()
{
    g_HelloWorld = this;

    m_AppName = "HelloWorld";
}

HelloWorld::~HelloWorld()
{
    g_HelloWorld = NULL;
}

// 初始化实现
bool HelloWorld::InitImpl()
{
    /*
    LOG_TRACE("1");
    LOG_DEBUG("2");
    LOG_INFO("3");
    LOG_NOTICE("4");
    LOG_WARN("5");
    LOG_ERROR("6");
    LOG_CRIT("7");
    LOG_ALERT("8");
    LOG_FATAL("9");
    */
    return true;
}

// 注册选项实现
bool HelloWorld::RegisterOptionsImpl()
{
    m_OptionsDesc.add_options()
    ("hi", "Hi options.")
    ("hello", boost::program_options::value<std::string>(), "Hello options.")
    ;

    return true;
}

// 解析选项实现
bool HelloWorld::ParseOptionsImpl()
{
    if (m_VariablesMap.count("hi"))
    {
        std::cout << "hi" << std::endl;
    }

    return true;
}

// 解析可以重载的选项实现
bool HelloWorld::ParseReloadableOptionsImpl(bool reload)
{
    if (m_VariablesMap.count("hello"))
    {
        std::cout << m_VariablesMap["hello"].as<std::string>() << std::endl;
    }

    return true;
}

// 关闭实现
void HelloWorld::ShutdownImpl()
{

}

// 更新实现
void HelloWorld::UpdateImpl()
{

}

// 停止实现
void HelloWorld::StopImpl()
{

}

// 重载配置实现
void HelloWorld::ReloadImpl()
{

}

// 处理输入实现
void HelloWorld::HandleInputImpl(const std::string& input)
{
    std::cout << input << std::endl;
}
