#include "ScriptMgr.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "ObjectMgr.h"
#include "TicketMgr.h"

class ticket_commandscript : public CommandScript
{
public:
    ticket_commandscript() : CommandScript("ticket_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand ticketResponseCommandTable[] =
        {
            { "append",         SEC_GAMEMASTER,     true,  &HandleGMTicketResponseAppendCommand,    "", NULL },
            { "appendln",       SEC_GAMEMASTER,     true,  &HandleGMTicketResponseAppendLnCommand,  "", NULL },
            { NULL,             0,                  false, NULL,                                    "", NULL }
        };

        static ChatCommand ticketCommandTable[] =
        {
            { "assign",         SEC_GAMEMASTER,     true,  &HandleGMTicketAssignToCommand,          "", NULL },
            { "close",          SEC_GAMEMASTER,     true,  &HandleGMTicketCloseByIdCommand,         "", NULL },
            { "closedlist",     SEC_GAMEMASTER,     true,  &HandleGMTicketListClosedCommand,        "", NULL },
            { "comment",        SEC_GAMEMASTER,     true,  &HandleGMTicketCommentCommand,           "", NULL },
            { "complete",       SEC_GAMEMASTER,     true,  &HandleGMTicketCompleteCommand,          "", NULL },
            { "delete",         SEC_GAMEMASTER,     true,  &HandleGMTicketDeleteByIdCommand,        "", NULL },
            { "escalate",       SEC_GAMEMASTER,     true,  &HandleGMTicketEscalateCommand,          "", NULL },
            { "escalatedlist",  SEC_GAMEMASTER,     true,  &HandleGMTicketListEscalatedCommand,     "", NULL },
            { "list",           SEC_GAMEMASTER,     true,  &HandleGMTicketListCommand,              "", NULL },
            { "onlinelist",     SEC_GAMEMASTER,     true,  &HandleGMTicketListOnlineCommand,        "", NULL },
            { "reset",          SEC_ADMINISTRATOR,  true,  &HandleGMTicketResetCommand,             "", NULL },
            { "togglesystem",   SEC_ADMINISTRATOR,  true,  &HandleToggleGMTicketSystem,             "", NULL },
            { "unassign",       SEC_GAMEMASTER,     true,  &HandleGMTicketUnAssignCommand,          "", NULL },
            { "viewid",         SEC_GAMEMASTER,     true,  &HandleGMTicketGetByIdCommand,           "", NULL },
            { "viewname",       SEC_GAMEMASTER,     true,  &HandleGMTicketGetByNameCommand,         "", NULL },
            { "response",       SEC_GAMEMASTER,     true,  NULL,              "", ticketResponseCommandTable },
            { NULL,             0,                  false, NULL,                                    "", NULL }
        };

        static ChatCommand commandTable[] =
        {
            { "ticket",         SEC_GAMEMASTER,     false, NULL,                      "", ticketCommandTable },
            { NULL,             0,                  false, NULL,                                    "", NULL }
        };

        return commandTable;
    }

    static bool HandleGMTicketAssignToCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* ticketIdStr = strtok((char*)args, " ");
        uint32 ticketId = atoi(ticketIdStr);

        char* targetStr = strtok(NULL, " ");
        if (!targetStr)
            return false;

