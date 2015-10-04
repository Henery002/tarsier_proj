#include "GateServer/GateServer.h"

int main(int argc, char* argv[])
{
    if (GateServer::Inst()->Init(argc, argv))
    {
        GateServer::Inst()->Run();
    }

    GateServer::Inst()->Shutdown();

    return 0;
}
