#include "Battleground.h"
#include "BattlegroundDS.h"
#include "Language.h"
#include "Player.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"

BattlegroundDS::BattlegroundDS()
{
    m_BgObjects.resize(BG_DS_OBJECT_MAX);
    m_BgCreatures.resize(BG_DS_NPC_MAX);

    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    //we must set messageIds
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;
}

BattlegroundDS::~BattlegroundDS()
{

}

void BattlegroundDS::PostUpdateImpl(uint32 diff)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (pipeKnockBackCount < BG_DS_PIPE_KNOCKBACK_TOTAL_COUNT)
    {
        if (pipeKnockBackTimer < diff)
        {
            for (uint32 i = BG_DS_NPC_PIPE_KNOCKBACK_1; i <= BG_DS_NPC_PIPE_KNOCKBACK_2; ++i)
                if (Creature* waterSpout = GetBgMap()->GetCreature(m_BgCreatures[i]))
                    waterSpout->CastSpell(waterSpout, BG_DS_SPELL_FLUSH, true);

            ++pipeKnockBackCount;
            pipeKnockBackTimer = BG_DS_PIPE_KNOCKBACK_DELAY;
        }
        else
            pipeKnockBackTimer -= diff;
    }
}

void BattlegroundDS::StartingEventCloseDoors()
{
    for (uint8 i = BG_DS_OBJECT_DOOR_1; i <= BG_DS_OBJECT_DOOR_2; ++i)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
}

void BattlegroundDS::StartingEventOpenDoors()
{
    for (uint8 i = BG_DS_OBJECT_DOOR_1; i <= BG_DS_OBJECT_DOOR_2; ++i)
        DoorOpen(i, true);

    for (uint8 i = BG_DS_OBJECT_BUFF_1; i <= BG_DS_OBJECT_BUFF_2; ++i)
        SpawnBGObject(i, 60);

    pipeKnockBackTimer = BG_DS_PIPE_KNOCKBACK_FIRST_DELAY;
    pipeKnockBackCount = 0;

    // Remove effects of Demonic Circle Summon
    for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        if (Player* player = ObjectAccessor::FindPlayer(itr->first))
            if (player->HasAura(48018))
                player->RemoveAurasDueToSpell(48018);
}

void BattlegroundDS::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    UpdateArenaWorldState();
}

void BattlegroundDS::AddPlayerToScoreboard(Player* player, uint32 team)
{
    BattlegroundDSScore* sc = new BattlegroundDSScore;
    sc->PlayerTeam = team;
    m_PlayerScores[player->GetGUID()] = sc;
}

void BattlegroundDS::RemovePlayer(Player* /*player*/, uint64 /*guid*/, uint32 /*team*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattlegroundDS::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        sLog->outError("BattlegroundDS: Killer player not found");
        return;
    }

    Battleground::HandleKillPlayer(player, killer);

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattlegroundDS::HandleAreaTrigger(Player* Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch (Trigger)
    {
        case 5347:
        case 5348:
            // Remove effects of Demonic Circle Summon
            if (Source->HasAura(48018))
                Source->RemoveAurasDueToSpell(48018);

            // Someone has get back into the pipes and the knockback has already been performed,
            // so we reset the knockback count for kicking the player again into the arena.
            if (pipeKnockBackCount >= BG_DS_PIPE_KNOCKBACK_TOTAL_COUNT)
                pipeKnockBackCount = 0;
            break;
        default:
            sLog->outError("WARNING: Unhandled AreaTrigger in BattlegroundDS: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}

bool BattlegroundDS::HandlePlayerUnderMap(Player* player)
{
    player->TeleportTo(GetMapId(), 1291.724f, 791.0f, 7.12f, 1.53f, false);
    return true;
}

void BattlegroundDS::FillInitialWorldStates(WorldPacket &data)
{
    data << uint32(3610) << uint32(1);                                              // 9 show
    UpdateArenaWorldState();
}

void BattlegroundDS::Reset()
{
    //call parent's class reset
    Battleground::Reset();
}

bool BattlegroundDS::SetupBattleground()
{
    // Gates
    if (!AddObject(BG_DS_OBJECT_DOOR_1, BG_DS_OBJECT_TYPE_DOOR_1, 1350.95f, 817.2f, 20.8096f, 3.15f, 0, 0, 0.99627f, 0.0862864f, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_DS_OBJECT_DOOR_2, BG_DS_OBJECT_TYPE_DOOR_2, 1232.65f, 764.913f, 20.0729f, 6.3f, 0, 0, 0.0310211f, -0.999519f, RESPAWN_IMMEDIATELY) ||
    // Buffs
        !AddObject(BG_DS_OBJECT_BUFF_1, BG_DS_OBJECT_TYPE_BUFF_1, 1330.056763f, 765.579285f, 3.159700f, 0, 0, 0, 0.730314f, -0.683111f, 120) ||
        !AddObject(BG_DS_OBJECT_BUFF_2, BG_DS_OBJECT_TYPE_BUFF_2, 1254.318237f, 816.529175f, 3.158578f, 0, 0, 0, 0.700409f, 0.713742f, 120) ||
    // Ready Markers
        !AddObject(BG_DS_OBJECT_READY_MARKER_A, BG_OBJECTID_READY_MARKER, 1230.58f, 760.11f, 17.06f, 3.11f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_DS_OBJECT_READY_MARKER_B, BG_OBJECTID_READY_MARKER, 1352.74f, 822.0f, 17.17f, 6.26f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
    // Pipe knockbacks
        !AddCreature(BG_DS_NPC_TYPE_WATER_SPOUT, BG_DS_NPC_PIPE_KNOCKBACK_1, 0, 1369.977f, 817.2882f, 16.08718f, 3.106686f, RESPAWN_IMMEDIATELY) ||
        !AddCreature(BG_DS_NPC_TYPE_WATER_SPOUT, BG_DS_NPC_PIPE_KNOCKBACK_2, 0, 1212.833f, 765.3871f, 16.09484f, 0.0f, RESPAWN_IMMEDIATELY))
    {
        sLog->outErrorDb("BatteGroundDS: Failed to spawn some object!");
        return false;
    }

    return true;
}
