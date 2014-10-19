/* ScriptData
Name: gm_commandscript
%Complete: 100
Comment: All gm related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "World.h"

class gm_commandscript : public CommandScript
{
public:
    gm_commandscript() : CommandScript("gm_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand gmCommandTable[] =
        {
            { "fly",            SEC_GAMEMASTER,     false, &HandleGMFlyCommand,               "", NULL },
            { "ingame",         SEC_PLAYER,         true,  &HandleGMListIngameCommand,        "", NULL },
            { "list",           SEC_GAMEMASTER,     true,  &HandleGMListFullCommand,          "", NULL },
            { "invisible",      SEC_GAMEMASTER,     false, &HandleGMInvisibleCommand,         "", NULL },
            { "",               SEC_GAMEMASTER,     false, &HandleGMCommand,                  "", NULL },
            { NULL,             0,                  false, NULL,                              "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "gm",             SEC_GAMEMASTER,     false, NULL,                     "", gmCommandTable },
            { NULL,             0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleGMFlyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target =  handler->getSelectedPlayer();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        WorldPacket data(12);
        if (strncmp(args, "on", 3) == 0)
            data.SetOpcode(SMSG_MOVE_SET_CAN_FLY);
        else if (strncmp(args, "off", 4) == 0)
            data.SetOpcode(SMSG_MOVE_UNSET_CAN_FLY);
        else
        {
            handler->SendSysMessage(LANG_USE_BOL);
            return false;
        }
        data.append(target->GetPackGUID());
        data << uint32(0);                                      // unknown
        target->SendMessageToSet(&data, true);
        handler->PSendSysMessage(LANG_COMMAND_FLYMODE_STATUS, handler->GetNameLink(target).c_str(), args);
        return true;
    }

    static bool HandleGMListIngameCommand(ChatHandler* handler, char const* /*args*/)
    {
        bool first = true;
        bool footer = false;

        AXIUM_READ_GUARD(HashMapHolder<Player>::LockType, *HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType const& m = sObjectAccessor->GetPlayers();
        for (HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
        {
            AccountTypes itrSec = itr->second->GetSession()->GetSecurity();

            if (!handler->GetSession())
                continue;

            if (itrSec <= SEC_VIP)
                continue;

            if (first)
            {
                first = false;
                footer = true;
                handler->SendSysMessage(LANG_GMS_ON_SRV);
                handler->SendSysMessage("========================");
            }
            char const* name = itr->second->GetName();
            uint8 security = itrSec;
            uint8 max = ((16 - strlen(name)) / 2);
            uint8 max2 = max;
            if ((max + max2 + strlen(name)) == 16)
                max2 = max - 1;
            handler->PSendSysMessage("|    %s GMLevel %u", name, security);
        }
        if (footer)
            handler->SendSysMessage("========================");
        if (first)
            handler->SendSysMessage(LANG_GMS_NOT_LOGGED);
        return true;
    }

    /// Display the list of GMs
    static bool HandleGMListFullCommand(ChatHandler* handler, char const* /*args*/)
    {
        ///- Get the accounts with GM Level >1
        QueryResult result = LoginDatabase.PQuery("SELECT a.username, aa.gmlevel FROM account a, account_access aa WHERE a.id=aa.id AND aa.gmlevel > 1 AND (aa.realmid = -1 OR aa.realmid = %u)", realmID);
        if (result)
        {
            handler->SendSysMessage(LANG_GMLIST);
            handler->SendSysMessage("========================");
            ///- Cycle through them. Display username and GM level
            do
            {
                Field* fields = result->Fetch();
                char const* name = fields[0].GetCString();
                uint8 security = fields[1].GetUInt8();
                uint8 max = (16 - strlen(name)) / 2;
                uint8 max2 = max;
                if ((max + max2 + strlen(name)) == 16)
                    max2 = max - 1;
                if (handler->GetSession())
                    handler->PSendSysMessage("|    %s GMLevel %u", name, security);
                else
                    handler->PSendSysMessage("|%*s%s%*s|   %u  |", max, " ", name, max2, " ", security);
            } while (result->NextRow());
            handler->SendSysMessage("========================");
        }
        else
            handler->PSendSysMessage(LANG_GMLIST_EMPTY);
        return true;
    }

    //Enable\Disable Invisible mode
    static bool HandleGMInvisibleCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return true;

        std::string param = (char*)args;

        if (param == "on")
        {
            handler->GetSession()->GetPlayer()->SetGMInvisible(true);
            handler->PSendSysMessage("You are now invisible");
            return true;
        }

        if (param == "off")
        {
            handler->PSendSysMessage("You are now visible");
            handler->GetSession()->GetPlayer()->SetGMInvisible(false);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

    //Enable\Disable GM Mode
    static bool HandleGMCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            if (handler->GetSession()->GetPlayer()->HasGameMasterTagOn())
                handler->GetSession()->SendNotification(LANG_GM_ON);
            else
                handler->GetSession()->SendNotification(LANG_GM_OFF);
            return true;
        }

        std::string param = (char*)args;

        if (param == "on")
        {
            handler->GetSession()->GetPlayer()->SetGameMasterMode(true);
            handler->GetSession()->SendNotification(LANG_GM_ON);
            return true;
        }

        if (param == "off")
        {
            handler->GetSession()->GetPlayer()->SetGameMasterMode(false);
            handler->GetSession()->SendNotification(LANG_GM_OFF);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }
};

void AddSC_gm_commandscript()
{
    new gm_commandscript();
}
