#include "Net/Acceptor.h"

// 接收器
Acceptor::Acceptor()
    : m_MaxAcceptNum(DEFAULT_MAX_ACCPET_NUM)
    , m_AcceptorListener(NULL)
    , m_PacketSizeLimit(UINT_MAX)
    , m_AcceptorState(ACCEPTOR_STATE_STOPED)
{
    LOG_TRACE("Acceptor::Acceptor()");
}

Acceptor::~Acceptor()
{
    LOG_TRACE("Acceptor::~Acceptor()");
}

// 启动
bool Acceptor::Start(const std::string& ip, int port)
{
    LOG_TRACE("Acceptor::Start(%s, %d)", ip.c_str(), port);

    m_AcceptorState = ACCEPTOR_STATE_STARTED;

    return true;
}

// 关闭
void Acceptor::Stop()
{
    LOG_TRACE("Acceptor::Stop()");

    m_AcceptorState = ACCEPTOR_STATE_STOPED;
}

// 更新
void Acceptor::Update()
{

}

// 最大接收连接数
int Acceptor::GetMaxAcceptNum()
{
    return m_MaxAcceptNum;
}

void Acceptor::SetMaxAcceptNum(int num)
{
    m_MaxAcceptNum = num;
}

// 接收监听器
AcceptorListener* Acceptor::GetAcceptorListener()
{
    return m_AcceptorListener;
}

void Acceptor::SetAcceptorListener(AcceptorListener* acceptorListener)
{
    m_AcceptorListener = acceptorListener;
}

// 数据包大小限制
uint32_t Acceptor::GetPacketSizeLimit()
{
    return m_PacketSizeLimit;
}

void Acceptor::SetPacketSizeLimit(uint32_t size)
{
    m_PacketSizeLimit = size;
}

// 获得接收器状态
ACCEPTOR_STATE_TYPE Acceptor::GetAcceptorState()
{
    return m_AcceptorState;
}
