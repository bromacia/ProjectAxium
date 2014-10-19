#include "ChallengeMgr.h"
#include "Chat.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BattlegroundQueue.h"
#include "Player.h"
#include "SocialMgr.h"
#include "Group.h"
#include "SharedDefines.h"

void ChallengeMgr::HandleChallenge(uint64 challengerGUID, uint64 challengedGUID, uint8 map)
{
    Player* challengerPlayer = ObjectAccessor::FindPlayer(challengerGUID);
    if (!challengerPlayer)
        return;

    Player* challengedPlayer = ObjectAccessor::FindPlayer(challengedGUID);
    if (!challengedPlayer)
        return;

    if (challengerPlayer == challengedPlayer)
    {
        challengerPlayer->SendSysMessage("You can't challenge yourself.");
        return;
    }

    if (challengerPlayer->IsFrozen())
    {
        challengerPlayer->SendSysMessage("You can't challenge another player while frozen.");
        return;
    }

    if (challengedPlayer->IsFrozen())
    {
        challengerPlayer->SendSysMessage("The player you're trying to challenge is frozen.");
        return;
    }

    if (challengedPlayer->HasGameMasterTagOn())
    {
        challengerPlayer->SendSysMessage("You can't challenge Game Masters.");
        return;
    }

    if (challengedPlayer->isDND())
    {
        challengerPlayer->SendSysMessage("You can't challenge players with Do Not Disturbed enabled.");
        return;
    }

    if (challengedPlayer->GetSocial()->HasIgnore(GUID_LOPART(challengerGUID)))
    {
        challengerPlayer->SendSysMessage("You can't challenge players who have ignored you.");
        return;
    }

    if (challengerPlayer->InBattleground())
    {
        challengerPlayer->SendSysMessage("You can't challenge another player while in a battleground.");
        return;
    }

    if (challengedPlayer->InBattleground())
    {
        challengerPlayer->SendSysMessage("The player you're trying to challenge is in a battleground.");
        return;
    }

    if (challengerPlayer->InBattlegroundQueue())
    {
        challengerPlayer->SendSysMessage("You can't challenge another player while in a battleground or arena queue.");
        return;
    }

    if (challengedPlayer->InBattlegroundQueue())
    {
        challengerPlayer->SendSysMessage("The player you're trying to challenge is in a battleground or arena queue.");
        return;
    }

    Group* groupA = challengerPlayer->GetGroup();
    Group* groupB = challengedPlayer->GetGroup();

    if (groupA && groupB && groupA != groupB)
    {
        if (groupA->GetMembersCount() != groupB->GetMembersCount())
        {
            challengerPlayer->SendSysMessage("Your group size doesn't match %s's group size.", challengedPlayer->GetName());
            return;
        }

        switch (groupA->GetMembersCount())
        {
            case 2: arenaType = ARENA_TYPE_2v2; break;
            case 3: arenaType = ARENA_TYPE_3v3; break;
            case 5: arenaType = ARENA_TYPE_5v5; break;
        }

        if (!arenaType)
        {
            challengerPlayer->SendSysMessage("Your group size doens't fit into any arena bracket.");
            return;
        }

        Group::MemberSlotList membersA = groupA->GetMembers();
        Group::MemberSlotList membersB = groupB->GetMembers();

        for (Group::member_citerator citr = membersA.begin(); citr != membersA.end(); ++citr)
        {
            Player* member = ObjectAccessor::FindPlayer(citr->guid);

            if (member->InBattlegroundQueue())
            {
                challengerPlayer->SendSysMessage("A player from your group is in a battleground or arena queue.");
                return;
            }

            if (member->InBattleground())
            {
                challengerPlayer->SendSysMessage("A player from your group is in a battleground.");
                return;
            }

            players[citr->guid] = PLAYER_TEAM_A;
        }

        for (Group::member_citerator citr = membersB.begin(); citr != membersB.end(); ++citr)
        {
            Player* member = ObjectAccessor::FindPlayer(citr->guid);

            if (member->InBattlegroundQueue())
            {
                challengerPlayer->SendSysMessage("A player in the group of the player you're challenging is in a battleground or arena queue.");
                return;
            }

            if (member->InBattleground())
            {
                challengerPlayer->SendSysMessage("A player in the group of the player you're challenging is in a battleground.");
                return;
            }

            players[citr->guid] = PLAYER_TEAM_B;
        }
    }
    else
    {
        arenaType = ARENA_TYPE_2v2;
        players[challengerGUID] = PLAYER_TEAM_A;
        players[challengedGUID] = PLAYER_TEAM_B;
    }

    switch (map)
    {
        case 1:  bgTypeId = BATTLEGROUND_NA; break;
        case 2:  bgTypeId = BATTLEGROUND_BE; break;
        case 3:  bgTypeId = BATTLEGROUND_RL; break;
        case 4:  bgTypeId = BATTLEGROUND_DS; break;
        default: bgTypeId = BATTLEGROUND_AA; break;
    }

    Battleground* battlegroundTemplate = sBattlegroundMgr->GetBattlegroundTemplate((BattlegroundTypeId)bgTypeId);
    if (!battlegroundTemplate)
    {
        challengerPlayer->SendSysMessage("Unable to find battleground template for type Id %u.", bgTypeId);
        return;
    }

    const PvPDifficultyEntry* bracketEntry = GetBattlegroundBracketByLevel(battlegroundTemplate->GetMapId(), challengedPlayer->getLevel());
    if (!bracketEntry)
    {
        challengerPlayer->SendSysMessage("Unable to find bracket entry for map %u.", battlegroundTemplate->GetMapId());
        return;
    }

    CreateBattleground(bracketEntry, (players.size() / 2));
    AddPlayersToBattleground(bracketEntry);
    CleanupChallenge();
}

