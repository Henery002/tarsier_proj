#ifndef NET_ACCEPTORPROXY_H_
#define NET_ACCEPTORPROXY_H_

#include "Net/Common.h"

// 接收器代理
class NET_API AcceptorProxy
{
public:
    AcceptorProxy(const std::string& ip = "", int port = 0, bool enable = false);
    virtual ~AcceptorProxy();

    // 创建接收器
    bool CreateAcceptor();

    // 销毁接收器
    void DestroyAcceptor(bool stop = true);

    // 重启接收器
    bool RestartAcceptor();

    // 是否启用接收器
    bool IsEnable();
    void SetEnable(bool enable);

    // 接收器
    Acceptor* GetAcceptor();

    // 接收器 IP
    const std::string& GetIp();
    void SetIp(const std::string& ip);

    // 接收器端口
    int GetPort();
    void SetPort(int port);

    // 重置
    void Reset(const std::string& ip, int port, bool enable, bool reload);

protected:

    // 是否启用接收器
    bool m_Enable;

    // 接收器
    Acceptor* m_Acceptor;

    // 接收器 IP
    std::string m_Ip;

    // 接收器端口
    int m_Port;
};

#endif // NET_ACCEPTORPROXY_H_
