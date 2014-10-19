#ifndef __BATTLEGROUNDDS_H
#define __BATTLEGROUNDDS_H

class Battleground;

enum BattlegroundDSObjectTypes
{
    BG_DS_OBJECT_DOOR_1             = 0,
    BG_DS_OBJECT_DOOR_2             = 1,
    BG_DS_OBJECT_BUFF_1             = 2,
    BG_DS_OBJECT_BUFF_2             = 3,
    BG_DS_OBJECT_READY_MARKER_A     = 4,
    BG_DS_OBJECT_READY_MARKER_B     = 5,
    BG_DS_OBJECT_MAX                = 7
};

enum BattlegroundDSObjects
{
    BG_DS_OBJECT_TYPE_DOOR_1    = 192642,
    BG_DS_OBJECT_TYPE_DOOR_2    = 192643,
    BG_DS_OBJECT_TYPE_BUFF_1    = 184663,
    BG_DS_OBJECT_TYPE_BUFF_2    = 184664
};

enum BattlegroundDSCreatureTypes
{
    BG_DS_NPC_PIPE_KNOCKBACK_1 = 0,
    BG_DS_NPC_PIPE_KNOCKBACK_2 = 1,
    BG_DS_NPC_MAX = 2
};

enum BattlegroundDSCreatures
{
    BG_DS_NPC_TYPE_WATER_SPOUT = 28567
};

enum BattlegroundDSSpells
{
    BG_DS_SPELL_FLUSH = 57405, // Visual and target selector for the starting knockback from the pipe
    BG_DS_SPELL_FLUSH_KNOCKBACK = 61698, // Knockback effect for previous spell (triggered, not need to be casted)
};

enum BattlegroundDSData
{ // These values are NOT blizzlike... need the correct data!
    BG_DS_WATERFALL_TIMER_MIN                    = 30000,
    BG_DS_WATERFALL_TIMER_MAX                    = 60000,
    BG_DS_WATERFALL_DURATION                     = 10000,
    BG_DS_WATERFALL_KNOCKBACK_TIMER              = 1500,

    BG_DS_PIPE_KNOCKBACK_FIRST_DELAY = 5000,
    BG_DS_PIPE_KNOCKBACK_DELAY = 3000,
    BG_DS_PIPE_KNOCKBACK_TOTAL_COUNT = 2
};

class BattlegroundDSScore : public BattlegroundScore
{
    public:
        BattlegroundDSScore() {};
        virtual ~BattlegroundDSScore() {};
        //TODO fix me
};

class BattlegroundDS : public Battleground
{
    public:
        BattlegroundDS();
        ~BattlegroundDS();

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player* player);
        virtual void AddPlayerToScoreboard(Player* player, uint32 team);
        virtual void StartingEventCloseDoors();
        virtual void StartingEventOpenDoors();

        void RemovePlayer(Player* player, uint64 guid, uint32 team);
        void HandleAreaTrigger(Player* Source, uint32 Trigger);
        bool SetupBattleground();
        virtual void Reset();
        virtual void FillInitialWorldStates(WorldPacket &d);
        void HandleKillPlayer(Player* player, Player* killer);
        bool HandlePlayerUnderMap(Player* player);
    private:
        virtual void PostUpdateImpl(uint32 diff);
        uint32 pipeKnockBackTimer;
        uint8 pipeKnockBackCount;
};
#endif
