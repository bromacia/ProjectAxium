#ifndef DUEL_MGR_H_
#define DUEL_MGR_H_

#include <ace/Singleton.h>

class Player;

class DuelMgr
{
    friend class ACE_Singleton<DuelMgr, ACE_Null_Mutex>;

    private:
        DuelMgr();
        ~DuelMgr();

    public:
        void HandleDuelStart(Player* initiator, Player* opponent);
        void HandleDuelEnd(Player* winner, Player* loser, DuelCompleteType type);
    private:
        void ResetPlayer(Player* player);
        void ResetPet(Pet* pet, Player* owner);
};

#define sDuelMgr ACE_Singleton<DuelMgr, ACE_Null_Mutex>::instance()

#endif /*DUEL_MGR_H_*/
