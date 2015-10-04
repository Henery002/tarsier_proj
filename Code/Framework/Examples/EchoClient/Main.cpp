#include "EchoClient/EchoClient.h"

int main(int argc, char* argv[])
{
    if (EchoClient::Inst()->Init(argc, argv))
    {
        EchoClient::Inst()->Run();
    }

    EchoClient::Inst()->Shutdown();

    return 0;
}
