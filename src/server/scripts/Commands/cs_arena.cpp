#include "ScriptPCH.h"
#include "ArenaTeamMgr.h"
#include "PvPMgr.h"

class arena_commandscript : public CommandScript
{
    public:
        arena_commandscript() : CommandScript("arena_commandscript") {}

        ChatCommand* GetCommands() const
        {
            static ChatCommand arenaCommandTable[] =
            {
                { "mmr",                SEC_PLAYER,         false, &HandleMMRCommand,                       "", NULL },
                { "cap",                SEC_PLAYER,         false, &HandleCapCommand,                       "", NULL },
                { "createteam",         SEC_PLAYER,         false, &HandleCreateTeamCommand,                "", NULL },
                { "queue",              SEC_PLAYER,         false, &HandleQueueCommand,                     "", NULL },
                { "resetweeklycap",     SEC_GAMEMASTER,     false, &HandleResetWeeklyCapCommand,            "", NULL },
                { NULL,                 0,                  false, NULL,                                    "", NULL }
            };
            static ChatCommand commandTable[] =
            {
                { "arena",              SEC_PLAYER,         false, NULL,                       "", arenaCommandTable },
                { NULL,                 0,                  false, NULL,                                    "", NULL }
            };
            return commandTable;
        }

        static bool HandleMMRCommand(ChatHandler* handler, const char* args)
        {
            uint64 targetGUID;
            std::string targetName;

            uint32 parseGUID = MAKE_NEW_GUID(atol((char*)args), 0, HIGHGUID_PLAYER);

            if (sObjectMgr->GetPlayerNameByGUID(parseGUID, targetName))
                targetGUID = parseGUID;
            else if (!handler->extractPlayerTarget((char*)args, 0, &targetGUID, &targetName))
                return false;

            handler->PSendSysMessage("Player: %s", targetName.c_str());
            handler->PSendSysMessage("2v2 MMR: %u", sPvPMgr->Get2v2MMR(targetGUID));
            handler->PSendSysMessage("3v3 MMR: %u", sPvPMgr->Get3v3MMR(targetGUID));
            handler->PSendSysMessage("5v5 MMR: %u", sPvPMgr->Get5v5MMR(targetGUID));
            return true;
        }

        static bool HandleCapCommand(ChatHandler* handler, const char* args)
        {
            uint64 targetGUID;
            std::string targetName;

            uint32 parseGUID = MAKE_NEW_GUID(atol((char*)args), 0, HIGHGUID_PLAYER);

            if (sObjectMgr->GetPlayerNameByGUID(parseGUID, targetName))
                targetGUID = parseGUID;
            else if (!handler->extractPlayerTarget((char*)args, 0, &targetGUID, &targetName))
                return false;

            handler->PSendSysMessage("Player: %s", targetName.c_str());

            uint32 arenaPointsCap = sPvPMgr->GetArenaPointsCap(targetGUID);
            if (arenaPointsCap)
                handler->PSendSysMessage("Arena Points earned this week: %u", sPvPMgr->GetWeeklyArenaPoints(targetGUID));
            else
            {
                arenaPointsCap = sPvPMgr->CalculateArenaPointsCap(targetGUID);

                if (arenaPointsCap < sWorld->getIntConfig(CONFIG_ARENA_POINTS_CAP_MINIMUM_CAP))
                    arenaPointsCap = sWorld->getIntConfig(CONFIG_ARENA_POINTS_CAP_MINIMUM_CAP);

                sPvPMgr->SetArenaPointsCap(targetGUID, arenaPointsCap);
                handler->PSendSysMessage("Arena Points earned this week: 0");
            }

            handler->PSendSysMessage("Arena Points Cap this week: %u", arenaPointsCap);
            return true;
        }

        static bool HandleCreateTeamCommand(ChatHandler* handler, const char* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            WorldPacket data(SMSG_PETITION_SHOWLIST, 81);
            data << player->GetGUID();
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
            return true;
        }

        static bool HandleQueueCommand(ChatHandler* handler, const char* /*args*/)
        {
            handler->GetSession()->SendBattlegGroundList(handler->GetSession()->GetPlayer()->GetGUID(), BATTLEGROUND_AA);
            return true;
        }

        static bool HandleResetWeeklyCapCommand(ChatHandler* handler, const char* /*args*/)
        {
            sArenaTeamMgr->ResetWeeklyCap();
            return true;
        }
};

void AddSC_arena_commandscript()
{
    new arena_commandscript();
}
