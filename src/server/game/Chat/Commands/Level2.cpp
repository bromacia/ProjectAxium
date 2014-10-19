#include "Common.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Item.h"
#include "GameObject.h"
#include "Opcodes.h"
#include "Chat.h"
#include "MapManager.h"
#include "Language.h"
#include "World.h"
#include "GameEventMgr.h"
#include "SpellMgr.h"
#include "PoolMgr.h"
#include "AccountMgr.h"
#include "WaypointManager.h"
#include "Util.h"
#include <cctype>
#include <iostream>
#include <fstream>
#include <map>
#include "OutdoorPvPMgr.h"
#include "Transport.h"
#include "TargetedMovementGenerator.h"                      // for HandleNpcUnFollowCommand
#include "CreatureGroups.h"

//mute player for some times
bool ChatHandler::HandleMuteCommand(const char* args)
{
    char* nameStr;
    char* delayStr;
    extractOptFirstArg((char*)args, &nameStr, &delayStr);
    if (!delayStr)
        return false;

    char *mutereason = strtok(NULL, "\r");
    std::string mutereasonstr = "No reason";
    if (mutereason != NULL)
         mutereasonstr = mutereason;

    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget(nameStr, &target, &target_guid, &target_name))
        return false;

    uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(target_guid);

    // find only player from same account if any
    if (!target)
        if (WorldSession* session = sWorld->FindSession(accountId))
            target = session->GetPlayer();

    uint32 notspeaktime = (uint32) atoi(delayStr);

    // must have strong lesser security level
    if (HasLowerSecurity (target, target_guid, true))
        return false;

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);

    if (target)
    {
        // Target is online, mute will be in effect right away.
        int64 muteTime = time(NULL) + notspeaktime * MINUTE;
        target->GetSession()->m_muteTime = muteTime;

        stmt->setInt64(0, muteTime);

        target->SendSysMessage(LANG_YOUR_CHAT_DISABLED, notspeaktime, mutereasonstr.c_str());
    }
    else
    {
        // Target is offline, mute will be in effect starting from the next login.
        int32 muteTime = -int32(notspeaktime * MINUTE);

        stmt->setInt64(0, muteTime);
    }

    stmt->setUInt32(1, accountId);

    LoginDatabase.Execute(stmt);

    std::string nameLink = playerLink(target_name);

    PSendSysMessage(target ? LANG_YOU_DISABLE_CHAT : LANG_COMMAND_DISABLE_CHAT_DELAYED, nameLink.c_str(), notspeaktime, mutereasonstr.c_str());

    return true;
}

//unmute player
bool ChatHandler::HandleUnmuteCommand(const char* args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(target_guid);

    // find only player from same account if any
    if (!target)
        if (WorldSession* session = sWorld->FindSession(accountId))
            target = session->GetPlayer();

    // must have strong lesser security level
    if (HasLowerSecurity (target, target_guid, true))
        return false;

    if (target)
    {
        if (target->CanSpeak())
        {
            SendSysMessage(LANG_CHAT_ALREADY_ENABLED);
            SetSentErrorMessage(true);
            return false;
        }

        target->GetSession()->m_muteTime = 0;
    }

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);

    stmt->setInt64(0, 0);
    stmt->setUInt32(1, accountId);

    LoginDatabase.Execute(stmt);

    if (target)
        target->SendSysMessage(LANG_YOUR_CHAT_ENABLED);

    std::string nameLink = playerLink(target_name);

    PSendSysMessage(LANG_YOU_ENABLE_CHAT, nameLink.c_str());
    return true;
}

bool ChatHandler::HandleGUIDCommand(const char* /*args*/)
{
    uint64 guid = m_session->GetPlayer()->GetSelection();

    if (guid == 0)
    {
        SendSysMessage(LANG_NO_SELECTION);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_OBJECT_GUID, GUID_LOPART(guid), GUID_HIPART(guid));
    return true;
}

 //move item to other slot
bool ChatHandler::HandleItemMoveCommand(const char* args)
{
    if (!*args)
        return false;
    uint8 srcslot, dstslot;

    char* pParam1 = strtok((char*)args, " ");
    if (!pParam1)
        return false;

    char* pParam2 = strtok(NULL, " ");
    if (!pParam2)
        return false;

    srcslot = (uint8)atoi(pParam1);
    dstslot = (uint8)atoi(pParam2);

    if (srcslot == dstslot)
        return true;

    if (!m_session->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, srcslot, true))
        return false;

    if (!m_session->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, dstslot, false))
        return false;

    uint16 src = ((INVENTORY_SLOT_BAG_0 << 8) | srcslot);
    uint16 dst = ((INVENTORY_SLOT_BAG_0 << 8) | dstslot);

    m_session->GetPlayer()->SwapItem(src, dst);

    return true;
}

