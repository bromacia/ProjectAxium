#include "ScriptPCH.h"
#include "Chat.h"
#include "Player.h"

class morph_commandscript : public CommandScript
{
    public:
        morph_commandscript() : CommandScript("morph_commandscript") { }

        ChatCommand* GetCommands() const
        {
            static ChatCommand commandTable[] =
            {
                { "morph",      SEC_GAMEMASTER,         false, &HandleMorphCommand,                 "", NULL },
                { "demorph",    SEC_GAMEMASTER,         false, &HandleDemorphCommand,               "", NULL },
                { NULL,         0,                      false, NULL,                                "", NULL }
            };

            return commandTable;
        }

        static bool HandleMorphCommand(ChatHandler* handler, const char* args)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target)
                target = handler->GetSession()->GetPlayer();
            else if (target->GetTypeId() == TYPEID_PLAYER && handler->HasLowerSecurity(target->ToPlayer(), 0))
                return false;

            if (!target)
                return false;

            if (!*args)
                return false;

            uint16 display_id = (uint16)atoi((char*)args);

            target->SetDisplayId(display_id);
            return true;
        }

        static bool HandleDemorphCommand(ChatHandler* handler, const char* /*args*/)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target)
                target = handler->GetSession()->GetPlayer();
            else if (target->GetTypeId() == TYPEID_PLAYER && handler->HasLowerSecurity(target->ToPlayer(), 0))
                return false;

            if (!target)
                return false;

            target->SetDisplayId(target->GetNativeDisplayId());
            return true;
        }
};

void AddSC_morph_commandscript()
{
    new morph_commandscript();
}