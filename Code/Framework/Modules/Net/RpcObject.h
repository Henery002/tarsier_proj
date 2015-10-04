#ifndef NET_RPCOBJECT_H_
#define NET_RPCOBJECT_H_

#include "Net/Common.h"

// 远程调用对象
class NET_API RpcObject
{
public:
    RpcObject();
    virtual ~RpcObject();

    // 执行
    virtual void Run();

    // 下一步
    virtual void Next();

protected:

};

#endif // NET_RPCOBJECT_H_