//kick player
bool ChatHandler::HandleKickPlayerCommand(const char* args)
{
    Player* target = NULL;
    std::string playerName;
    if (!extractPlayerTarget((char*)args, &target, NULL, &playerName))
        return false;

    if (m_session && target == m_session->GetPlayer())
    {
        SendSysMessage(LANG_COMMAND_KICKSELF);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(target, 0))
        return false;

    if (sWorld->getBoolConfig(CONFIG_SHOW_KICK_IN_WORLD))
        sWorld->SendWorldText(LANG_COMMAND_KICKMESSAGE, playerName.c_str());
    else
        PSendSysMessage(LANG_COMMAND_KICKMESSAGE, playerName.c_str());

    target->GetSession()->CloseSession();
    return true;
}

//show info of player
bool ChatHandler::HandlePInfoCommand(const char* args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;

    uint32 parseGUID = MAKE_NEW_GUID(atol((char*)args), 0, HIGHGUID_PLAYER);

    if (sObjectMgr->GetPlayerNameByGUID(parseGUID, target_name))
    {
        target = sObjectMgr->GetPlayerByLowGUID(parseGUID);
        target_guid = parseGUID;
    }
    else if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    uint32 accId = 0;
    uint32 total_player_time = 0;
    uint32 latency = 0;
    uint8 race;
    uint8 Class;
    uint32 spec = 0;
    int64 muteTime = 0;
    int64 banTime = -1;
    uint32 mapId;
    uint32 areaId;
    uint32 phase = 0;

    // get additional information from Player object
    if (target)
    {
        // check online security
        if (HasLowerSecurity(target, 0))
            return false;

        accId = target->GetSession()->GetAccountId();
        total_player_time = target->GetTotalPlayedTime();
        latency = target->GetSession()->GetLatency();
        race = target->getRace();
        Class = target->getClass();
        spec = target->m_playerSpec;
        muteTime = target->GetSession()->m_muteTime;
        mapId = target->GetMapId();
        areaId = target->GetAreaId();
        phase = target->GetPhaseMask();
    }
    // get additional information from DB
    else
    {
        // check offline security
        if (HasLowerSecurity(NULL, target_guid))
            return false;

        //                                                    0          1        2     3      4    5     6
        QueryResult result = CharacterDatabase.PQuery("SELECT totaltime, account, race, class, map, zone, spec FROM characters "
                                                      "WHERE guid = '%u'", GUID_LOPART(target_guid));
        if (!result)
            return false;

        Field* fields = result->Fetch();
        total_player_time = fields[0].GetUInt32();
        accId = fields[1].GetUInt32();
        race = fields[2].GetUInt8();
        Class = fields[3].GetUInt8();
        mapId = fields[4].GetUInt16();
        areaId = fields[5].GetUInt16();
        spec = fields[6].GetUInt32();
    }

    std::string username = GetAxiumString(LANG_ERROR);
    std::string email = GetAxiumString(LANG_ERROR);
    std::string last_ip = GetAxiumString(LANG_ERROR);
    uint32 security = 0;
    std::string securityString = GetAxiumString(LANG_ERROR);
    std::string last_login = GetAxiumString(LANG_ERROR);

    QueryResult result = LoginDatabase.PQuery("SELECT a.username, aa.gmlevel, a.email, a.last_ip, a.last_login, a.mutetime "
                                                "FROM account a "
                                                "LEFT JOIN account_access aa "
                                                "ON (a.id = aa.id AND (aa.RealmID = -1 OR aa.RealmID = %u)) "
                                                "WHERE a.id = '%u'", realmID, accId);
    if (result)
    {
        Field* fields = result->Fetch();
        username = fields[0].GetString();
        security = fields[1].GetUInt32();
        email = fields[2].GetString();
        muteTime = fields[5].GetUInt64();

        if (email.empty())
            email = "Unknown";

        if (!m_session || m_session->GetSecurity() >= AccountTypes(security))
        {
            last_ip = fields[3].GetString();
            last_login = fields[4].GetString();
        }
        else
        {
            last_ip = "Unknown";
            last_login = "Unknown";
        }

        switch (security)
        {
            case 0: securityString  = "PLAYER";          break;
            case 1: securityString  = "VIP";             break;
            case 2: securityString  = "GAMEMASTER";      break;
            case 3: securityString  = "HEAD GAMEMASTER"; break;
            case 4: securityString  = "ADMINISTRATOR";   break;
            case 5: securityString  = "CONSOLE";         break;
            default: securityString = "UNKNOWN";         break;
        }
    }

    std::string nameLink = playerLink(target_name);

    PSendSysMessage("Player%s %s (guid: %u, account: %u): %s, Email: %s, Security: %s, Last IP: %s, Last login: %s, Latency: %ums",
    (target ? "" : "(Offline)"), nameLink.c_str(), GUID_LOPART(target_guid), accId, username.c_str(), email.c_str(), securityString.c_str(), last_ip.c_str(), last_login.c_str(), latency);

    std::string bannedby = "Unknown";
    std::string banreason = "";
    if (QueryResult result2 = LoginDatabase.PQuery("SELECT unbandate, bandate = unbandate, bannedby, banreason FROM account_banned "
                                                  "WHERE id = '%u' AND active ORDER BY bandate ASC LIMIT 1", accId))
    {
        Field* fields = result2->Fetch();
        banTime = fields[1].GetBool() ? 0 : fields[0].GetUInt64();
        bannedby = fields[2].GetString();
        banreason = fields[3].GetString();
    }
    else if (QueryResult result3 = CharacterDatabase.PQuery("SELECT unbandate, bandate = unbandate, bannedby, banreason FROM character_banned "
                                                           "WHERE guid = '%u' AND active ORDER BY bandate ASC LIMIT 1", GUID_LOPART(target_guid)))
    {
        Field* fields = result3->Fetch();
        banTime = fields[1].GetBool() ? 0 : fields[0].GetUInt64();
        bannedby = fields[2].GetString();
        banreason = fields[3].GetString();
    }

    if (muteTime > 0 && muteTime >= time(NULL))
        PSendSysMessage("Mute time remaining: %s", secsToTimeString(muteTime - time(NULL), true).c_str());

    if (banTime >= 0)
        PSendSysMessage("Ban time remaining: %s, Banned by: %s, Reason: %s", banTime > 0 ? secsToTimeString(banTime - time(NULL), true).c_str() : "permanently", bannedby.c_str(), banreason.c_str());

    std::string race_s, Class_s, spec_s;
    switch (race)
    {
        case RACE_HUMAN:            race_s = "Human";       break;
        case RACE_ORC:              race_s = "Orc";         break;
        case RACE_DWARF:            race_s = "Dwarf";       break;
        case RACE_NIGHTELF:         race_s = "Night Elf";   break;
        case RACE_UNDEAD_PLAYER:    race_s = "Undead";      break;
        case RACE_TAUREN:           race_s = "Tauren";      break;
        case RACE_GNOME:            race_s = "Gnome";       break;
        case RACE_TROLL:            race_s = "Troll";       break;
        case RACE_BLOODELF:         race_s = "Blood Elf";   break;
        case RACE_DRAENEI:          race_s = "Draenei";     break;
    }
    switch (Class)
    {
        case CLASS_WARRIOR:         Class_s = "Warrior";        break;
        case CLASS_PALADIN:         Class_s = "Paladin";        break;
        case CLASS_HUNTER:          Class_s = "Hunter";         break;
        case CLASS_ROGUE:           Class_s = "Rogue";          break;
        case CLASS_PRIEST:          Class_s = "Priest";         break;
        case CLASS_DEATH_KNIGHT:    Class_s = "Death Knight";   break;
        case CLASS_SHAMAN:          Class_s = "Shaman";         break;
        case CLASS_MAGE:            Class_s = "Mage";           break;
        case CLASS_WARLOCK:         Class_s = "Warlock";        break;
        case CLASS_DRUID:           Class_s = "Druid";          break;
    }
    switch (spec)
    {
        case PLAYERSPEC_WARRIOR_ARMS:           spec_s = "Arms";            break;
        case PLAYERSPEC_WARRIOR_FURY:           spec_s = "Fury";            break;
        case PLAYERSPEC_WARRIOR_PROTECTION:     spec_s = "Protection";      break;
        case PLAYERSPEC_PALADIN_HOLY:           spec_s = "Holy";            break;
        case PLAYERSPEC_PALADIN_PROTECTION:     spec_s = "Protection";      break;
        case PLAYERSPEC_PALADIN_RETRIBUTION:    spec_s = "Retribution";     break;
        case PLAYERSPEC_DEATHKNIGHT_BLOOD:      spec_s = "Blood";           break;
        case PLAYERSPEC_DEATHKNIGHT_FROST:      spec_s = "Frost";           break;
        case PLAYERSPEC_DEATHKNIGHT_UNHOLY:     spec_s = "Unholy";          break;
        case PLAYERSPEC_HUNTER_BEASTMASTERY:    spec_s = "Beast Mastery";   break;
        case PLAYERSPEC_HUNTER_MARKSMANSHIP:    spec_s = "Marksmanship";    break;
        case PLAYERSPEC_HUNTER_SURVIVAL:        spec_s = "Survival";        break;
        case PLAYERSPEC_SHAMAN_ELEMENTAL:       spec_s = "Elemental";       break;
        case PLAYERSPEC_SHAMAN_ENHANCEMENT:     spec_s = "Enhancement";     break;
        case PLAYERSPEC_SHAMAN_RESTORATION:     spec_s = "Restoration";     break;
        case PLAYERSPEC_ROGUE_ASSASSINATION:    spec_s = "Assassination";   break;
        case PLAYERSPEC_ROGUE_COMBAT:           spec_s = "Combat";          break;
        case PLAYERSPEC_ROGUE_SUBLETY:          spec_s = "Sublety";         break;
        case PLAYERSPEC_DRUID_BALANCE:          spec_s = "Balance";         break;
        case PLAYERSPEC_DRUID_FERALCOMBAT:      spec_s = "Feral";           break;
        case PLAYERSPEC_DRUID_RESTORATION:      spec_s = "Restoration";     break;
        case PLAYERSPEC_PRIEST_DISCIPLINE:      spec_s = "Discipline";      break;
        case PLAYERSPEC_PRIEST_HOLY:            spec_s = "Holy";            break;
        case PLAYERSPEC_PRIEST_SHADOW:          spec_s = "Shadow";          break;
        case PLAYERSPEC_MAGE_ARCANE:            spec_s = "Arcane";          break;
        case PLAYERSPEC_MAGE_FIRE:              spec_s = "Fire";            break;
        case PLAYERSPEC_MAGE_FROST:             spec_s = "Frost";           break;
        case PLAYERSPEC_WARLOCK_AFFLICTION:     spec_s = "Affliction";      break;
        case PLAYERSPEC_WARLOCK_DEMONOLOGY:     spec_s = "Demonology";      break;
        case PLAYERSPEC_WARLOCK_DESTRUCTION:    spec_s = "Destruction";     break;
        default:                                spec_s = "Hybrid/None";     break;
    }

    std::string timeStr = secsToTimeString(total_player_time, true, true);
    PSendSysMessage("Race: %s, Class: %s, Spec: %s, Played Time: %s", race_s.c_str(), Class_s.c_str(), spec_s.c_str(), timeStr.c_str());

    // Add map, zone, subzone and phase to output
    int locale = GetSessionDbcLocale();
    std::string areaName = "<Unknown>";
    std::string zoneName = "";

    MapEntry const* map = sMapStore.LookupEntry(mapId);

    AreaTableEntry const* area = GetAreaEntryByAreaID(areaId);
    if (area)
    {
        areaName = area->area_name[locale];

        AreaTableEntry const* zone = GetAreaEntryByAreaID(area->zone);

        if (zone)
            zoneName = zone->area_name[locale];
    }

    if (target)
    {
        if (!zoneName.empty())
            PSendSysMessage("Map: %s, Area: %s, Zone: %s, Phase: %u", map->name[locale], zoneName.c_str(), areaName.c_str(), phase);
        else
            PSendSysMessage("Map: %s, Area: %s, Zone: %s, Phase: %u", map->name[locale], areaName.c_str(), "<Unknown>", phase);
    }
    else
        PSendSysMessage("Map: %s, Area: %s", map->name[locale], areaName.c_str());

    return true;
}

