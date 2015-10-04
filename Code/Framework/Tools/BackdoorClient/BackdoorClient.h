#ifndef BACKDOORCLIENT_H_
#define BACKDOORCLIENT_H_

#include "BackdoorClient/Common.h"
#include "BackdoorClient/BackdoorBackend.h"

// 后门客户端
class BackdoorClient : public SingletonDynamicDerived<BackdoorClient, App>
{
public:
    BackdoorClient();
    virtual ~BackdoorClient();

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
    BackdoorBackend m_BackdoorBackend;
};

// 后门客户端实例
extern BackdoorClient* g_BackdoorClient;

#endif // BACKDOORCLIENT_H_
