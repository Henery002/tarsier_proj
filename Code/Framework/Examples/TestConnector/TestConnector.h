#ifndef TESTCONNECTOR_H_
#define TESTCONNECTOR_H_

#include "TestConnector/Common.h"
#include "TestConnector/TestBackend.h"

// 连接器测试程序
class TestConnector : public SingletonDynamicDerived<TestConnector, App>
{
public:
    TestConnector();
    virtual ~TestConnector();

    // 初始化实现
    virtual bool InitImpl();

    // 注册选项实现
    virtual bool RegisterOptionsImpl();

    // 解析选项实现
    virtual bool ParseOptionsImpl();

    // 解析可以重载的选项实现
    virtual bool ParseReloadableOptionsImpl(bool reload);

    // 关闭实现
    virtual void ShutdownImpl();

    // 更新实现
    virtual void UpdateImpl();

    // 停止实现
    virtual void StopImpl();

    // 重载配置实现
    virtual void ReloadImpl();

    // 处理输入实现
    virtual void HandleInputImpl(const std::string& input);

protected:

    // 测试后端
    TestBackend m_TestBackend;

    // 输入队列
    std::queue<std::string> m_InputQueue;
};

// 连接器测试程序实例
extern TestConnector* g_TestConnector;

#endif // TESTCONNECTOR_H_