//rename characters
bool ChatHandler::HandleCharacterRenameCommand(const char* args)
{
    Player* target;
    uint64 targetGuid;
    std::string targetName;
    if (!extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
        return false;

    if (target)
    {
        // check online security
        if (HasLowerSecurity(target, 0))
            return false;

        PSendSysMessage(LANG_RENAME_PLAYER, GetNameLink(target).c_str());
        target->SetAtLoginFlag(AT_LOGIN_RENAME);
    }
    else
    {
        // check offline security
        if (HasLowerSecurity(NULL, targetGuid))
            return false;

        std::string oldNameLink = playerLink(targetName);

        PSendSysMessage(LANG_RENAME_PLAYER_GUID, oldNameLink.c_str(), GUID_LOPART(targetGuid));

        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);

        stmt->setUInt16(0, uint16(AT_LOGIN_RENAME));
        stmt->setUInt32(1, GUID_LOPART(targetGuid));

        CharacterDatabase.Execute(stmt);
    }

    return true;
}

// customize characters
bool ChatHandler::HandleCharacterCustomizeCommand(const char* args)
{
    Player* target;
    uint64 targetGuid;
    std::string targetName;
    if (!extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
        return false;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);

    stmt->setUInt16(0, uint16(AT_LOGIN_CUSTOMIZE));

    if (target)
    {
        PSendSysMessage(LANG_CUSTOMIZE_PLAYER, GetNameLink(target).c_str());
        target->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);

        stmt->setUInt32(1, target->GetGUIDLow());
    }
    else
    {
        std::string oldNameLink = playerLink(targetName);

        stmt->setUInt32(1, GUID_LOPART(targetGuid));

        PSendSysMessage(LANG_CUSTOMIZE_PLAYER_GUID, oldNameLink.c_str(), GUID_LOPART(targetGuid));
    }

    CharacterDatabase.Execute(stmt);

    return true;
}

