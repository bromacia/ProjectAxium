#include "Chat.h"
#include "Player.h"
#include "Group.h"

class group_commandscript : public CommandScript
{
public:
    group_commandscript() : CommandScript("group_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand groupCommandTable[] =
        {
            { "leader",          SEC_GAMEMASTER,     false, &HandleLeaderCommand,                    "", NULL },
            { "disband",         SEC_GAMEMASTER,     false, &HandleDisbandCommand,                   "", NULL },
            { "remove",          SEC_GAMEMASTER,     false, &HandleRemoveCommand,                    "", NULL },
            { "summon",          SEC_GAMEMASTER,     false, &HandleSummonCommand,                    "", NULL },
            { "freeze",          SEC_GAMEMASTER,     false, &HandleFreezeCommand,                    "", NULL },
            { "unfreeze",        SEC_GAMEMASTER,     false, &HandleUnFreezeCommand,                  "", NULL },
            { "prepare",         SEC_GAMEMASTER,     false, &HandlePrepareCommand,                   "", NULL },
            { NULL,              0,                  false, NULL,                                    "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "group",           SEC_GAMEMASTER,     false, NULL,                       "", groupCommandTable },
            { NULL,              0,                  false, NULL,                                    "", NULL }
        };
        return commandTable;
    }

    static bool HandleLeaderCommand(ChatHandler* handler, const char* args)
    {
        Player* player;
        Group* group = handler->GetSession()->GetPlayer()->GetGroup();
        uint64 guid;
        char* cname  = strtok((char*)args, " ");

        if (handler->GetPlayerGroupAndGUIDByName(cname, player, group, guid))
            if (group && group->GetLeaderGUID() != guid)
            {
                group->ChangeLeader(guid);
                group->SendUpdate();
            }
        return true;
    }

    static bool HandleDisbandCommand(ChatHandler* handler, const char* args)
    {
        Player* player;
        Group* group = handler->GetSession()->GetPlayer()->GetGroup();
        uint64 guid;
        char* cname  = strtok((char*)args, " ");

        if (handler->GetPlayerGroupAndGUIDByName(cname, player, group, guid))
            if (group)
                group->Disband();
        return true;
    }

    static bool HandleRemoveCommand(ChatHandler* handler, const char* args)
    {
        Player* player;
        Group* group = handler->GetSession()->GetPlayer()->GetGroup();
        uint64 guid;
        char* cname  = strtok((char*)args, " ");

        if (handler->GetPlayerGroupAndGUIDByName(cname, player, group, guid))
            if (group)
                group->RemoveMember(guid);
        return true;
    }

    static bool HandleSummonCommand(ChatHandler* handler, const char* args)
    {
        WorldSession* m_session = handler->GetSession();
        Player* target;

        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        Group* grp = target->GetGroup();

        std::string nameLink = handler->GetNameLink(target);

        // check online security
        if (handler->HasLowerSecurity(target, 0))
            return false;

        if (!grp)
        {
            handler->PSendSysMessage(LANG_NOT_IN_GROUP, nameLink.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        Map* gmMap = m_session->GetPlayer()->GetMap();
        bool to_instance =  gmMap->Instanceable();

        // we are in instance, and can summon only player in our group with us as lead
        if (to_instance && (
            !m_session->GetPlayer()->GetGroup() || (grp->GetLeaderGUID() != m_session->GetPlayer()->GetGUID()) ||
            (m_session->GetPlayer()->GetGroup()->GetLeaderGUID() != m_session->GetPlayer()->GetGUID())))
            // the last check is a bit excessive, but let it be, just in case
        {
            handler->SendSysMessage(LANG_CANNOT_SUMMON_TO_INST);
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

            if (player->IsBeingTeleported() == true)
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, plNameLink.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (to_instance)
            {
                Map* plMap = player->GetMap();

                if (plMap->Instanceable() && plMap->GetInstanceId() != gmMap->GetInstanceId())
                {
                    // cannot summon from instance to instance
                    handler->PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, plNameLink.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }

            handler->PSendSysMessage(LANG_SUMMONING, plNameLink.c_str(), "");
            if (handler->needReportToTarget(player))
                player->SendSysMessage(LANG_SUMMONED_BY, handler->GetNameLink().c_str());

            // stop flight if need
            if (player->isInFlight())
            {
                player->GetMotionMaster()->MovementExpired();
                player->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                player->SaveRecallPosition();

            // before GM
            float x, y, z;
            m_session->GetPlayer()->GetClosePoint(x, y, z, player->GetObjectSize());
            player->TeleportTo(m_session->GetPlayer()->GetMapId(), x, y, z, player->GetOrientation());
        }
        return true;
    }

    static bool HandleFreezeCommand(ChatHandler* handler, const char* args)
    {
        WorldSession* m_session = handler->GetSession();
        Player* target;

        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        Group* grp = target->GetGroup();

        std::string nameLink = handler->GetNameLink(target);

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

            player->CombatStop();
            if (player->IsNonMeleeSpellCasted(true))
                player->InterruptNonMeleeSpells(true);
            player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            player->RemoveAllControlled();

            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(9454))
                Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, player, player);

            CharacterDatabase.PQuery("UPDATE characters SET isFrozen = 1 WHERE guid = '%u'", player->GetGUIDLow());
        }
        return true;
    }

    static bool HandleUnFreezeCommand(ChatHandler* handler, const char* args)
    {
        WorldSession* m_session = handler->GetSession();
        Player* target;

        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        Group* grp = target->GetGroup();

        std::string nameLink = handler->GetNameLink(target);

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

            player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            player->RemoveAura(9454);
            CharacterDatabase.PQuery("UPDATE characters SET isFrozen = 0 WHERE guid = '%u'", player->GetGUIDLow());
        }
        return true;
    }

    static bool HandlePrepareCommand(ChatHandler* handler, const char* args)
    {
        WorldSession* m_session = handler->GetSession();
        Player* target;

        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        uint64 target_guid = target->GetGUID();
        Group* grp = target->GetGroup();

        std::string nameLink = handler->GetNameLink(target);

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

            if (player)
            {
                player->ResurrectPlayer(1.0f);
                player->SpawnCorpseBones();
                player->SaveToDB();
                player->RemoveAllPlayerSpellCooldowns();
                player->RemoveAllNegativeAuras();
                player->ClearDiminishings();
                player->ClearComboPoints();
                player->ClearInCombat();
                player->getHostileRefManager().deleteReferences();
            }
            else
                // will resurrected at login without corpse
                sObjectAccessor->ConvertCorpseForPlayer(target_guid);

            if (Pet* pet = player->GetPet())
                pet->RemoveAllPetSpellCooldowns(player);
        }
        return true;
    }
};

void AddSC_group_commandscript()
{
    new group_commandscript();
}