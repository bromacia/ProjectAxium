#include "ScriptPCH.h"
#include "Chat.h"

class misc_commandscript : public CommandScript
{
public:
    misc_commandscript() : CommandScript("misc_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand commandTable[] =
        {
            { "dev",            SEC_ADMINISTRATOR,  false,  &HandleDevCommand,          "", NULL },
            { NULL,             0,                  false,  NULL,                       "", NULL }
        };
        return commandTable;
    }

    static bool HandleDevCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            if (handler->GetSession()->GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DEVELOPER))
                handler->GetSession()->SendNotification(LANG_DEV_ON);
            else
                handler->GetSession()->SendNotification(LANG_DEV_OFF);
            return true;
        }

        std::string argstr = (char*)args;

        if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetDeveloperMode(true);
            handler->GetSession()->SendNotification(LANG_DEV_ON);
#ifdef _DEBUG_VMAPS
            VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
            vMapManager->processCommand("stoplog");
#endif
            return true;
        }

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetDeveloperMode(false);
            handler->GetSession()->SendNotification(LANG_DEV_OFF);
#ifdef _DEBUG_VMAPS
            VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
            vMapManager->processCommand("startlog");
#endif
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }
};

void AddSC_misc_commandscript()
{
    new misc_commandscript();
}
