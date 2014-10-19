#include "PvPMgr.h"
#include "Player.h"
#include "ObjectAccessor.h"

uint32 PvPMgr::CalculateArenaPointsCap(uint64 guid)
{
    uint32 personalRatings[3] = { GetLifetime2v2Rating(guid), GetLifetime3v3Rating(guid), GetLifetime5v5Rating(guid) };
    uint32 highestPersonalRating = 0;

    for (uint8 i = 0; i < 3; ++i)
        if (personalRatings[i] > highestPersonalRating)
            highestPersonalRating = personalRatings[i];

    return (float)(highestPersonalRating * (highestPersonalRating / 1000));
}

uint32 PvPMgr::GetWeeklyArenaPoints(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetWeeklyArenaPoints();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT weeklyArenaPoints FROM characters WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt32();
    }
}

void PvPMgr::SetWeeklyArenaPoints(uint64 guid, uint32 arenaPoints)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetWeeklyArenaPoints(arenaPoints);

    CharacterDatabase.PExecute("UPDATE characters SET weeklyArenaPoints = '%u' WHERE guid = '%u'", arenaPoints, GUID_LOPART(guid));
}

uint32 PvPMgr::GetArenaPointsCap(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetArenaPointsCap();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT arenaPointsCap FROM characters WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt32();
    }
}

void PvPMgr::SetArenaPointsCap(uint64 guid, uint32 arenaPointsCap)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetArenaPointsCap(arenaPointsCap);

    CharacterDatabase.PExecute("UPDATE characters SET arenaPointsCap = '%u' WHERE guid = '%u'", arenaPointsCap, GUID_LOPART(guid));
}

uint16 PvPMgr::Get2v2MMR(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->Get2v2MMR();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT matchMakerRating FROM character_arena_stats WHERE slot = 0 AND guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::Set2v2MMR(uint64 guid, uint16 mmr)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->Set2v2MMR(mmr);

    CharacterDatabase.PExecute("UPDATE character_arena_stats SET matchMakerRating = '%u' WHERE slot = 0 AND guid = '%u'", mmr, GUID_LOPART(guid));

    if (mmr > GetLifetime2v2MMR(guid))
        SetLifetime2v2MMR(guid, mmr);
}

uint16 PvPMgr::Get3v3MMR(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->Get3v3MMR();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT matchMakerRating FROM character_arena_stats WHERE slot = 1 AND guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::Set3v3MMR(uint64 guid, uint16 mmr)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->Set3v3MMR(mmr);

    CharacterDatabase.PExecute("UPDATE character_arena_stats SET matchMakerRating = '%u' WHERE slot = 1 AND guid = '%u'", mmr, GUID_LOPART(guid));

    if (mmr > GetLifetime3v3MMR(guid))
        SetLifetime3v3MMR(guid, mmr);
}

uint16 PvPMgr::Get5v5MMR(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->Get5v5MMR();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT matchMakerRating FROM character_arena_stats WHERE slot = 2 AND guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::Set5v5MMR(uint64 guid, uint16 mmr)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->Set5v5MMR(mmr);

    CharacterDatabase.PExecute("UPDATE character_arena_stats SET matchMakerRating = '%u' WHERE slot = 2 AND guid = '%u'", mmr, GUID_LOPART(guid));

    if (mmr > GetLifetime5v5MMR(guid))
        SetLifetime5v5MMR(guid, mmr);
}

uint16 PvPMgr::GetLifetime2v2Rating(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime2v2Rating();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime2v2Rating FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime2v2Rating(uint64 guid, uint16 rating)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime2v2Rating(rating);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime2v2Rating = '%u' WHERE guid = '%u'", rating, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime2v2MMR(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime2v2MMR();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime2v2MMR FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime2v2MMR(uint64 guid, uint16 mmr)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime2v2MMR(mmr);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime2v2MMR = '%u' WHERE guid = '%u'", mmr, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime2v2Wins(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime2v2Wins();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime2v2Wins FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime2v2Wins(uint64 guid, uint16 wins)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime2v2Wins(wins);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime2v2Wins = '%u' WHERE guid = '%u'", wins, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime2v2Games(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime2v2Games();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime2v2Games FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime2v2Games(uint64 guid, uint16 games)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime2v2Games(games);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime2v2Games = '%u' WHERE guid = '%u'", games, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime3v3Rating(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime3v3Rating();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime3v3Rating FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime3v3Rating(uint64 guid, uint16 rating)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime3v3Rating(rating);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime3v3Rating = '%u' WHERE guid = '%u'", rating, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime3v3MMR(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime3v3MMR();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime3v3MMR FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime3v3MMR(uint64 guid, uint16 mmr)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime3v3MMR(mmr);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime3v3MMR = '%u' WHERE guid = '%u'", mmr, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime3v3Wins(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime3v3Wins();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime3v3Wins FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime3v3Wins(uint64 guid, uint16 wins)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime3v3Wins(wins);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime3v3Wins = '%u' WHERE guid = '%u'", wins, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime3v3Games(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime3v3Games();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime3v3Games FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime3v3Games(uint64 guid, uint16 games)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime3v3Games(games);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime3v3Games = '%u' WHERE guid = '%u'", games, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime5v5Rating(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime5v5Rating();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime5v5Rating FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime5v5Rating(uint64 guid, uint16 rating)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime5v5Rating(rating);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime5v5Rating = '%u' WHERE guid = '%u'", rating, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime5v5MMR(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime5v5MMR();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime5v5MMR FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime5v5MMR(uint64 guid, uint16 mmr)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime5v5MMR(mmr);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime5v5MMR = '%u' WHERE guid = '%u'", mmr, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime5v5Wins(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime5v5Wins();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime5v5Wins FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime5v5Wins(uint64 guid, uint16 wins)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime5v5Wins(wins);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime5v5Wins = '%u' WHERE guid = '%u'", wins, GUID_LOPART(guid));
}

uint16 PvPMgr::GetLifetime5v5Games(uint64 guid) const
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        return player->GetLifetime5v5Games();
    else
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT Lifetime5v5Games FROM character_pvp_stats WHERE guid = '%u'", GUID_LOPART(guid));
        if (!result)
            return 0;

        return (*result)[0].GetUInt16();
    }
}

void PvPMgr::SetLifetime5v5Games(uint64 guid, uint16 games)
{
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetLifetime5v5Games(games);

    CharacterDatabase.PExecute("UPDATE character_pvp_stats SET Lifetime5v5Games = '%u' WHERE guid = '%u'", games, GUID_LOPART(guid));
}
