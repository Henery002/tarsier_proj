#ifndef BACKDOORSERVICEIMPL_H_
#define BACKDOORSERVICEIMPL_H_

#include "App/Common.h"

// 后门服务实现
class BackdoorServiceImpl : public BackdoorService
{
public:
    BackdoorServiceImpl();
    virtual ~BackdoorServiceImpl();

    // 授权
    virtual void Auth(::google::protobuf::RpcController* controller,
                      const ::BackdoorAuthReq* request,
                      ::BackdoorAuthRsp* response,
                      ::google::protobuf::Closure* done);

    // 输入
    virtual void Input(::google::protobuf::RpcController* controller,
                       const ::BackdoorInputReq* request,
                       ::BackdoorInputRsp* response,
                       ::google::protobuf::Closure* done);
};

#endif // BACKDOORSERVICEIMPL_H_