bool ChatHandler::HandleCharacterChangeFactionCommand(const char* args)
{
    Player* target;
    uint64 targetGuid;
    std::string targetName;

    if (!extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
        return false;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);

    stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_FACTION));

    if (target)
    {
        PSendSysMessage(LANG_CUSTOMIZE_PLAYER, GetNameLink(target).c_str());
        target->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);

        stmt->setUInt32(1, target->GetGUIDLow());
    }
    else
    {
        std::string oldNameLink = playerLink(targetName);

        PSendSysMessage(LANG_CUSTOMIZE_PLAYER_GUID, oldNameLink.c_str(), GUID_LOPART(targetGuid));

        stmt->setUInt32(1, GUID_LOPART(targetGuid));
    }

    CharacterDatabase.Execute(stmt);

    return true;
}

bool ChatHandler::HandleCharacterChangeRaceCommand(const char* args)
{
    Player* target;
    uint64 targetGuid;
    std::string targetName;
    if (!extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
        return false;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);

    stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_RACE));

    if (target)
    {
        // TODO : add text into database
        PSendSysMessage(LANG_CUSTOMIZE_PLAYER, GetNameLink(target).c_str());
        target->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);

        stmt->setUInt32(1, target->GetGUIDLow());
    }
    else
    {
        std::string oldNameLink = playerLink(targetName);

        // TODO : add text into database
        PSendSysMessage(LANG_CUSTOMIZE_PLAYER_GUID, oldNameLink.c_str(), GUID_LOPART(targetGuid));

        stmt->setUInt32(1, GUID_LOPART(targetGuid));
    }

    CharacterDatabase.Execute(stmt);

    return true;
}

