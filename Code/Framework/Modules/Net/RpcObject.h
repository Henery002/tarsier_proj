#ifndef NET_RPCOBJECT_H_
#define NET_RPCOBJECT_H_

#include "Net/Common.h"

// Զ�̵��ö���
class NET_API RpcObject
{
public:
    RpcObject();
    virtual ~RpcObject();

    // ִ��
    virtual void Run();

    // ��һ��
    virtual void Next();

protected:

};

#endif // NET_RPCOBJECT_H_
