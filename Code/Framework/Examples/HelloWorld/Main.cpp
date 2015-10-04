#include "HelloWorld/HelloWorld.h"

int main(int argc, char* argv[])
{
    if (HelloWorld::Inst()->Init(argc, argv))
    {
        HelloWorld::Inst()->Run();
    }

    HelloWorld::Inst()->Shutdown();

    return 0;
}
