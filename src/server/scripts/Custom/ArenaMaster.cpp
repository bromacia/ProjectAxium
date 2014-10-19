#include "ArenaMaster.h"

ArenaMaster::ArenaMaster() : CreatureScript("ArenaMaster")
{
}

bool ArenaMaster::OnGossipHello(Player* player, Creature* creature)
{
    CreateArenasMap();

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_letter_17:30|t Create An Arena Team", GOSSIP_SENDER_MAIN, ARENA_SPECTATE_MENU_CREATE_TEAM);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\ability_dualwield:30|t Queue For Arena", GOSSIP_SENDER_MAIN, ARENA_SPECTATE_MENU_QUEUE);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\Achievement_arena_2v2_2:30|t Spectate 2v2 Matches", GOSSIP_SENDER_MAIN, ARENA_SPECTATE_MENU_2V2_MATCHES);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\Achievement_arena_3v3_2:30|t Spectate 3v3 Matches", GOSSIP_SENDER_MAIN, ARENA_SPECTATE_MENU_3V3_MATCHES);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\icons\\Achievement_arena_5v5_2:30|t Spectate 5v5 Matches", GOSSIP_SENDER_MAIN, ARENA_SPECTATE_MENU_5V5_MATCHES);
    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_grouplooking:30|t Spectate a Player", GOSSIP_SENDER_MAIN, ARENA_SPECTATE_MENU_SPECTATE_PLAYER, "", 0, true);
    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    return true;
}

bool ArenaMaster::OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();

    switch (sender)
    {
        case GOSSIP_SENDER_MAIN:
        {
            switch (action)
            {
                case GOSSIP_SENDER_MAIN:
                {
                    OnGossipHello(player, creature);
                    break;
                }
                case ARENA_SPECTATE_MENU_CREATE_TEAM:
                {
                    HandleCreateTeam(player, creature);
                    break;
                }
                case ARENA_SPECTATE_MENU_QUEUE:
                {
                    HandleQueue(player);
                    break;
                }
                case ARENA_SPECTATE_MENU_2V2_MATCHES:
                case ARENA_SPECTATE_MENU_3V3_MATCHES:
                case ARENA_SPECTATE_MENU_5V5_MATCHES:
                {
                    HandleShowMatches(player, creature, sender, action);
                    break;
                }
            }
            break;
        }
        case ARENA_SPECTATE_MENU_2V2_MATCHES:
        case ARENA_SPECTATE_MENU_3V3_MATCHES:
        case ARENA_SPECTATE_MENU_5V5_MATCHES:
        {
            switch (action)
            {
                case ARENA_SPECTATE_MENU_PAGE_NEXT:
                case ARENA_SPECTATE_MENU_PAGE_PREVIOUS:
                {
                    HandleShowMatches(player, creature, sender, action);
                    break;
                }
                default:
                {
                    AddPlayerToArena(player, action);
                    break;
                }
            }
            break;
        }
    }
    return true;
}

bool ArenaMaster::OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
{
    switch (action) { case ARENA_SPECTATE_MENU_SPECTATE_PLAYER: HandleSpectatePlayer(player, code); break; }
    return true;
}

void ArenaMaster::HandleCreateTeam(Player* player, Creature* creature)
{
    player->CLOSE_GOSSIP_MENU();

    WorldPacket data(SMSG_PETITION_SHOWLIST, 81);
    data << creature->GetGUID();
    data << uint8(3);                                                               // count
    // 2v2
    data << uint32(1);                                                              // index
    data << uint32(23560);                                                          // charter entry
    data << uint32(16161);                                                          // charter display id
    data << uint32(0);                                                              // charter cost
    data << uint32(2);                                                              // unknown
    data << uint32(sWorld->getIntConfig(CONFIG_REQUIRED_2V2_CHARTER_SIGNATURES));   // required signs?
    // 3v3
    data << uint32(2);                                                              // index
    data << uint32(23561);                                                          // charter entry
    data << uint32(16161);                                                          // charter display id
    data << uint32(0);                                                              // charter cost
    data << uint32(3);                                                              // unknown
    data << uint32(sWorld->getIntConfig(CONFIG_REQUIRED_3V3_CHARTER_SIGNATURES));   // required signs?
    // 5v5
    data << uint32(3);                                                              // index
    data << uint32(23562);                                                          // charter entry
    data << uint32(16161);                                                          // charter display id
    data << uint32(0);                                                              // charter cost
    data << uint32(5);                                                              // unknown
    data << uint32(sWorld->getIntConfig(CONFIG_REQUIRED_5V5_CHARTER_SIGNATURES));   // required signs?
    player->GetSession()->SendPacket(&data);
}