bool ChatHandler::HandleCharacterReputationCommand(const char* args)
{
    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

    LocaleConstant loc = GetSessionDbcLocale();

    FactionStateList const& targetFSL = target->GetReputationMgr().GetStateList();
    for (FactionStateList::const_iterator itr = targetFSL.begin(); itr != targetFSL.end(); ++itr)
    {
        const FactionState& faction = itr->second;
        FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction.ID);
        char const* factionName = factionEntry ? factionEntry->name[loc] : "#Not found#";
        ReputationRank rank = target->GetReputationMgr().GetRank(factionEntry);
        std::string rankName = GetAxiumString(ReputationRankStrIndex[rank]);
        std::ostringstream ss;
        if (m_session)
            ss << faction.ID << " - |cffffffff|Hfaction:" << faction.ID << "|h[" << factionName << ' ' << localeNames[loc] << "]|h|r";
        else
            ss << faction.ID << " - " << factionName << ' ' << localeNames[loc];

        ss << ' ' << rankName << " (" << target->GetReputationMgr().GetReputation(factionEntry) << ')';

        if (faction.Flags & FACTION_FLAG_VISIBLE)
            ss << GetAxiumString(LANG_FACTION_VISIBLE);
        if (faction.Flags & FACTION_FLAG_AT_WAR)
            ss << GetAxiumString(LANG_FACTION_ATWAR);
        if (faction.Flags & FACTION_FLAG_PEACE_FORCED)
            ss << GetAxiumString(LANG_FACTION_PEACE_FORCED);
        if (faction.Flags & FACTION_FLAG_HIDDEN)
            ss << GetAxiumString(LANG_FACTION_HIDDEN);
        if (faction.Flags & FACTION_FLAG_INVISIBLE_FORCED)
            ss << GetAxiumString(LANG_FACTION_INVISIBLE_FORCED);
        if (faction.Flags & FACTION_FLAG_INACTIVE)
            ss << GetAxiumString(LANG_FACTION_INACTIVE);

        SendSysMessage(ss.str().c_str());
    }
    return true;
}

