#ifndef DELAYED_PACKET_MGR_H_
#define DELAYED_PACKET_MGR_H_

#include <ace/Singleton.h>

struct DelayedPacket
{
    public:
        DelayedPacket() : m_reciever(nullptr), m_packet(NULL), m_sendTimeMs(0) {}
        DelayedPacket(WorldSession* reciever, WorldPacket packet, uint32 sendTimeMs) :
            m_reciever(reciever), m_packet(packet), m_sendTimeMs(sendTimeMs) {}

        WorldSession* m_reciever;
        WorldPacket m_packet;
        uint32 m_sendTimeMs;
};

class DelayedPacketMgr
{
    friend class ACE_Singleton<DelayedPacketMgr, ACE_Null_Mutex>;

    private:
        DelayedPacketMgr() {}
        ~DelayedPacketMgr() {}

    public:
        void Update(uint32 diff);

        void Queue(WorldSession* reciever, WorldPacket packet, uint32 delayByMs);
        void Queue(DelayedPacket* delayedPacket);

    private:
        typedef std::deque<DelayedPacket*> DelayedPacketQueue;
        DelayedPacketQueue m_delayedPacketQueue;
};

#define sDelayedPacketMgr ACE_Singleton<DelayedPacketMgr, ACE_Null_Mutex>::instance()

#endif /*DELAYED_PACKET_MGR_H_*/