void ArenaMaster::HandleQueue(Player* player)
{
    player->CLOSE_GOSSIP_MENU();
    player->GetSession()->SendBattlegGroundList(player->GetGUID(), BATTLEGROUND_AA);
}

void ArenaMaster::HandleShowMatches(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    ChatHandler handler = ChatHandler(player);
    CreateArenasMap();
    uint8 arenaType = 0;
    uint32 gossipAction = 0;

    switch (sender)
    {
        case GOSSIP_SENDER_MAIN: arenaType = GetArenaTypeByAction(action); gossipAction = action; break;
        default:                 arenaType = GetArenaTypeByAction(sender); gossipAction = sender; break;
    }

    uint8 roomLeft = MAX_RESULTS_PER_PAGE;

    if (arenasMap.empty())
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "No Arena Matches In Progress", GOSSIP_SENDER_MAIN, gossipAction);
    else
    {
        BattlegroundsMap::iterator itr = arenasMap.begin();

        switch (action)
        {
            case ARENA_SPECTATE_MENU_PAGE_NEXT:     player->IncrementGossipPage();
            case ARENA_SPECTATE_MENU_PAGE_PREVIOUS: player->DecrementGossipPage();
            default:
            {
                uint8 gossipPage = player->GetGossipPage();
                if (gossipPage < 1)
                {
                    sLog->outError("ArenaSpectateMgr:: Gossip Page below 1 for player: [%u]%s", player->GetGUIDLow(), player->GetName());
                    handler.PSendSysMessage("Something went wrong, try again.");
                    player->CLOSE_GOSSIP_MENU();
                    return;
                }
                else if (gossipPage > 1)
                {
                    uint16 increment = (--gossipPage) * roomLeft;
                    if (increment < 50)
                    {
                        sLog->outError("ArenaSpectateMgr:: Gossip increment less than 50 for player: [%u]%s", player->GetGUIDLow(), player->GetName());
                        handler.PSendSysMessage("Something went wrong, try again.");
                        player->CLOSE_GOSSIP_MENU();
                        return;
                    }

                    for (uint16 i = 0; i < increment; ++i)
                    {
                        ++itr;
                        if (itr == arenasMap.end())
                            break;
                    }
                    break;
                }
            }
        }

        for (; itr != arenasMap.end(); ++itr)
        {
            Battleground* arena = itr->second;
            if (!arena)
                continue;

            if (!CheckBattleground(arena))
                continue;

            if (!arena->GetBgMap())
                continue;

            if (arena->GetArenaType() != arenaType)
                continue;

            if (!arena->isRated())
                continue;

            ArenaTeam* goldTeam = sArenaTeamMgr->GetArenaTeamById(arena->GetArenaTeamIdByIndex(BG_TEAM_ALLIANCE));
            if (!goldTeam)
                continue;

            ArenaTeam* greenTeam = sArenaTeamMgr->GetArenaTeamById(arena->GetArenaTeamIdByIndex(BG_TEAM_HORDE));
            if (!greenTeam)
                continue;

            if (goldTeam->GetStats().Rating < sWorld->getIntConfig(CONFIG_ARENA_SPECTATOR_MIN_RATING) && greenTeam->GetStats().Rating < sWorld->getIntConfig(CONFIG_ARENA_SPECTATOR_MIN_RATING))
                continue;

            std::stringstream gossipText;
            // GoldTeamName[GoldTeamRating] vs GreenTeamName[GreenTeamRating]
            gossipText << goldTeam->GetName() << "[" << goldTeam->GetRating() << "] vs " << greenTeam->GetName() << "[" << greenTeam->GetRating() << "]";
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, gossipText.str(), gossipAction, itr->first);

            --roomLeft;

            if (!roomLeft)
                break;
        }

        if (roomLeft == MAX_RESULTS_PER_PAGE)
        {
            std::stringstream gossipText;
            gossipText << "No Arena Matches Above " << sWorld->getIntConfig(CONFIG_ARENA_SPECTATOR_MIN_RATING) << " Rating In Progress";
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, gossipText.str(), GOSSIP_SENDER_MAIN, gossipAction);
        }

        if (itr != arenasMap.end())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next Page", gossipAction, ARENA_SPECTATE_MENU_PAGE_NEXT);

            if (sender != GOSSIP_SENDER_MAIN)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Previous Page", gossipAction, ARENA_SPECTATE_MENU_PAGE_PREVIOUS);
        }
    }

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_MAIN);
    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
}