bool ChatHandler::HandleLookupEventCommand(const char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld->getIntConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
    GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr->GetActiveEventList();

    for (uint32 id = 0; id < events.size(); ++id)
    {
        GameEventData const& eventData = events[id];

        std::string descr = eventData.description;
        if (descr.empty())
            continue;

        if (Utf8FitTo(descr, wnamepart))
        {
            if (maxResults && count++ == maxResults)
            {
                PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                return true;
            }

            char const* active = activeEvents.find(id) != activeEvents.end() ? GetAxiumString(LANG_ACTIVE) : "";

            if (m_session)
                PSendSysMessage(LANG_EVENT_ENTRY_LIST_CHAT, id, id, eventData.description.c_str(), active);
            else
                PSendSysMessage(LANG_EVENT_ENTRY_LIST_CONSOLE, id, eventData.description.c_str(), active);

            if (!found)
                found = true;
        }
    }

    if (!found)
        SendSysMessage(LANG_NOEVENTFOUND);

    return true;
}

bool ChatHandler::HandleCombatStopCommand(const char* args)
{
    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

    // check online security
    if (HasLowerSecurity(target, 0))
        return false;

    target->CombatStopWithPets(true);
    target->getHostileRefManager().deleteReferences();
    return true;
}

bool ChatHandler::HandleLookupPlayerIpCommand(const char* args)
{
    std::string ip;
    int32 limit;
    char* limit_str;

    Player *chr = getSelectedPlayer();
    if (!*args)
    {
        // NULL only if used from console
        if (!chr || chr == GetSession()->GetPlayer())
            return false;

        ip = chr->GetSession()->GetRemoteAddress();
        limit = -1;
    }
    else
    {
        ip = strtok((char*)args, " ");
        limit_str = strtok(NULL, " ");
        limit = limit_str ? atoi(limit_str) : -1;
    }

    LoginDatabase.EscapeString(ip);

    QueryResult result = LoginDatabase.PQuery("SELECT id, username FROM account WHERE last_ip = '%s'", ip.c_str());

    return LookupPlayerSearchCommand(result, limit);
}

bool ChatHandler::HandleLookupPlayerAccountCommand(const char* args)
{
    if (!*args)
        return false;

    std::string account = strtok((char*)args, " ");
    char* limit_str = strtok(NULL, " ");
    int32 limit = limit_str ? atoi(limit_str) : -1;

    if (!AccountMgr::normalizeString(account))
        return false;

    LoginDatabase.EscapeString(account);

    QueryResult result = LoginDatabase.PQuery("SELECT id, username FROM account WHERE username = '%s'", account.c_str());

    return LookupPlayerSearchCommand(result, limit);
}

bool ChatHandler::HandleLookupPlayerEmailCommand(const char* args)
{
    if (!*args)
        return false;

    std::string email = strtok((char*)args, " ");
    char* limit_str = strtok(NULL, " ");
    int32 limit = limit_str ? atoi(limit_str) : -1;

    LoginDatabase.EscapeString(email);

    QueryResult result = LoginDatabase.PQuery("SELECT id, username FROM account WHERE email = '%s'", email.c_str());

    return LookupPlayerSearchCommand(result, limit);
}

