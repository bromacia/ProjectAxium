/* ScriptData
Name: tele_commandscript
%Complete: 100
Comment: All tele related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "MapManager.h"
#include "Chat.h"
#include "Group.h"

class tele_commandscript : public CommandScript
{
public:
    tele_commandscript() : CommandScript("tele_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand teleCommandTable[] =
        {
            { "add",            SEC_GAMEMASTER,     false, &HandleTeleAddCommand,             "", NULL },
            { "del",            SEC_GAMEMASTER,     true,  &HandleTeleDelCommand,             "", NULL },
            { "name",           SEC_GAMEMASTER,     true,  &HandleTeleNameCommand,            "", NULL },
            { "group",          SEC_GAMEMASTER,     false, &HandleTeleGroupCommand,           "", NULL },
            { "",               SEC_VIP,            false, &HandleTeleCommand,                "", NULL },
            { NULL,             0,                  false, NULL,                              "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "tele",           SEC_VIP,            false, NULL,                   "", teleCommandTable },
            { NULL,             0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleTeleAddCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string name = args;

        if (sObjectMgr->GetGameTele(name))
        {
            handler->SendSysMessage(LANG_COMMAND_TP_ALREADYEXIST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameTele tele;
        tele.position_x  = player->GetPositionX();
        tele.position_y  = player->GetPositionY();
        tele.position_z  = player->GetPositionZ();
        tele.orientation = player->GetOrientation();
        tele.mapId       = player->GetMapId();
        tele.name        = name;

        if (sObjectMgr->AddGameTele(tele))
        {
            handler->SendSysMessage(LANG_COMMAND_TP_ADDED);
        }
        else
        {
            handler->SendSysMessage(LANG_COMMAND_TP_ADDEDERR);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleTeleDelCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string name = args;

        if (!sObjectMgr->DeleteGameTele(name))
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->SendSysMessage(LANG_COMMAND_TP_DELETED);
        return true;
    }

    // teleport player to given game_tele.entry
    static bool HandleTeleNameCommand(ChatHandler* handler, const char* args)
    {
        char* nameStr;
        char* teleStr;
        handler->extractOptFirstArg((char*)args, &nameStr, &teleStr);
        if (!teleStr)
            return false;

        Player* target;
        uint64 target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget(nameStr, &target, &target_guid, &target_name))
            return false;

        if (strcmp(teleStr, "$home") == 0)    // References target's homebind
        {
            if (target)
                target->TeleportTo(target->m_homebindMapId, target->m_homebindX, target->m_homebindY, target->m_homebindZ, target->GetOrientation());
            else
            {
                QueryResult resultDB = CharacterDatabase.PQuery("SELECT mapId, zoneId, posX, posY, posZ FROM character_homebind WHERE guid = %u", target_guid);
                if (resultDB)
                {
                    Field* fieldsDB = resultDB->Fetch();
                    uint32 mapId = fieldsDB[0].GetUInt32();
                    uint32 zoneId = fieldsDB[1].GetUInt32();
                    float posX = fieldsDB[2].GetFloat();
                    float posY = fieldsDB[3].GetFloat();
                    float posZ = fieldsDB[4].GetFloat();

                    Player::SavePositionInDB(mapId, posX, posY, posZ, 0, zoneId, target_guid);
                }
            }

            return true;
        }

        // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
        GameTele const* tele = handler->extractGameTeleFromLink(teleStr);
        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target)
        {
            // check online security
            if (handler->HasLowerSecurity(target, 0))
                return false;

            std::string chrNameLink = handler->playerLink(target_name);

            if (target->IsBeingTeleported() == true)
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, chrNameLink.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            // stop flight if need
            if (target->isInFlight())
            {
                target->GetMotionMaster()->MovementExpired();
                target->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                target->SaveRecallPosition();

            target->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
        }
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(NULL, target_guid))
                return false;

            std::string nameLink = handler->playerLink(target_name);

            handler->PSendSysMessage(LANG_TELEPORTING_TO, nameLink.c_str(), handler->GetAxiumString(LANG_OFFLINE), tele->name.c_str());
            Player::SavePositionInDB(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation,
                sMapMgr->GetZoneId(tele->mapId, tele->position_x, tele->position_y, tele->position_z), target_guid);
        }

        return true;
    }

    //Teleport group to given game_tele.entry
    static bool HandleTeleGroupCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, 0))
            return false;

        // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
        GameTele const* tele = handler->extractGameTeleFromLink((char*)args);
        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        MapEntry const* map = sMapStore.LookupEntry(tele->mapId);
        if (!map || map->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_CANNOT_TELE_TO_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string nameLink = handler->GetNameLink(target);

        Group* grp = target->GetGroup();
        if (!grp)
        {
            handler->PSendSysMessage(LANG_NOT_IN_GROUP, nameLink.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* player = itr->getSource();

            if (!player || !player->GetSession())
                continue;

            // check online security
            if (handler->HasLowerSecurity(player, 0))
                return false;

            std::string plNameLink = handler->GetNameLink(player);

            if (player->IsBeingTeleported())
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, plNameLink.c_str());
                continue;
            }

            handler->PSendSysMessage(LANG_TELEPORTING_TO, plNameLink.c_str(), "", tele->name.c_str());
            if (handler->needReportToTarget(player))
                player->SendSysMessage(LANG_TELEPORTED_TO_BY, nameLink.c_str());

            // stop flight if need
            if (player->isInFlight())
            {
                player->GetMotionMaster()->MovementExpired();
                player->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                player->SaveRecallPosition();

            player->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
        }

        return true;
    }

    static bool HandleTeleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
        const GameTele *tele = handler->extractGameTeleFromLink((char*)args);

        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (IsPhraseInString(tele->name, "axium_") && handler->GetSession()->GetSecurity() < SEC_GAMEMASTER)
            return false;

        if (player->CanTeleportTo(tele))
        {
            if (player->isInFlight())
            {
                player->GetMotionMaster()->MovementExpired();
                player->CleanupAfterTaxiFlight();
            }
            else
                player->SaveRecallPosition();

            player->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
            player->lastTeleportTime = getMSTime();
        }

        return true;
    }
};

void AddSC_tele_commandscript()
{
    new tele_commandscript();
}