void ArenaMaster::HandleSpectatePlayer(Player* player, const char* cPlayerName)
{
    ChatHandler handler = ChatHandler(player);
    CreateArenasMap();
    uint32 arenaId = 0;
    Player* target = NULL;
    uint64 tmpGuid = sObjectMgr->GetPlayerGUIDByName(cPlayerName);
    uint32 guidLow = GUID_LOPART(tmpGuid);

    player->CLOSE_GOSSIP_MENU();

    if (!guidLow)
    {
        handler.PSendSysMessage("Unable to find a player with that name.");
        return;
    }

    target = sObjectMgr->GetPlayerByLowGUID(guidLow);
    if (!target)
    {
        handler.PSendSysMessage("The player you're trying to spectate is offline.");
        return;
    }

    arenaId = target->GetBattlegroundId();
    if (!arenaId)
    {
        handler.PSendSysMessage("The player you're trying to spectate isn't in a arena.");
        return;
    }

    if (!arenasMap[arenaId])
    {
        handler.PSendSysMessage("The arena match of the player you're trying to spectate either hasn't started yet or doesn't exist.");
        return;
    }

    AddPlayerToArena(player, arenaId);
}

void ArenaMaster::AddPlayerToArena(Player* player, uint32 action)
{
    ChatHandler handler = ChatHandler(player);
    CreateArenasMap();
    player->CLOSE_GOSSIP_MENU();

    if (!arenasMap[action])
    {
        handler.PSendSysMessage("The arena match you're trying to spectate either hasn't started yet or doesn't exist.");
        return;
    }

    if (!CheckBattleground(arenasMap[action]))
    {
        handler.PSendSysMessage("The arena match you're trying to spectate hasn't started yet.");
        return;
    }

    if (!arenasMap[action]->GetBgMap())
    {
        handler.PSendSysMessage("The map for the arena match you're trying to spectate is being deconstructed.");
        return;
    }

    Battleground* arena = sBattlegroundMgr->GetBattleground(action, arenasMap[action]->GetTypeID(false));
    if (!arena)
    {
        handler.PSendSysMessage("The arena match of the player you're trying to spectate no longer exists.");
        return;
    }

    player->RemoveFromAllBattlegroundQueues();
    player->SetArenaSpectatorState(true);
    player->SetBattlegroundId(action, arena->GetTypeID(false));
    player->SetBattlegroundEntryPoint();
    arena->HandlePlayerUnderMap(player);
}

bool ArenaMaster::CheckBattleground(Battleground* bg)
{
    if (bg->GetStatus() != STATUS_IN_PROGRESS)
        return false;

    if (!bg->GetPlayersCountByTeam(ALLIANCE) || !bg->GetPlayersCountByTeam(HORDE))
        return false;

    return true;
}

void ArenaMaster::CreateArenasMap()
{
    arenasMap.clear();

    BattlegroundsMap tempMap;

    // Nagrand Arena
    tempMap = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BATTLEGROUND_NA);
    for (BattlegroundsMap::iterator itr = tempMap.begin(); itr != tempMap.end(); ++itr)
        if (CheckBattleground(itr->second))
            arenasMap[itr->first] = itr->second;

    // Blade's Edge Arena
    tempMap = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BATTLEGROUND_BE);
    for (BattlegroundsMap::iterator itr = tempMap.begin(); itr != tempMap.end(); ++itr)
        if (CheckBattleground(itr->second))
            arenasMap[itr->first] = itr->second;

    // Ruins of Lordaeran
    tempMap = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BATTLEGROUND_RL);
    for (BattlegroundsMap::iterator itr = tempMap.begin(); itr != tempMap.end(); ++itr)
        if (CheckBattleground(itr->second))
            arenasMap[itr->first] = itr->second;

    // Dalaran Sewers
    tempMap = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BATTLEGROUND_DS);
    for (BattlegroundsMap::iterator itr = tempMap.begin(); itr != tempMap.end(); ++itr)
        if (CheckBattleground(itr->second))
            arenasMap[itr->first] = itr->second;

    // Ring of Valor
    tempMap = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BATTLEGROUND_RV);
    for (BattlegroundsMap::iterator itr = tempMap.begin(); itr != tempMap.end(); ++itr)
        if (CheckBattleground(itr->second))
            arenasMap[itr->first] = itr->second;
}

void AddSC_ArenaMaster()
{
    new ArenaMaster();
}