bool ChatHandler::LookupPlayerSearchCommand(QueryResult result, int32 limit)
{
    if (!result)
    {
        PSendSysMessage(LANG_NO_PLAYERS_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    int i = 0;
    uint32 count = 0;
    uint32 maxResults = sWorld->getIntConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);
    do
    {
        if (maxResults && count++ == maxResults)
        {
            PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
            return true;
        }

        Field* fields = result->Fetch();
        uint32 acc_id = fields[0].GetUInt32();
        std::string acc_name = fields[1].GetString();

        QueryResult chars = CharacterDatabase.PQuery("SELECT guid, name FROM characters WHERE account = '%u'", acc_id);
        if (chars)
        {
            PSendSysMessage(LANG_LOOKUP_PLAYER_ACCOUNT, acc_name.c_str(), acc_id);

            uint64 guid = 0;
            std::string name;

            do
            {
                Field* charfields = chars->Fetch();
                guid = charfields[0].GetUInt64();
                name = charfields[1].GetString();

                PSendSysMessage(LANG_LOOKUP_PLAYER_CHARACTER, name.c_str(), guid);
                ++i;

            } while (chars->NextRow() && (limit == -1 || i < limit));
        }
    } while (result->NextRow());

    if (i == 0)                                                // empty accounts only
    {
        PSendSysMessage(LANG_NO_PLAYERS_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

/// Triggering corpses expire check in world
bool ChatHandler::HandleServerCorpsesCommand(const char* /*args*/)
{
    sObjectAccessor->RemoveOldCorpses();
    return true;
}

bool ChatHandler::HandleRepairitemsCommand(const char* args)
{
    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

    // check online security
    if (HasLowerSecurity(target, 0))
        return false;

    // Repair items
    target->DurabilityRepairAll(false, 0, false);

    PSendSysMessage(LANG_YOU_REPAIR_ITEMS, GetNameLink(target).c_str());
    if (needReportToTarget(target))
        target->SendSysMessage(LANG_YOUR_ITEMS_REPAIRED, GetNameLink().c_str());
    return true;
}

bool ChatHandler::HandleWaterwalkCommand(const char* args)
{
    if (!*args)
        return false;

    Player* player = getSelectedPlayer();

    if (!player)
    {
        PSendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(player, 0))
        return false;

    if (strncmp(args, "on", 3) == 0)
        player->SetMovement(MOVE_WATER_WALK);               // ON
    else if (strncmp(args, "off", 4) == 0)
        player->SetMovement(MOVE_LAND_WALK);                // OFF
    else
    {
        SendSysMessage(LANG_USE_BOL);
        return false;
    }

    PSendSysMessage(LANG_YOU_SET_WATERWALK, args, GetNameLink(player).c_str());
    if (needReportToTarget(player))
        player->SendSysMessage(LANG_YOUR_WATERWALK_SET, args, GetNameLink().c_str());
    return true;
}

bool ChatHandler::HandleCreatePetCommand(const char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    Creature* creatureTarget = getSelectedCreature();

    if (!creatureTarget || creatureTarget->isPet() || creatureTarget->GetTypeId() == TYPEID_PLAYER)
    {
        PSendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(creatureTarget->GetEntry());
    // Creatures with family 0 crashes the server
    if (cInfo->family == 0)
    {
        PSendSysMessage("This creature cannot be tamed. (family id: 0).");
        SetSentErrorMessage(true);
        return false;
    }

    if (player->GetPetGUID())
    {
        PSendSysMessage("You already have a pet");
        SetSentErrorMessage(true);
        return false;
    }

    // Everything looks OK, create new pet
    Pet* pet = new Pet(player, HUNTER_PET);
    if (!pet->CreateBaseAtCreature(creatureTarget))
    {
        delete pet;
        PSendSysMessage("Error 1");
        return false;
    }

    creatureTarget->setDeathState(JUST_DIED);
    creatureTarget->RemoveCorpse();
    creatureTarget->SetHealth(0); // just for nice GM-mode view

    pet->SetUInt64Value(UNIT_FIELD_CREATEDBY, player->GetGUID());
    pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, player->getFaction());

    if (!pet->InitStatsForLevel(creatureTarget->getLevel()))
    {
        sLog->outError("InitStatsForLevel() in EffectTameCreature failed! Pet deleted.");
        PSendSysMessage("Error 2");
        delete pet;
        return false;
    }

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel()-1);

    pet->GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);
    // this enables pet details window (Shift+P)
    pet->InitPetCreateSpells();
    pet->SetFullHealth();

    pet->GetMap()->AddToMap(pet->ToCreature());

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel());

    player->SetMinion(pet, true);
    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    player->PetSpellInitialize();

    return true;
}

bool ChatHandler::HandlePetLearnCommand(const char* args)
{
    if (!*args)
        return false;

    Player* player = m_session->GetPlayer();
    Pet* pet = player->GetPet();

    if (!pet)
    {
        PSendSysMessage("You have no pet");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 spellId = extractSpellIdFromLink((char*)args);

    if (!spellId || !sSpellMgr->GetSpellInfo(spellId))
        return false;

    // Check if pet already has it
    if (pet->HasSpell(spellId))
    {
        PSendSysMessage("Pet already has spell: %u", spellId);
        SetSentErrorMessage(true);
        return false;
    }

    // Check if spell is valid
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spellId);
        SetSentErrorMessage(true);
        return false;
    }

    pet->learnSpell(spellId);

    PSendSysMessage("Pet has learned spell %u", spellId);
    return true;
}

