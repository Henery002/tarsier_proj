#include "App/BackdoorServiceImpl.h"

// 后门服务实现
BackdoorServiceImpl::BackdoorServiceImpl()
{

}

BackdoorServiceImpl::~BackdoorServiceImpl()
{

}

// 授权
void BackdoorServiceImpl::Auth(::google::protobuf::RpcController* controller,
                               const ::BackdoorAuthReq* request,
                               ::BackdoorAuthRsp* response,
                               ::google::protobuf::Closure* done)
{

}

// 输入
void BackdoorServiceImpl::Input(::google::protobuf::RpcController* controller,
                                const ::BackdoorInputReq* request,
                                ::BackdoorInputRsp* response,
                                ::google::protobuf::Closure* done)
{

}