        std::string target(targetStr);
        if (!normalizePlayerName(target))
            return false;

        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket || ticket->IsClosed())
        {
            handler->SendSysMessage("Ticket not found.");
            return true;
        }

        // Get target information
        uint64 targetGuid = sObjectMgr->GetPlayerGUIDByName(target.c_str());
        uint64 targetAccountId = sObjectMgr->GetPlayerAccountIdByGUID(targetGuid);
        uint32 targetGmLevel = AccountMgr::GetSecurity(targetAccountId, realmID);

        // Target must exist and have administrative rights
        if (!targetGuid || AccountMgr::IsPlayerAccount(targetGmLevel))
        {
            handler->SendSysMessage("Invalid name specified. Name should be that of an online Gamemaster.");
            return true;
        }

        // If already assigned, leave
        if (ticket->IsAssignedTo(targetGuid))
        {
            handler->PSendSysMessage("This ticket is already assigned to yourself. To unassign use .ticket unassign %d and then reassign.", ticket->GetId());
            return true;
        }

        // If assigned to different player other than current, leave
        //! Console can override though
        Player* player = handler->GetSession() ? handler->GetSession()->GetPlayer() : NULL;
        if (player && ticket->IsAssignedNotTo(player->GetGUID()))
        {
            handler->PSendSysMessage("Ticket(%d) is already assigned.", ticket->GetId(), target.c_str());
            return true;
        }

        // Assign ticket
        SQLTransaction trans = SQLTransaction(NULL);
        ticket->SetAssignedTo(targetGuid, targetGmLevel == SEC_ADMINISTRATOR ? true : false);
        ticket->SaveToDB(trans);
        sTicketMgr->UpdateLastChange();
        std::string msg = ticket->FormatMessageString(*handler, NULL, NULL, NULL, target.c_str(), NULL);
        handler->SendGlobalGMSysMessage(msg.c_str());
        return true;
    }

    static bool HandleGMTicketCloseByIdCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 ticketId = atoi(args);
        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket || ticket->IsClosed() || ticket->IsCompleted())
        {
            handler->SendSysMessage("Ticket not found.");
            return true;
        }

        // Ticket should be assigned to the player who tries to close it.
        // Console can override though
        Player* player = handler->GetSession() ? handler->GetSession()->GetPlayer() : NULL;
        if (player && ticket->IsAssignedNotTo(player->GetGUID()))
        {
            handler->PSendSysMessage("Cannot close Ticket(%d). It's assigned to another GM.", ticket->GetId());
            return true;
        }

        sTicketMgr->CloseTicket(ticket->GetId(), player ? player->GetGUID() : -1);
        sTicketMgr->UpdateLastChange();

        std::string msg = ticket->FormatMessageString(*handler, NULL, player ? player->GetName() : "Console", NULL, NULL, NULL);
        handler->SendGlobalGMSysMessage(msg.c_str());

        // Inform player, who submitted this ticket, that it is closed
        if (Player* submitter = ticket->GetPlayer())
        {
            if (submitter->IsInWorld())
            {
                WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
                data << uint32(GMTICKET_RESPONSE_TICKET_DELETED);
                submitter->GetSession()->SendPacket(&data);
            }
        }

        return true;
    }

    static bool HandleGMTicketCommentCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* ticketIdStr = strtok((char*)args, " ");
        uint32 ticketId = atoi(ticketIdStr);

        char* comment = strtok(NULL, "\n");
        if (!comment)
            return false;

        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket || ticket->IsClosed())
        {
            handler->PSendSysMessage("Ticket not found.");
            return true;
        }

        // Cannot comment ticket assigned to someone else
        //! Console excluded
        Player* player = handler->GetSession() ? handler->GetSession()->GetPlayer() : NULL;
        if (ticket->IsAssignedNotTo(player->GetGUID()))
        {
            handler->PSendSysMessage("Ticket(%d) is already assigned.", ticket->GetId());
            return true;
        }

        SQLTransaction trans = SQLTransaction(NULL);
        ticket->SetComment(comment);
        ticket->SaveToDB(trans);
        sTicketMgr->UpdateLastChange();

        std::stringstream msg;
        msg << ticket->FormatMessageString(*handler, NULL, NULL, NULL, ticket->GetAssignedToName().c_str(), NULL);
        msg << (player ? player->GetName() : "Console") << " |cff00ff00Added comment|r: [" << comment << "]|r";
        handler->SendGlobalGMSysMessage(msg.str().c_str());
        return true;
    }

    static bool HandleGMTicketListClosedCommand(ChatHandler* handler, char const* /*args*/)
    {
        sTicketMgr->ShowClosedList(*handler);
        return true;
    }

    static bool HandleGMTicketCompleteCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 ticketId = atoi(args);
        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket || ticket->IsClosed() || ticket->IsCompleted())
        {
            handler->SendSysMessage("Ticket not found.");
            return true;
        }

        Player* ticketCreator = ticket->GetPlayer();
        if (!ticketCreator || !ticketCreator->IsInWorld())
        {
            handler->PSendSysMessage("The creator of Ticket(%d) is not online.", ticket->GetId());
            return false;
        }

        ticket->SendResponse(ticketCreator->GetSession());

        Player* player = handler->GetSession() ? handler->GetSession()->GetPlayer() : NULL;
        SQLTransaction trans = SQLTransaction(NULL);
        ticket->SetCompleted();
        ticket->SaveToDB(trans);
        sTicketMgr->UpdateLastChange();
        std::string msg = ticket->FormatMessageString(*handler, player ? player->GetName() : "Console", NULL, NULL, NULL, NULL);
        handler->SendGlobalGMSysMessage(msg.c_str());
        return true;
    }

    static bool HandleGMTicketDeleteByIdCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 ticketId = atoi(args);
        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket)
        {
            handler->SendSysMessage("Ticket not found.");
            return true;
        }

        std::string msg = ticket->FormatMessageString(*handler, NULL, NULL, handler->GetSession() ? handler->GetSession()->GetPlayer()->GetName() : "Console", NULL, NULL);
        handler->SendGlobalGMSysMessage(msg.c_str());

        sTicketMgr->RemoveTicket(ticket->GetId());
        sTicketMgr->UpdateLastChange();

        if (Player* player = ticket->GetPlayer())
        {
            if (player->IsInWorld())
            {
                // Force abandon ticket
                WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
                data << uint32(GMTICKET_RESPONSE_TICKET_DELETED);
                player->GetSession()->SendPacket(&data);
            }
        }

        return true;
    }

    static bool HandleGMTicketEscalateCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 ticketId = atoi(args);
        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket || ticket->IsClosed() || ticket->IsCompleted() || ticket->GetEscalatedStatus() != TICKET_UNASSIGNED)
        {
            handler->SendSysMessage("Ticket not found.");
            return true;
        }

        ticket->SetEscalatedStatus(TICKET_IN_ESCALATION_QUEUE);

        if (Player* player = ticket->GetPlayer())
            if (player->IsInWorld())
                sTicketMgr->SendTicket(player->GetSession(), ticket);

        sTicketMgr->UpdateLastChange();
        return true;
    }

    static bool HandleGMTicketListEscalatedCommand(ChatHandler* handler, char const* /*args*/)
    {
        sTicketMgr->ShowEscalatedList(*handler);
        return true;
    }

    static bool HandleGMTicketListCommand(ChatHandler* handler, char const* /*args*/)
    {
        sTicketMgr->ShowList(*handler, false);
        return true;
    }

    static bool HandleGMTicketListOnlineCommand(ChatHandler* handler, char const* /*args*/)
    {
        sTicketMgr->ShowList(*handler, true);
        return true;
    }

    static bool HandleGMTicketResetCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (sTicketMgr->GetOpenTicketCount())
        {
            handler->SendSysMessage("There are pending open tickets, please close them first!");
            return true;
        }
        else
        {
            sTicketMgr->ResetTickets();
            handler->SendSysMessage("All closed tickets were deleted and the ticket counter is reset to |cffff00ff 1|r");
        }

        return true;
    }

    static bool HandleToggleGMTicketSystem(ChatHandler* handler, char const* /*args*/)
    {
        bool status = !sTicketMgr->GetStatus();
        sTicketMgr->SetStatus(status);
        handler->PSendSysMessage(status ? LANG_ALLOW_TICKETS : LANG_DISALLOW_TICKETS);
        return true;
    }

    static bool HandleGMTicketUnAssignCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 ticketId = atoi(args);
        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket || ticket->IsClosed())
        {
            handler->SendSysMessage("Ticket not found.");
            return true;
        }

        // Ticket must be assigned
        if (!ticket->IsAssigned())
        {
            handler->PSendSysMessage("Ticket(%d) is not assigned, you cannot unassign it.", ticket->GetId());
            return true;
        }

        // Get security level of player, whom this ticket is assigned to
        uint32 security = SEC_PLAYER;
        Player* assignedPlayer = ticket->GetAssignedPlayer();
        if (assignedPlayer && assignedPlayer->IsInWorld())
            security = assignedPlayer->GetSession()->GetSecurity();
        else
        {
            uint64 guid = ticket->GetAssignedToGUID();
            uint32 accountId = sObjectMgr->GetPlayerAccountIdByGUID(guid);
            security = AccountMgr::GetSecurity(accountId, realmID);
        }

        // Check security
        //! If no m_session present it means we're issuing this command from the console
        uint32 mySecurity = handler->GetSession() ? handler->GetSession()->GetSecurity() : SEC_CONSOLE;
        if (security > mySecurity)
        {
            handler->SendSysMessage("You cannot unassign tickets from staff members with a higher security level than yourself.");
            return true;
        }

        SQLTransaction trans = SQLTransaction(NULL);
        ticket->SetUnassigned();
        ticket->SaveToDB(trans);
        sTicketMgr->UpdateLastChange();

        std::string msg = ticket->FormatMessageString(*handler, NULL, NULL, NULL, ticket->GetAssignedToName().c_str(), handler->GetSession() ? handler->GetSession()->GetPlayer()->GetName() : "Console");
        handler->SendGlobalGMSysMessage(msg.c_str());
        return true;
    }

    static bool HandleGMTicketGetByIdCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 ticketId = atoi(args);
        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket || ticket->IsClosed() || ticket->IsCompleted())
        {
            handler->SendSysMessage("Ticket not found.");
            return true;
        }

        SQLTransaction trans = SQLTransaction(NULL);
        ticket->SetViewed();
        ticket->SaveToDB(trans);
        handler->SendSysMessage(ticket->FormatMessageString(*handler, true).c_str());
        return true;
    }

    static bool HandleGMTicketGetByNameCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string name(args);
        if (!normalizePlayerName(name))
            return false;

        // Detect target's GUID
        uint64 guid = 0;
        if (Player* player = sObjectAccessor->FindPlayerByName(name.c_str()))
            guid = player->GetGUID();
        else
            guid = sObjectMgr->GetPlayerGUIDByName(name);

        // Target must exist
        if (!guid)
        {
            handler->SendSysMessage(LANG_NO_PLAYERS_FOUND);
            return true;
        }

        // Ticket must exist
        GmTicket* ticket = sTicketMgr->GetTicketByPlayer(guid);
        if (!ticket)
        {
            handler->SendSysMessage("Ticket not found.");
            return true;
        }

        SQLTransaction trans = SQLTransaction(NULL);
        ticket->SetViewed();
        ticket->SaveToDB(trans);
        handler->SendSysMessage(ticket->FormatMessageString(*handler, true).c_str());
        return true;
    }

    static bool _HandleGMTicketResponseAppendCommand(char const* args, bool newLine, ChatHandler* handler)
    {
        if (!*args)
            return false;

        char* ticketIdStr = strtok((char*)args, " ");
        uint32 ticketId = atoi(ticketIdStr);

        char* response = strtok(NULL, "\n");
        if (!response)
            return false;

        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket || ticket->IsClosed())
        {
            handler->PSendSysMessage("Ticket not found.");
            return true;
        }

        // Cannot add response to ticket, assigned to someone else
        //! Console excluded
        Player* player = handler->GetSession() ? handler->GetSession()->GetPlayer() : NULL;
        if (player && ticket->IsAssignedNotTo(player->GetGUID()))
        {
            handler->PSendSysMessage("Ticket(%d) is already assigned.", ticket->GetId());
            return true;
        }

        SQLTransaction trans = SQLTransaction(NULL);
        ticket->AppendResponse(response);
        if (newLine)
            ticket->AppendResponse("\n");
        ticket->SaveToDB(trans);
        handler->PSendSysMessage("Response successfully appended to Ticket(%d).", ticket->GetId());
        return true;
    }

    static bool HandleGMTicketResponseAppendCommand(ChatHandler* handler, char const* args)
    {
        return _HandleGMTicketResponseAppendCommand(args, false, handler);
    }

    static bool HandleGMTicketResponseAppendLnCommand(ChatHandler* handler, char const* args)
    {
        return _HandleGMTicketResponseAppendCommand(args, true, handler);
    }
};

void AddSC_ticket_commandscript()
{
    new ticket_commandscript();
}
