#include "Battleground.h"
#include "Chat.h"
#include "Player.h"

class spectate_commandscript : public CommandScript
{
public:
    spectate_commandscript() : CommandScript("spectate_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand pvpCommandTable[] =
        {
            { "setviewpoint",       SEC_PLAYER,         false, &HandleSetViewpointCommand,              "", NULL },
            { "restoreviewpoint",   SEC_PLAYER,         false, &HandleRestoreViewpointCommand,          "", NULL },
            { NULL,                 0,                  false, NULL,                                    "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "spectate",           SEC_PLAYER,         false, NULL,                         "", pvpCommandTable },
            { NULL,                 0,                  false, NULL,                                    "", NULL }
        };
        return commandTable;
    }

    static bool HandleSetViewpointCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->PSendSysMessage("You have no player selected.");
            return false;
        }

        if (target == player)
        {
            handler->PSendSysMessage("You can't spectate yourself.");
            return false;
        }

        if (!player->IsArenaSpectator())
        {
            handler->PSendSysMessage("You can't spectate unless you're flagged as a spectator.");
            return false;
        }

        Battleground* bg = target->GetBattleground();
        if (!bg)
        {
            handler->PSendSysMessage("Your target isn't in a battleground/arena.");
            return false;
        }

        if (bg != player->GetBattleground())
        {
            handler->PSendSysMessage("Your target isn't in the same battleground/arena as you.");
            return false;
        }

        if (!bg->isArena())
        {
            handler->PSendSysMessage("Your target isn't in an arena.");
            return false;
        }

        player->SetViewpoint(target, true);
        return true;
    }

    static bool HandleRestoreViewpointCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        WorldObject* viewpoint = player->GetCurrentViewpoint();
        if (!viewpoint)
        {
            handler->PSendSysMessage("There is no viewpoint to restore.");
            return false;
        }

        Player* pViewpoint = viewpoint->ToPlayer();
        if (!pViewpoint)
        {
            handler->PSendSysMessage("Your viewpoint isn't a valid player.");
            return false;
        }

        if (pViewpoint == player)
        {
            handler->PSendSysMessage("Your viewpoint is already you.");
            return false;
        }

        if (!player->IsArenaSpectator())
        {
            handler->PSendSysMessage("You can't restore your viewpoint unless you're flagged as a spectator.");
            return false;
        }

        Battleground* bg = pViewpoint->GetBattleground();
        if (!bg)
        {
            sLog->outError("HandleRestoreViewpointCommand:: Viewpoint: ([%u]%s) for player [%u]%s isn't in a battleground..",
            pViewpoint->GetGUIDLow(), pViewpoint->GetName(), player->GetGUIDLow(), player->GetName());
            handler->SetSentErrorMessage(false);
            return false;
        }

        if (bg != player->GetBattleground())
        {
            sLog->outError("HandleRestoreViewpointCommand:: Viewpoint: ([%u]%s) for player [%u]%s is in a different battleground.",
            pViewpoint->GetGUIDLow(), pViewpoint->GetName(), player->GetGUIDLow(), player->GetName());
            handler->SetSentErrorMessage(false);
            return false;
        }

        player->SetViewpoint(viewpoint, false);
        return true;
    }
};

void AddSC_spectate_commandscript()
{
    new spectate_commandscript();
}
