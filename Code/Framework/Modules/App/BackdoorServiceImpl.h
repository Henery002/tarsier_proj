#ifndef BACKDOORSERVICEIMPL_H_
#define BACKDOORSERVICEIMPL_H_

#include "App/Common.h"

// ���ŷ���ʵ��
class BackdoorServiceImpl : public BackdoorService
{
public:
    BackdoorServiceImpl();
    virtual ~BackdoorServiceImpl();

    // ��Ȩ
    virtual void Auth(::google::protobuf::RpcController* controller,
                      const ::BackdoorAuthReq* request,
                      ::BackdoorAuthRsp* response,
                      ::google::protobuf::Closure* done);

    // ����
    virtual void Input(::google::protobuf::RpcController* controller,
                       const ::BackdoorInputReq* request,
                       ::BackdoorInputRsp* response,
                       ::google::protobuf::Closure* done);
};

#endif // BACKDOORSERVICEIMPL_H_
