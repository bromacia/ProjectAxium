#include "Define.h"
#include "ArenaTeamMgr.h"
#include "World.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "Language.h"
#include "ObjectAccessor.h"

ArenaTeamMgr::ArenaTeamMgr()
{
    NextArenaTeamId = 1;
}

ArenaTeamMgr::~ArenaTeamMgr()
{
    for (ArenaTeamContainer::iterator itr = arenaTeamStore.begin(); itr != arenaTeamStore.end(); ++itr)
        delete itr->second;
}

// Arena teams collection
ArenaTeam* ArenaTeamMgr::GetArenaTeamById(uint32 arenaTeamId) const
{
    ArenaTeamContainer::const_iterator itr = arenaTeamStore.find(arenaTeamId);
    if (itr != arenaTeamStore.end())
        return itr->second;

    return NULL;
}

ArenaTeam* ArenaTeamMgr::GetArenaTeamByName(const std::string& arenaTeamName) const
{
    std::string search = arenaTeamName;
    std::transform(search.begin(), search.end(), search.begin(), ::toupper);
    for (ArenaTeamContainer::const_iterator itr = arenaTeamStore.begin(); itr != arenaTeamStore.end(); ++itr)
    {
        std::string teamName = itr->second->GetName();
        std::transform(teamName.begin(), teamName.end(), teamName.begin(), ::toupper);
        if (search == teamName)
            return itr->second;
    }
    return NULL;
}

ArenaTeam* ArenaTeamMgr::GetArenaTeamByCaptain(uint64 guid) const
{
    for (ArenaTeamContainer::const_iterator itr = arenaTeamStore.begin(); itr != arenaTeamStore.end(); ++itr)
        if (itr->second->GetCaptain() == guid)
            return itr->second;

    return NULL;
}

void ArenaTeamMgr::AddArenaTeam(ArenaTeam* arenaTeam)
{
    arenaTeamStore[arenaTeam->GetId()] = arenaTeam;
}

void ArenaTeamMgr::RemoveArenaTeam(uint32 arenaTeamId)
{
    arenaTeamStore.erase(arenaTeamId);
}

uint32 ArenaTeamMgr::GenerateArenaTeamId()
{
    if (NextArenaTeamId >= 0xFFFFFFFE)
    {
        sLog->outError("Arena team ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return NextArenaTeamId++;
}

void ArenaTeamMgr::LoadArenaTeams()
{
    uint32 oldMSTime = getMSTime();

    // Clean out the trash before loading anything
    CharacterDatabase.Execute("DELETE FROM arena_team_member WHERE arenaTeamId NOT IN (SELECT arenaTeamId FROM arena_team)");

    //                                                                   0        1         2         3          4              5            6            7           8
    QueryResult result = CharacterDatabase.Query("SELECT arena_team.arenaTeamId, name, captainGuid, type, backgroundColor, emblemStyle, emblemColor, borderStyle, borderColor, "
        //                                               9        10        11         12           13       14
        "rating, weekGames, weekWins, seasonGames, seasonWins, rank FROM arena_team ORDER BY arena_team.arenaTeamId ASC");

    if (!result)
    {
        sLog->outString(">> Loaded 0 arena teams. DB table `arena_team` is empty!");
        sLog->outString();
        return;
    }

    QueryResult result2 = CharacterDatabase.Query(
        //              0              1           2             3              4                 5          6     7          8                  9
        "SELECT arenaTeamId, atm.guid, atm.weekGames, atm.weekWins, atm.seasonGames, atm.seasonWins, c.name, class, personalRating, matchMakerRating FROM arena_team_member atm"
        " INNER JOIN arena_team ate USING (arenaTeamId)"
        " LEFT JOIN characters AS c ON atm.guid = c.guid"
        " LEFT JOIN character_arena_stats AS cas ON c.guid = cas.guid AND (cas.slot = 0 AND ate.type = 2 OR cas.slot = 1 AND ate.type = 3 OR cas.slot = 2 AND ate.type = 5)"
        " ORDER BY atm.arenateamid ASC");

    uint32 count = 0;
    do
    {
        ArenaTeam* newArenaTeam = new ArenaTeam;

        if (!newArenaTeam->LoadArenaTeamFromDB(result) || !newArenaTeam->LoadMembersFromDB(result2))
        {
            newArenaTeam->Disband(NULL);
            delete newArenaTeam;
            continue;
        }

        AddArenaTeam(newArenaTeam);

        ++count;
    }
    while (result->NextRow());

    sLog->outString(">> Loaded %u arena teams in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

void ArenaTeamMgr::ResetWeeklyCap()
{
    sWorld->SendGlobalText("Starting update of arena statistics...", NULL);

    for (ArenaTeamContainer::iterator itr = arenaTeamStore.begin(); itr != arenaTeamStore.end(); ++itr)
    {
        if (ArenaTeam* at = itr->second)
        {
            at->FinishWeek();
            at->SaveToDB();
            at->NotifyStatsChanged();
        }
    }

    SessionMap sessions = sWorld->GetAllSessions();
    for (SessionMap::const_iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
    {
        if (Player* player = itr->second->GetPlayer())
        {
            player->SetWeeklyArenaPoints(0);
            player->SetArenaPointsCap(0);
        }
    }

    CharacterDatabase.PExecute("UPDATE characters SET weeklyArenaPoints = 0, arenaPointsCap = 0");

    sWorld->SendGlobalText("Update of arena statistics finished.", NULL);
}
