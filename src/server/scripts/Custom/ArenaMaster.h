#include "ScriptMgr.h"
#include "ArenaTeam.h"
#include "ArenaTeamMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "Chat.h"
#include "Creature.h"
#include "Player.h"
#include "World.h"

enum ArenaOptions
{
    ARENA_SPECTATE_MENU_CREATE_TEAM = 2,
    ARENA_SPECTATE_MENU_QUEUE,
    ARENA_SPECTATE_MENU_2V2_MATCHES,
    ARENA_SPECTATE_MENU_3V3_MATCHES,
    ARENA_SPECTATE_MENU_5V5_MATCHES,
    ARENA_SPECTATE_MENU_SPECTATE_PLAYER,
    ARENA_SPECTATE_MENU_PAGE_NEXT,
    ARENA_SPECTATE_MENU_PAGE_PREVIOUS,
};

#define MAX_RESULTS_PER_PAGE 50

class ArenaMaster : public CreatureScript
{
    public:
        ArenaMaster();
        bool OnGossipHello(Player* player, Creature* creature);
        bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action);
        bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code);
        void HandleCreateTeam(Player* player, Creature* creature);
        void HandleQueue(Player* player);
        void HandleShowMatches(Player* player, Creature* creature, uint32 sender, uint32 action);
        void HandleSpectateMatch(Player* player, Creature* creature, uint32 action);
        void HandleSpectatePlayer(Player* player, const char* cPlayerName);
        void AddPlayerToArena(Player* player, uint32 action);

    private:
        bool CheckBattleground(Battleground* bg);
        void CreateArenasMap();
        uint8 GetArenaTypeByAction(uint32 action)
        {
            switch (action)
            {
                case ARENA_SPECTATE_MENU_2V2_MATCHES: return ARENA_TYPE_2v2;
                case ARENA_SPECTATE_MENU_3V3_MATCHES: return ARENA_TYPE_3v3;
                case ARENA_SPECTATE_MENU_5V5_MATCHES: return ARENA_TYPE_5v5;
                default: return 0;
            }
        }

        BattlegroundsMap arenasMap;
};
