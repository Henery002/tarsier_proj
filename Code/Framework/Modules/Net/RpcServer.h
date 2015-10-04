#ifndef NET_RPCSERVER_H_
#define NET_RPCSERVER_H_

#include "Net/Common.h"
#include "google/protobuf/service.h"

// 服务元数据
struct NET_API ServiceMeta
{
    google::protobuf::Service* service;
    const google::protobuf::MethodDescriptor* method;
    const google::protobuf::Message* requestPrototype;
    const google::protobuf::Message* responsePrototype;
};

// 远程调用服务器
class NET_API RpcServer
{
public:
    RpcServer();
    virtual ~RpcServer();

    // 处理数据包
    void HandlePacket(Connection* conn, Packet* packet);

    // 注册服务
    bool RegisterService(google::protobuf::Service* service);

    // 获得服务元数据
    ServiceMeta* GetServiceMeta(int cmd);

protected:

    // 服务元数据
    std::map<int, ServiceMeta> m_ServiceMetas;
};

#endif // NET_RPCSERVER_H_
