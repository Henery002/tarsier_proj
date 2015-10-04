#ifndef HELLOWORLD_H_
#define HELLOWORLD_H_

#include "HelloWorld/Common.h"

// 例子应用程序
class HelloWorld : public SingletonDynamicDerived<HelloWorld, App>
{
public:
    HelloWorld();
    virtual ~HelloWorld();

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
};

// 例子应用程序实例
extern HelloWorld* g_HelloWorld;

#endif // HELLOWORLD_H_
