#include "Net/RpcServer.h"

// 远程调用服务器
RpcServer::RpcServer()
    : m_ServiceMetas()
{

}

RpcServer::~RpcServer()
{

}

// 处理数据包
void RpcServer::HandlePacket(Connection* conn, Packet* packet)
{

}

// 注册服务
bool RpcServer::RegisterService(google::protobuf::Service* service)
{
    BOOST_ASSERT(service);

    const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();

    for (int i = 0; i < desc->method_count(); ++i)
    {
        const google::protobuf::MethodDescriptor* method = desc->method(i);
        int cmd = method->options().GetExtension(method_id);

        if (!cmd)
        {
            LOG_ERROR("Undefined method_id for (%s)", method->full_name().c_str());
            return false;
        }

        ServiceMeta meta = {
            service,
            method,
            &(service->GetRequestPrototype(method)),
            &(service->GetResponsePrototype(method)),
        };

        if (!m_ServiceMetas.insert(std::make_pair(cmd, meta)).second)
        {
            LOG_ERROR("Duplicated method_id found for (%s)", method->full_name().c_str());
            return false;
        }
    }

    return true;
}

// 获得服务元数据
ServiceMeta* RpcServer::GetServiceMeta(int cmd)
{
    std::map<int, ServiceMeta>::iterator it = m_ServiceMetas.find(cmd);
    if (it == m_ServiceMetas.end())
    {
        return NULL;
    }
    return &it->second;
}
