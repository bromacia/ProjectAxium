#include "Battleground.h"
#include "BattlegroundRV.h"
#include "ObjectAccessor.h"
#include "Language.h"
#include "Player.h"
#include "WorldPacket.h"
#include "GameObject.h"

BattlegroundRV::BattlegroundRV()
{
    m_BgObjects.resize(BG_RV_OBJECT_MAX);

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

BattlegroundRV::~BattlegroundRV()
{

}

void BattlegroundRV::PostUpdateImpl(uint32 diff)
{
    if (getTimer() < diff)
    {
        switch (getState())
        {
            case BG_RV_STATE_OPEN_FENCES:
                setTimer(BG_RV_PILAR_TO_FIRE_TIMER);
                setState(BG_RV_STATE_CLOSE_FIRE);
                break;
            case BG_RV_STATE_CLOSE_FIRE:
                for (uint8 i = BG_RV_OBJECT_FIRE_1; i <= BG_RV_OBJECT_FIREDOOR_2; ++i)
                    DoorClose(i);
                setTimer(BG_RV_FIRE_TO_PILAR_TIMER);
                setState(BG_RV_STATE_OPEN_PILARS);
                break;
            case BG_RV_STATE_OPEN_PILARS:
                for (uint8 i = BG_RV_OBJECT_PILAR_1; i <= BG_RV_OBJECT_PULLEY_2; ++i)
                    DoorOpen(i);
                setTimer(BG_RV_PILAR_TO_FIRE_TIMER);
                setState(BG_RV_STATE_OPEN_FIRE);
                break;
            case BG_RV_STATE_OPEN_FIRE:
                // FIXME: after 3.2.0 it's only decorative and should be opened only one time at battle start
                for (uint8 i = BG_RV_OBJECT_FIRE_1; i <= BG_RV_OBJECT_FIREDOOR_2; ++i)
                    DoorOpen(i);
                setTimer(BG_RV_FIRE_TO_PILAR_TIMER);
                setState(BG_RV_STATE_CLOSE_PILARS);
                break;
            case BG_RV_STATE_CLOSE_PILARS:
                for (uint8 i = BG_RV_OBJECT_PILAR_1; i <= BG_RV_OBJECT_PULLEY_2; ++i)
                    DoorOpen(i);
                setTimer(BG_RV_PILAR_TO_FIRE_TIMER);
                setState(BG_RV_STATE_CLOSE_FIRE);
                break;
        }
    }
    else
        setTimer(getTimer() - diff);
}

void BattlegroundRV::StartingEventCloseDoors()
{
}

void BattlegroundRV::StartingEventOpenDoors()
{
    // Buff respawn
    SpawnBGObject(BG_RV_OBJECT_BUFF_1, 90);
    SpawnBGObject(BG_RV_OBJECT_BUFF_2, 90);
    // Elevators
    DoorOpen(BG_RV_OBJECT_ELEVATOR_1);
    DoorOpen(BG_RV_OBJECT_ELEVATOR_2);

    setState(BG_RV_STATE_OPEN_FENCES);
    setTimer(BG_RV_FIRST_TIMER);
}

void BattlegroundRV::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    //create score and add it to map, default values are set in constructor
    BattlegroundRVScore* sc = new BattlegroundRVScore;

    m_PlayerScores[player->GetGUID()] = sc;

    UpdateWorldState(BG_RV_WORLD_STATE_A, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(BG_RV_WORLD_STATE_H, GetAlivePlayersCountByTeam(HORDE));
}

void BattlegroundRV::AddPlayerToScoreboard(Player* player, uint32 team)
{
    BattlegroundRVScore* sc = new BattlegroundRVScore;
    sc->PlayerTeam = team;
    m_PlayerScores[player->GetGUID()] = sc;
}

void BattlegroundRV::RemovePlayer(Player* /*player*/, uint64 /*guid*/, uint32 /*team*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateWorldState(BG_RV_WORLD_STATE_A, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(BG_RV_WORLD_STATE_H, GetAlivePlayersCountByTeam(HORDE));

    CheckArenaWinConditions();
}

void BattlegroundRV::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        sLog->outError("BattlegroundRV: Killer player not found");
        return;
    }

    Battleground::HandleKillPlayer(player, killer);

    UpdateWorldState(BG_RV_WORLD_STATE_A, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(BG_RV_WORLD_STATE_H, GetAlivePlayersCountByTeam(HORDE));

    CheckArenaWinConditions();
}

bool BattlegroundRV::HandlePlayerUnderMap(Player* player)
{
    player->TeleportTo(GetMapId(), 763.5f, -284, 28.276f, 2.422f, false);
    return true;
}

void BattlegroundRV::HandleAreaTrigger(Player* Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch (Trigger)
    {
        case 5224:
        case 5226:
        // fire was removed in 3.2.0
        case 5473:
        case 5474:
            break;
        default:
            sLog->outError("WARNING: Unhandled AreaTrigger in BattlegroundRV: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}

void BattlegroundRV::FillInitialWorldStates(WorldPacket &data)
{
    data << uint32(BG_RV_WORLD_STATE_A) << uint32(GetAlivePlayersCountByTeam(ALLIANCE));
    data << uint32(BG_RV_WORLD_STATE_H) << uint32(GetAlivePlayersCountByTeam(HORDE));
    data << uint32(BG_RV_WORLD_STATE) << uint32(1);
}

void BattlegroundRV::Reset()
{
    //call parent's class reset
    Battleground::Reset();
}

bool BattlegroundRV::SetupBattleground()
{
    // Elevators
    if (!AddObject(BG_RV_OBJECT_ELEVATOR_1, BG_RV_OBJECT_TYPE_ELEVATOR_1, 763.536377f, -294.535767f, 0.505383f, 3.141593f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_ELEVATOR_2, BG_RV_OBJECT_TYPE_ELEVATOR_2, 763.506348f, -273.873352f, 0.505383f, 0.000000f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
    // Buffs
        !AddObject(BG_RV_OBJECT_BUFF_1, BG_RV_OBJECT_TYPE_BUFF_1, 735.551819f, -284.794678f, 28.276682f, 0.034906f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_BUFF_2, BG_RV_OBJECT_TYPE_BUFF_2, 791.224487f, -284.794464f, 28.276682f, 2.600535f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
    // Fire
        !AddObject(BG_RV_OBJECT_FIRE_1, BG_RV_OBJECT_TYPE_FIRE_1, 743.543457f, -283.799469f, 28.286655f, 3.141593f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_FIRE_2, BG_RV_OBJECT_TYPE_FIRE_2, 782.971802f, -283.799469f, 28.286655f, 3.141593f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_FIREDOOR_1, BG_RV_OBJECT_TYPE_FIREDOOR_1, 743.711060f, -284.099609f, 27.542587f, 3.141593f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_FIREDOOR_2, BG_RV_OBJECT_TYPE_FIREDOOR_2, 783.221252f, -284.133362f, 27.535686f, 0.000000f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
    // Gear
        !AddObject(BG_RV_OBJECT_GEAR_1, BG_RV_OBJECT_TYPE_GEAR_1, 763.664551f, -261.872986f, 26.686588f, 0.000000f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_GEAR_2, BG_RV_OBJECT_TYPE_GEAR_2, 763.578979f, -306.146149f, 26.665222f, 3.141593f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
    // Pulley
        !AddObject(BG_RV_OBJECT_PULLEY_1, BG_RV_OBJECT_TYPE_PULLEY_1, 700.722290f, -283.990662f, 39.517582f, 3.141593f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_PULLEY_2, BG_RV_OBJECT_TYPE_PULLEY_2, 826.303833f, -283.996429f, 39.517582f, 0.000000f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
    // Pilars
        !AddObject(BG_RV_OBJECT_PILAR_1, BG_RV_OBJECT_TYPE_PILAR_1, 763.632385f, -306.162384f, 25.909504f, 3.141593f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_PILAR_2, BG_RV_OBJECT_TYPE_PILAR_2, 723.644287f, -284.493256f, 24.648525f, 3.141593f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_PILAR_3, BG_RV_OBJECT_TYPE_PILAR_3, 763.611145f, -261.856750f, 25.909504f, 0.000000f, 0, 0, 0, RESPAWN_IMMEDIATELY) ||
        !AddObject(BG_RV_OBJECT_PILAR_4, BG_RV_OBJECT_TYPE_PILAR_4, 802.211609f, -284.493256f, 24.648525f, 0.000000f, 0, 0, 0, RESPAWN_IMMEDIATELY))
    {
        sLog->outErrorDb("BatteGroundRV: Failed to spawn some object!");
        return false;
    }
    return true;
}
