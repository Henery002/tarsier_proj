#ifndef NET_RPCSERVER_H_
#define NET_RPCSERVER_H_

#include "Net/Common.h"
#include "google/protobuf/service.h"

// ����Ԫ����
struct NET_API ServiceMeta
{
    google::protobuf::Service* service;
    const google::protobuf::MethodDescriptor* method;
    const google::protobuf::Message* requestPrototype;
    const google::protobuf::Message* responsePrototype;
};

// Զ�̵��÷�����
class NET_API RpcServer
{
public:
    RpcServer();
    virtual ~RpcServer();

    // �������ݰ�
    void HandlePacket(Connection* conn, Packet* packet);

    // ע�����
    bool RegisterService(google::protobuf::Service* service);

    // ��÷���Ԫ����
    ServiceMeta* GetServiceMeta(int cmd);

protected:

    // ����Ԫ����
    std::map<int, ServiceMeta> m_ServiceMetas;
};

#endif // NET_RPCSERVER_H_
