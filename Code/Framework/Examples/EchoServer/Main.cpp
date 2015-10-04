#include "EchoServer/EchoServer.h"

int main(int argc, char* argv[])
{
    if (EchoServer::Inst()->Init(argc, argv))
    {
        EchoServer::Inst()->Run();
    }

    EchoServer::Inst()->Shutdown();

    return 0;
}