bool ChatHandler::HandlePetUnlearnCommand(const char* args)
{
    if (!*args)
        return false;

    Player* player = m_session->GetPlayer();
    Pet* pet = player->GetPet();

    if (!pet)
    {
        PSendSysMessage("You have no pet");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 spellId = extractSpellIdFromLink((char*)args);

    if (pet->HasSpell(spellId))
        pet->removeSpell(spellId, false);
    else
        PSendSysMessage("Pet doesn't have that spell");

    return true;
}

bool ChatHandler::HandleLookupTitleCommand(const char* args)
{
    if (!*args)
        return false;

    // can be NULL in console call
    Player* target = getSelectedPlayer();

    // title name have single string arg for player name
    char const* targetName = target ? target->GetName() : "NAME";

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    uint32 counter = 0;                                     // Counter for figure out that we found smth.
    uint32 maxResults = sWorld->getIntConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    // Search in CharTitles.dbc
    for (uint32 id = 0; id < sCharTitlesStore.GetNumRows(); id++)
    {
        CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(id);
        if (titleInfo)
        {
            int loc = GetSessionDbcLocale();
            std::string name = titleInfo->name[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, wnamepart))
            {
                loc = 0;
                for (; loc < TOTAL_LOCALES; ++loc)
                {
                    if (loc == GetSessionDbcLocale())
                        continue;

                    name = titleInfo->name[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo(name, wnamepart))
                        break;
                }
            }

            if (loc < TOTAL_LOCALES)
            {
                if (maxResults && counter == maxResults)
                {
                    PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                char const* knownStr = target && target->HasTitle(titleInfo) ? GetAxiumString(LANG_KNOWN) : "";

                char const* activeStr = target && target->GetUInt32Value(PLAYER_CHOSEN_TITLE) == titleInfo->bit_index
                    ? GetAxiumString(LANG_ACTIVE)
                    : "";

                char titleNameStr[80];
                snprintf(titleNameStr, 80, name.c_str(), targetName);

                // send title in "id (idx:idx) - [namedlink locale]" format
                if (m_session)
                    PSendSysMessage(LANG_TITLE_LIST_CHAT, id, titleInfo->bit_index, id, titleNameStr, localeNames[loc], knownStr, activeStr);
                else
                    PSendSysMessage(LANG_TITLE_LIST_CONSOLE, id, titleInfo->bit_index, titleNameStr, localeNames[loc], knownStr, activeStr);

                ++counter;
            }
        }
    }
    if (counter == 0)                                       // if counter == 0 then we found nth
        SendSysMessage(LANG_COMMAND_NOTITLEFOUND);
    return true;
}

bool ChatHandler::HandleCharacterTitlesCommand(const char* args)
{
    if (!*args)
        return false;

    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

    LocaleConstant loc = GetSessionDbcLocale();
    char const* targetName = target->GetName();
    char const* knownStr = GetAxiumString(LANG_KNOWN);

    // Search in CharTitles.dbc
    for (uint32 id = 0; id < sCharTitlesStore.GetNumRows(); id++)
    {
        CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(id);
        if (titleInfo && target->HasTitle(titleInfo))
        {
            std::string name = titleInfo->name[loc];
            if (name.empty())
                continue;

            char const* activeStr = target && target->GetUInt32Value(PLAYER_CHOSEN_TITLE) == titleInfo->bit_index
                ? GetAxiumString(LANG_ACTIVE)
                : "";

            char titleNameStr[80];
            snprintf(titleNameStr, 80, name.c_str(), targetName);

            // send title in "id (idx:idx) - [namedlink locale]" format
            if (m_session)
                PSendSysMessage(LANG_TITLE_LIST_CHAT, id, titleInfo->bit_index, id, titleNameStr, localeNames[loc], knownStr, activeStr);
            else
                PSendSysMessage(LANG_TITLE_LIST_CONSOLE, id, titleInfo->bit_index, name.c_str(), localeNames[loc], knownStr, activeStr);
        }
    }
    return true;
}