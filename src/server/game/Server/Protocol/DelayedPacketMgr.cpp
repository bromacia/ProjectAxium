#include "DelayedPacketMgr.h"

void DelayedPacketMgr::Update(uint32 diff)
{
    if (m_delayedPacketQueue.empty())
        return;

    uint32 msTime = getMSTime();
    for (DelayedPacketQueue::iterator itr = m_delayedPacketQueue.begin(); itr != m_delayedPacketQueue.end();)
    {
        DelayedPacket* delayedPacket = *itr;
        if (!delayedPacket->m_reciever)
            return;

        if ((delayedPacket->m_sendTimeMs + diff) <= msTime)
        {
            delayedPacket->m_reciever->SendPacket(&delayedPacket->m_packet);
            itr = m_delayedPacketQueue.erase(itr);
        }
        else
            ++itr;
    }
}

void DelayedPacketMgr::Queue(WorldSession* reciever, WorldPacket packet, uint32 delayByMs)
{
    m_delayedPacketQueue.push_back(new DelayedPacket(reciever, packet, getMSTime() + delayByMs));
}

void DelayedPacketMgr::Queue(DelayedPacket* delayedPacket)
{
    m_delayedPacketQueue.push_back(delayedPacket);
}
