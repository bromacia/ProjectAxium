#ifndef PVP_MGR_H_
#define PVP_MGR_H_

#include <ace/Singleton.h>

class Player;

enum PvPTitle
{
    TITLE_A_PRIVATE              = 1,
    TITLE_A_CORPORAL             = 2,
    TITLE_A_SERGEANT             = 3,
    TITLE_A_MASTER_SERGEANT      = 4,
    TITLE_A_SERGEANT_MAJOR       = 5,
    TITLE_A_KNIGHT               = 6,
    TITLE_A_KNIGHT_LIEUTENANT    = 7,
    TITLE_A_KNIGHT_CAPTAIN       = 8,
    TITLE_A_KNIGHT_CHAMPION      = 9,
    TITLE_A_LIEUTENANT_COMMANDER = 10,
    TITLE_A_COMMANDER            = 11,
    TITLE_A_MARSHAL              = 12,
    TITLE_A_FIELD_MARSHAL        = 13,
    TITLE_A_GRAND_MARSHAL        = 14,
    TITLE_H_SCOUT                = 15,
    TITLE_H_GRUNT                = 16,
    TITLE_H_SERGEANT             = 17,
    TITLE_H_SENIOR_SERGEANT      = 18,
    TITLE_H_FIRST_SERGEANT       = 19,
    TITLE_H_STONE_GUARD          = 20,
    TITLE_H_BLOOD_GUARD          = 21,
    TITLE_H_LEGIONAIRE           = 22,
    TITLE_H_CENTURION            = 23,
    TITLE_H_CHAMPION             = 24,
    TITLE_H_LIEUTENANT_GENERAL   = 25,
    TITLE_H_GENERAL              = 26,
    TITLE_H_WARLORD              = 27,
    TITLE_H_HIGH_WARLORD         = 28,
};

class PvPMgr
{
    friend class ACE_Singleton<PvPMgr, ACE_Null_Mutex>;

    private:
        PvPMgr() {}
        ~PvPMgr() {}

    public:
        uint32 CalculateArenaPointsCap(uint64 guid);

        uint32 GetWeeklyArenaPoints(uint64 guid) const;
        void SetWeeklyArenaPoints(uint64, uint32 arenaPoints);
        uint32 GetArenaPointsCap(uint64 guid) const;
        void SetArenaPointsCap(uint64, uint32 arenaPointsCap);

        uint16 Get2v2MMR(uint64 guid) const;
        void Set2v2MMR(uint64 guid, uint16 mmr);
        uint16 Get3v3MMR(uint64 guid) const;
        void Set3v3MMR(uint64 guid, uint16 mmr);
        uint16 Get5v5MMR(uint64 guid) const;
        void Set5v5MMR(uint64 guid, uint16 mmr);

        uint16 GetLifetime2v2Rating(uint64 guid) const;
        void SetLifetime2v2Rating(uint64 guid, uint16 rating);
        uint16 GetLifetime2v2MMR(uint64 guid) const;
        void SetLifetime2v2MMR(uint64 guid, uint16 mmr);
        uint16 GetLifetime2v2Wins(uint64 guid) const;
        void SetLifetime2v2Wins(uint64 guid, uint16 wins);
        uint16 GetLifetime2v2Games(uint64 guid) const;
        void SetLifetime2v2Games(uint64 guid, uint16 games);

        uint16 GetLifetime3v3Rating(uint64 guid) const;
        void SetLifetime3v3Rating(uint64 guid, uint16 rating);
        uint16 GetLifetime3v3MMR(uint64 guid) const;
        void SetLifetime3v3MMR(uint64 guid, uint16 mmr);
        uint16 GetLifetime3v3Wins(uint64 guid) const;
        void SetLifetime3v3Wins(uint64 guid, uint16 wins);
        uint16 GetLifetime3v3Games(uint64 guid) const;
        void SetLifetime3v3Games(uint64 guid, uint16 games);

        uint16 GetLifetime5v5Rating(uint64 guid) const;
        void SetLifetime5v5Rating(uint64 guid, uint16 rating);
        uint16 GetLifetime5v5MMR(uint64 guid) const;
        void SetLifetime5v5MMR(uint64 guid, uint16 mmr);
        uint16 GetLifetime5v5Wins(uint64 guid) const;
        void SetLifetime5v5Wins(uint64 guid, uint16 wins);
        uint16 GetLifetime5v5Games(uint64 guid) const;
        void SetLifetime5v5Games(uint64 guid, uint16 games);
};

#define sPvPMgr ACE_Singleton<PvPMgr, ACE_Null_Mutex>::instance()

#endif /*PVP_MGR_H_*/
