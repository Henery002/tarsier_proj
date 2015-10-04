#include "Net/Net.h"

#if TARGET_PLATFORM == PLATFORM_WINDOWS
#include "Net/Windows/IocpAcceptor.h"
#elif TARGET_PLATFORM == PLATFORM_LINUX
#include "Net/Linux/EpollAcceptor.h"
#endif // TARGET_PLATFORM

// 应用程序实例
Net* g_Net = NULL;

// 网络
Net::Net()
    : m_Acceptors()
{
    LOG_TRACE("Net::Net()");

    BOOST_ASSERT(!g_Net);

    g_Net = this;
}

Net::~Net()
{
    LOG_TRACE("Net::~Net()");

    g_Net = NULL;
}

// 初始化
bool Net::Init()
{
    LOG_TRACE("Net::Init()");

#if TARGET_PLATFORM == PLATFORM_WINDOWS

    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2, 2), &wsd))
    {
        LOG_ERROR("WSAStartup() failed, %s.", GetStrError()->c_str());
        return false;
    }

#endif // TARGET_PLATFORM

    return true;
}

// 关闭
void Net::Shutdown()
{
    LOG_TRACE("Net::Shutdown()");

    if (!m_Acceptors.empty())
    {
        for (std::size_t i = 0; i < m_Acceptors.size(); ++i)
        {
            m_Acceptors[i]->Stop();
            SAFE_DELETE(Acceptor, m_Acceptors[i]);
        }

        m_Acceptors.clear();
    }

    if (!m_Connectors.empty())
    {
        for (std::size_t i = 0; i < m_Connectors.size(); ++i)
        {
            m_Connectors[i]->Disconnect();
            SAFE_DELETE(Connector, m_Connectors[i]);
        }

        m_Connectors.clear();
    }

#if TARGET_PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif // TARGET_PLATFORM

    Net::DestroyInst();
}

// 更新
void Net::Update()
{
    for (std::size_t i = 0; i < m_Acceptors.size(); ++i)
    {
        m_Acceptors[i]->Update();
    }

    for (std::size_t i = 0; i < m_Connectors.size(); ++i)
    {
        m_Connectors[i]->Update();
    }
}

// 接收器
Acceptor* Net::CreateAcceptor()
{
    Acceptor* acceptor = NULL;

#if TARGET_PLATFORM == PLATFORM_WINDOWS
    acceptor = NEW(IocpAcceptor, MEM_TAG_NET);
#elif TARGET_PLATFORM == PLATFORM_LINUX
    acceptor = NEW(EpollAcceptor, MEM_TAG_NET);
#endif // TARGET_PLATFORM

    BOOST_ASSERT(acceptor);

    m_Acceptors.push_back(acceptor);

    return acceptor;
}

void Net::DestroyAcceptor(Acceptor* acceptor, bool stop)
{
    BOOST_ASSERT(acceptor);

    if (stop)
    {
        acceptor->Stop();
    }

    std::vector<Acceptor*>::iterator it = std::find(m_Acceptors.begin(), m_Acceptors.end(), acceptor);
    if (it != m_Acceptors.end())
    {
        m_Acceptors.erase(it);
    }

    SAFE_DELETE(Acceptor, acceptor);
}

// 连接器
Connector* Net::CreateConnector()
{
    Connector* connector = NEW(Connector, MEM_TAG_NET);

    m_Connectors.push_back(connector);

    return connector;
}

void Net::DestroyConnector(Connector* connector, bool distconn)
{
    BOOST_ASSERT(connector);

    if (distconn)
    {
        connector->Disconnect();
    }

    std::vector<Connector*>::iterator it = std::find(m_Connectors.begin(), m_Connectors.end(), connector);
    if (it != m_Connectors.end())
    {
        m_Connectors.erase(it);
    }

    SAFE_DELETE(Connector, connector);
}
