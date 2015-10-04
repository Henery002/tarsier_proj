#include "TestConnector/TestConnector.h"

int main(int argc, char* argv[])
{
    if (TestConnector::Inst()->Init(argc, argv))
    {
        TestConnector::Inst()->Run();
    }

    TestConnector::Inst()->Shutdown();

    return 0;
}