void ChallengeMgr::CreateBattleground(const PvPDifficultyEntry* bracketEntry, uint8 teamSize)
{
    bg = sBattlegroundMgr->CreateNewBattleground((BattlegroundTypeId)bgTypeId, bracketEntry, arenaType, false, bgTypeId == BATTLEGROUND_AA ? true : false);
    bg->SetRated(false);
    bg->SetChallenge(true);
    bg->SetChallengeTeamSize(teamSize);
}

void ChallengeMgr::AddPlayersToBattleground(const PvPDifficultyEntry* bracketEntry)
{
    uint32 msTime = getMSTime();

    GroupQueueInfo* ginfoA               = new GroupQueueInfo;
    ginfoA->BgTypeId                     = (BattlegroundTypeId)bgTypeId;
    ginfoA->ArenaType                    = arenaType;
    ginfoA->IsRated                      = false;
    ginfoA->IsInvitedToBGInstanceGUID    = 0;
    ginfoA->JoinTime                     = msTime;
    ginfoA->RemoveInviteTime             = 0;
    ginfoA->Team                         = ALLIANCE;
    ginfoA->ArenaTeamId                  = 0;
    ginfoA->ArenaTeamRating              = 0;
    ginfoA->ArenaMatchmakerRating        = 0;
    ginfoA->OTeam                        = HORDE;
    ginfoA->OpponentsTeamRating          = 0;
    ginfoA->OpponentsMatchmakerRating    = 0;
    ginfoA->Players.clear();

    GroupQueueInfo* ginfoB               = new GroupQueueInfo;
    ginfoB->BgTypeId                     = (BattlegroundTypeId)bgTypeId;
    ginfoB->ArenaType                    = arenaType;
    ginfoB->IsRated                      = false;
    ginfoB->IsInvitedToBGInstanceGUID    = 0;
    ginfoB->JoinTime                     = msTime;
    ginfoB->RemoveInviteTime             = 0;
    ginfoB->Team                         = HORDE;
    ginfoB->ArenaTeamId                  = 0;
    ginfoB->ArenaTeamRating              = 0;
    ginfoB->ArenaMatchmakerRating        = 0;
    ginfoB->OTeam                        = ALLIANCE;
    ginfoB->OpponentsTeamRating          = 0;
    ginfoB->OpponentsMatchmakerRating    = 0;
    ginfoB->Players.clear();

    BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId((BattlegroundTypeId)bgTypeId, bg->GetArenaType());
    BattlegroundQueue& bgQueue = sBattlegroundMgr->m_BattlegroundQueues[bgQueueTypeId];

    for (Players::const_iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        Player* player = ObjectAccessor::FindPlayer(itr->first);
        if (!player)
            continue;

        switch (itr->second)
        {
            case PLAYER_TEAM_A:
            {
                PlayerQueueInfo& pQueueInfo = bgQueue.m_QueuedPlayers[itr->first];
                pQueueInfo.LastOnlineTime   = msTime;
                pQueueInfo.GroupInfo        = ginfoA;
                ginfoA->Players[itr->first] = &pQueueInfo;
                player->challengeInfo.ginfo = ginfoA;
                break;
            }
            case PLAYER_TEAM_B:
            {
                PlayerQueueInfo& pQueueInfo = bgQueue.m_QueuedPlayers[itr->first];
                pQueueInfo.LastOnlineTime   = msTime;
                pQueueInfo.GroupInfo        = ginfoB;
                ginfoB->Players[itr->first] = &pQueueInfo;
                player->challengeInfo.ginfo = ginfoB;
                break;
            }
        }

        WorldPacket data;
        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, player->AddBattlegroundQueueId((BattlegroundQueueTypeId)bgQueueTypeId), STATUS_WAIT_QUEUE, 0, 0, arenaType);
        player->GetSession()->SendPacket(&data);
    }

    bgQueue.InviteGroupToBG(ginfoA, bg);
    bgQueue.InviteGroupToBG(ginfoB, bg);
    bg->StartBattleground();
}

