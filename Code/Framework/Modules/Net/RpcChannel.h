#ifndef NET_RPCCHANNEL_H_
#define NET_RPCCHANNEL_H_

#include "Net/Common.h"
#include "google/protobuf/service.h"

// 远程调用通道
class NET_API RpcChannel : public google::protobuf::RpcChannel
{
public:
    RpcChannel();
    virtual ~RpcChannel();

    // Call the given method of the remote service.  The signature of this
    // procedure looks the same as Service::CallMethod(), but the requirements
    // are less strict in one important way:  the request and response objects
    // need not be of any specific class as long as their descriptors are
    // method->input_type() and method->output_type().
    virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request,
                            google::protobuf::Message* response,
                            google::protobuf::Closure* done);

protected:

    // 连接
    Connection* m_Connection;
};

#endif // NET_RPCCHANNEL_H_
