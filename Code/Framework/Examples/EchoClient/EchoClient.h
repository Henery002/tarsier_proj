#ifndef ECHOCLIENT_H_
#define ECHOCLIENT_H_

#include "EchoClient/Common.h"

// 例子应用程序
class EchoClient : public SingletonDynamicDerived<EchoClient, App>
{
public:
    EchoClient();
    virtual ~EchoClient();

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

    // 套接字
    SocketProxy& GetSocket();

protected:

    // 套接字
    SocketProxy m_Socket;

    // 接收线程
    boost::thread m_RecvThread;
};

// 例子应用程序实例
extern EchoClient* g_EchoClient;

#endif // ECHOCLIENT_H_