void ChallengeMgr::CleanupChallenge()
{
    players.clear();
    arenaType = 0;
    bgTypeId = 0;
    bg = NULL;
}

ChatCommand* challenge_commandscript::GetCommands() const
{
    static ChatCommand commandTable[] =
    {
        { "challenge",          SEC_PLAYER,         false, &HandleChallengeCommand,                 "", NULL },
        { NULL,                 0,                  false, NULL,                                    "", NULL }
    };
    return commandTable;
}

bool challenge_commandscript::HandleChallengeCommand(ChatHandler* handler, const char* args)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return false;

    if (!*args)
    {
        player->SendSysMessage("Incorrect syntax.");
        player->SendSysMessage("Syntax: .challenge $playerName #arenaMap");
        player->SendSysMessage("Please refer to your Player Handbook for arena map numbers and more information.");
        handler->SetSentErrorMessage(true);
        return false;
    }

    std::string name = strtok((char*)args, " ");
    if (!normalizePlayerName(name))
    {
        player->SendSysMessage("Invalid player name.");
        handler->SetSentErrorMessage(true);
        return false;
    }

    uint64 guid = 0;
    if (Player* player = sObjectAccessor->FindPlayerByName(name.c_str()))
        guid = player->GetGUID();
    else
        guid = sObjectMgr->GetPlayerGUIDByName(name);

    if (!guid)
    {
        player->SendSysMessage("No player found with the name %s.", name.c_str());
        handler->SetSentErrorMessage(true);
        return false;
    }

    uint8 bgTypeId = 0;
    char* bgTypeIdArg = strtok(NULL, " ");

    if (bgTypeIdArg)
        bgTypeId = (uint8)atoi(bgTypeIdArg);

    sChallengeMgr->HandleChallenge(player->GetGUID(), guid, bgTypeId);
    return true;
}

void AddSC_challenge_commandscript()
{
    new challenge_commandscript();
}
