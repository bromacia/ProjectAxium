#ifndef __BATTLEGROUNDRL_H
#define __BATTLEGROUNDRL_H

class Battleground;

enum BattlegroundRLObjectTypes
{
    BG_RL_OBJECT_DOOR_1             = 0,
    BG_RL_OBJECT_DOOR_2             = 1,
    BG_RL_OBJECT_BUFF_1             = 2,
    BG_RL_OBJECT_BUFF_2             = 3,
    BG_RL_OBJECT_READY_MARKER_A     = 4,
    BG_RL_OBJECT_READY_MARKER_B     = 5,
    BG_RL_OBJECT_MAX                = 6
};

enum BattlegroundRLObjects
{
    BG_RL_OBJECT_TYPE_DOOR_1            = 185918,
    BG_RL_OBJECT_TYPE_DOOR_2            = 185917,
    BG_RL_OBJECT_TYPE_BUFF_1            = 184663,
    BG_RL_OBJECT_TYPE_BUFF_2            = 184664
};

class BattlegroundRLScore : public BattlegroundScore
{
    public:
        BattlegroundRLScore() {};
        virtual ~BattlegroundRLScore() {};
        //TODO fix me
};

class BattlegroundRL : public Battleground
{
    public:
        BattlegroundRL();
        ~BattlegroundRL();

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player* player);
        virtual void AddPlayerToScoreboard(Player* player, uint32 team);
        virtual void Reset();
        virtual void FillInitialWorldStates(WorldPacket &d);
        virtual void StartingEventCloseDoors();
        virtual void StartingEventOpenDoors();

        void RemovePlayer(Player* player, uint64 guid, uint32 team);
        void HandleAreaTrigger(Player* Source, uint32 Trigger);
        bool SetupBattleground();
        void HandleKillPlayer(Player* player, Player* killer);
        bool HandlePlayerUnderMap(Player* player);
};
#endif
