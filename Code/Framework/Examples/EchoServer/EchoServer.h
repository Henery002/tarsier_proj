#ifndef ECHOSERVER_H_
#define ECHOSERVER_H_

#include "EchoServer/Common.h"
#include "EchoServer/EchoAcceptor.h"

// 回显服务器
class EchoServer : public SingletonDynamicDerived<EchoServer, App>
{
public:
    EchoServer();
    virtual ~EchoServer();

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

protected:

    // 回显接收器
    EchoAcceptor m_EchoAcceptor;
};

// 回显服务器实例
extern EchoServer* g_EchoServer;

#endif // ECHOSERVER_H_
