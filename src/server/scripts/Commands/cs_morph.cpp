#include "ScriptPCH.h"
#include "Chat.h"
#include "Player.h"

enum MorphIds
{
    HUM      = 19723,
    HUF      = 19724,
    DWM      = 20317,
    NEM      = 20318,
    GNM      = 20580,
    GNF      = 20581,
    DRF      = 20323,
    ORM      = 21267,
    ORF      = 20316,
    TAM      = 20585,
    TAF      = 20584,
    TRM      = 20321,
    BEM      = 20578,
    BEF      = 20579,
    GOM      = 20582,
    GOF      = 20583,
    NOG      = 7550,
    PIM      = 4620,
    PIF      = 4619,
    NIM      = 4617,
    NIF      = 4618
};

class morph_commandscript : public CommandScript
{
    public:
        morph_commandscript() : CommandScript("morph_commandscript") { }

        ChatCommand* GetCommands() const
        {
            static ChatCommand morphCommandTable[] =
            {
                { "hum",        SEC_VIP,            false, &HandleMorphHumanMaleCommand,        "", NULL },
                { "huf",        SEC_VIP,            false, &HandleMorphHumanFemaleCommand,      "", NULL },
                { "dwm",        SEC_VIP,            false, &HandleMorphDwarfMaleCommand,        "", NULL },
                { "nem",        SEC_VIP,            false, &HandleMorphNightElfMaleCommand,     "", NULL },
                { "gnm",        SEC_VIP,            false, &HandleMorphGnomeMaleCommand,        "", NULL },
                { "gnf",        SEC_VIP,            false, &HandleMorphGnomeFemaleCommand,      "", NULL },
                { "drf",        SEC_VIP,            false, &HandleMorphDraeneiFemaleCommand,    "", NULL },
                { "orm",        SEC_VIP,            false, &HandleMorphFelOrcMaleCommand,       "", NULL },
                { "orf",        SEC_VIP,            false, &HandleMorphOrcFemaleCommand,        "", NULL },
                { "tam",        SEC_VIP,            false, &HandleMorphTaurenMaleCommand,       "", NULL },
                { "taf",        SEC_VIP,            false, &HandleMorphTaurenFemaleCommand,     "", NULL },
                { "trm",        SEC_VIP,            false, &HandleMorphTrollMaleCommand,        "", NULL },
                { "bem",        SEC_VIP,            false, &HandleMorphBloodElfMaleCommand,     "", NULL },
                { "bef",        SEC_VIP,            false, &HandleMorphBloodElfFemaleCommand,   "", NULL },
                { "gom",        SEC_VIP,            false, &HandleMorphGoblinMaleCommand,       "", NULL },
                { "gof",        SEC_VIP,            false, &HandleMorphGoblinFemaleCommand,     "", NULL },
                { "nog",        SEC_PLAYER,         false, &HandleMorphNoggenfoggerCommand,     "", NULL },
                { "pim",        SEC_PLAYER,         false, &HandleMorphPirateMaleCommand,       "", NULL },
                { "pif",        SEC_PLAYER,         false, &HandleMorphPirateFemaleCommand,     "", NULL },
                { "nim",        SEC_PLAYER,         false, &HandleMorphNinjaMaleCommand,        "", NULL },
                { "nif",        SEC_PLAYER,         false, &HandleMorphNinjaFemaleCommand,      "", NULL },
                { NULL,         0,                  false, NULL,                                "", NULL }
            };

            static ChatCommand commandTable[] =
            {
                { "morph",      SEC_PLAYER,         false, NULL,                   "", morphCommandTable },
                { "demorph",    SEC_PLAYER,         false, &HandleDemorphCommand,               "", NULL },
                { NULL,         0,                  false, NULL,                                "", NULL }
            };

            return commandTable;
        }

        static bool HandleMorphHumanMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(HUM);
            return true;
        }

        static bool HandleMorphHumanFemaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(HUF);
            return true;
        }

        static bool HandleMorphDwarfMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(DWM);
            return true;
        }

        static bool HandleMorphNightElfMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(NEM);
            return true;
        }

        static bool HandleMorphGnomeMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(GNM);
            return true;
        }

        static bool HandleMorphGnomeFemaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(GNF);
            return true;
        }

        static bool HandleMorphDraeneiFemaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(DRF);
            return true;
        }

        static bool HandleMorphFelOrcMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(ORM);
            return true;
        }

        static bool HandleMorphOrcFemaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(ORF);
            return true;
        }

        static bool HandleMorphTaurenMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(TAM);
            return true;
        }

        static bool HandleMorphTaurenFemaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(TAF);
            return true;
        }

        static bool HandleMorphTrollMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(TRM);
            return true;
        }

        static bool HandleMorphBloodElfMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(BEM);
            return true;
        }

        static bool HandleMorphBloodElfFemaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(BEF);
            return true;
        }

        static bool HandleMorphGoblinMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(GOM);
            return true;
        }

        static bool HandleMorphGoblinFemaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(GOF);
            return true;
        }

        static bool HandleMorphNoggenfoggerCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(NOG);
            return true;
        }

        static bool HandleMorphPirateMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(PIM);
            return true;
        }

        static bool HandleMorphPirateFemaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(PIF);
            return true;
        }

        static bool HandleMorphNinjaMaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(NIM);
            return true;
        }

        static bool HandleMorphNinjaFemaleCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            player->Morph(NIF);
            return true;
        }

        static bool HandleDemorphCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();

            if (player->GetSession()->GetSecurity() >= SEC_GAMEMASTER)
            {
                Unit* target = handler->getSelectedUnit();
                if (!target)
                    target = player;
                else if (target->GetTypeId() == TYPEID_PLAYER && handler->HasLowerSecurity((Player*)target, 0))
                    return false;

                if (Player* pTarget = target->ToPlayer())
                    pTarget->Demorph(true);
                else
                    target->SetDisplayId(target->GetNativeDisplayId());

                return true;
            }

            player->Demorph();
            return true;
        }
};

void AddSC_morph_commandscript()
{
    new morph_commandscript();
}