#include "ScriptPCH.h"
#include "Chat.h"
#include "Player.h"

class modeloverride_commandscript : public CommandScript
{
    public:
        modeloverride_commandscript() : CommandScript("modeloverride_commandscript") { }

        ChatCommand* GetCommands() const
        {
            static ChatCommand modeloverrideCommandTable[] =
            {
                { "race",           SEC_VIP,            false,  &HandleRaceCommand,             "",     NULL },
                { "gender",         SEC_VIP,            false,  &HandleGenderCommand,           "",     NULL },
                { "skin",           SEC_VIP,            false,  &HandleSkinCommand,             "",     NULL },
                { "face",           SEC_VIP,            false,  &HandleFaceCommand,             "",     NULL },
                { "hairstyle",      SEC_VIP,            false,  &HandleHairStyleCommand,        "",     NULL },
                { "haircolor",      SEC_VIP,            false,  &HandleHairColorCommand,        "",     NULL },
                { "facialfeature",  SEC_VIP,            false,  &HandleFacialFeatureCommand,    "",     NULL },
                { "restore",        SEC_VIP,            false,  &HandleRestoreCommand,          "",     NULL },
                { "copy",           SEC_GAMEMASTER,     false,  &HandleCopyCommand,             "",     NULL },
                { "send",           SEC_GAMEMASTER,     false,  &HandleSendCommand,             "",     NULL },
                { NULL,             0,                  false,  NULL,                           "",     NULL }
            };

            static ChatCommand commandTable[] =
            {
                { "modeloverride",  SEC_VIP,    false,  NULL,   "",     modeloverrideCommandTable },
                { NULL,             0,          false,  NULL,   "",                          NULL }
            };

            return commandTable;
        }

        static bool HandleRaceCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            if (!*args)
                return false;

            uint8 race = (uint8)atoi((char*)args);
            if (!((1 << (race - 1)) & RACEMASK_ALL_PLAYABLE))
                return false;

            ModelOverride* mo = player->GetModelOverride();
            mo->SetRace(race);
            return true;
        }

        static bool HandleGenderCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            if (!*args)
                return false;

            uint8 gender = (uint8)atoi((char*)args);
            if (gender >= GENDER_NONE)
                return false;

            ModelOverride* mo = player->GetModelOverride();
            mo->SetGender(gender);
            return true;
        }

        static bool HandleSkinCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            if (!*args)
                return false;

            uint8 skin = (uint8)atoi((char*)args);

            ModelOverride* mo = player->GetModelOverride();
            uint8 rgp = ((mo->GetRace() << 4) | mo->GetGender());
            if (skin > RaceGenderLimits.at(rgp).Skin)
                return false;

            mo->SetSkin(skin);
            return true;
        }

        static bool HandleFaceCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            if (!*args)
                return false;

            uint8 face = (uint8)atoi((char*)args);

            ModelOverride* mo = player->GetModelOverride();
            uint8 rgp = ((mo->GetRace() << 4) | mo->GetGender());
            if (face > RaceGenderLimits.at(rgp).Face)
                return false;

            mo->SetFace(face);
            return true;
        }

        static bool HandleHairStyleCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            if (!*args)
                return false;

            uint8 hairStyle = (uint8)atoi((char*)args);

            ModelOverride* mo = player->GetModelOverride();
            uint8 rgp = ((mo->GetRace() << 4) | mo->GetGender());
            if (hairStyle > RaceGenderLimits.at(rgp).HairStyle)
                return false;

            mo->SetHairStyle(hairStyle);
            return true;
        }

        static bool HandleHairColorCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            if (!*args)
                return false;

            uint8 hairColor = (uint8)atoi((char*)args);

            ModelOverride* mo = player->GetModelOverride();
            uint8 rgp = ((mo->GetRace() << 4) | mo->GetGender());
            if (hairColor > RaceGenderLimits.at(rgp).HairColor)
                return false;

            mo->SetHairColor(hairColor);
            return true;
        }

        static bool HandleFacialFeatureCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            if (!*args)
                return false;

            uint8 facialFeature = (uint8)atoi((char*)args);

            ModelOverride* mo = player->GetModelOverride();
            uint8 rgp = ((mo->GetRace() << 4) | mo->GetGender());
            if (facialFeature > RaceGenderLimits.at(rgp).FacialFeature)
                return false;

            mo->SetFacialFeature(facialFeature);
            return true;
        }

        static bool HandleRestoreCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            player->GetModelOverride()->Restore();
            return true;
        }

        static bool HandleCopyCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            Player* target = handler->getSelectedPlayer();
            if (!target)
                return false;

            ModelOverride* pmo = player->GetModelOverride();
            ModelOverride* tmo = target->GetModelOverride();
            pmo->SetRace(tmo->GetRace());
            pmo->SetGender(tmo->GetGender());
            pmo->SetSkin(tmo->GetSkin());
            pmo->SetFace(tmo->GetFace());
            pmo->SetHairStyle(tmo->GetHairStyle());
            pmo->SetHairColor(tmo->GetHairColor());
            pmo->SetFacialFeature(tmo->GetFacialFeature());
            return true;
        }

        static bool HandleSendCommand(ChatHandler* handler, const char* args)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            Player* target = handler->getSelectedPlayer();
            if (!target)
                return false;

            ModelOverride* pmo = player->GetModelOverride();
            ModelOverride* tmo = target->GetModelOverride();
            tmo->SetRace(pmo->GetRace());
            tmo->SetGender(pmo->GetGender());
            tmo->SetSkin(pmo->GetSkin());
            tmo->SetFace(pmo->GetFace());
            tmo->SetHairStyle(pmo->GetHairStyle());
            tmo->SetHairColor(pmo->GetHairColor());
            tmo->SetFacialFeature(pmo->GetFacialFeature());
            return true;
        }
};

void AddSC_modeloverride_commandscript()
{
    new modeloverride_commandscript();
}
