#ifndef _ARENATEAMMGR_H
#define _ARENATEAMMGR_H

#include "ArenaTeam.h"

class ArenaTeamMgr
{
    friend class ACE_Singleton<ArenaTeamMgr, ACE_Null_Mutex>;

    private:
        ArenaTeamMgr();
        ~ArenaTeamMgr();

    public:
        typedef UNORDERED_MAP<uint32, ArenaTeam*> ArenaTeamContainer;

        ArenaTeam* GetArenaTeamById(uint32 arenaTeamId) const;
        ArenaTeam* GetArenaTeamByName(std::string const& arenaTeamName) const;
        ArenaTeam* GetArenaTeamByCaptain(uint64 guid) const;

        void LoadArenaTeams();
        void AddArenaTeam(ArenaTeam* arenaTeam);
        void RemoveArenaTeam(uint32 Id);

        void ResetWeeklyCap();

        uint32 GenerateArenaTeamId();
        void SetNextArenaTeamId(uint32 Id) { NextArenaTeamId = Id; }

        const ArenaTeamContainer& GetArenaTeamStore() { return arenaTeamStore; }

    private:
        uint32 NextArenaTeamId;
        ArenaTeamContainer arenaTeamStore;
};

#define sArenaTeamMgr ACE_Singleton<ArenaTeamMgr, ACE_Null_Mutex>::instance()

#endif
