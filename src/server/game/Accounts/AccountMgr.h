#ifndef _ACCMGR_H
#define _ACCMGR_H

#include "Define.h"
#include <string>

enum AccountOpResult
{
    AOR_OK,
    AOR_NAME_TOO_LONG,
    AOR_PASS_TOO_LONG,
    AOR_NAME_ALREDY_EXIST,
    AOR_NAME_NOT_EXIST,
    AOR_DB_INTERNAL_ERROR
};

#define MAX_ACCOUNT_STR 16

namespace AccountMgr
{
        AccountOpResult CreateAccount(std::string username, std::string password);
        AccountOpResult DeleteAccount(uint32 accountId);
        AccountOpResult ChangeUsername(uint32 accountId, std::string newUsername, std::string newPassword);
        AccountOpResult ChangePassword(uint32 accountId, std::string newPassword);
        bool CheckPassword(uint32 accountId, std::string password);

        uint32 GetId(std::string username);
        uint32 GetSecurity(uint32 accountId);
        uint32 GetSecurity(uint64 accountId, int32 realmId);
        bool GetName(uint32 accountId, std::string& name);
        uint32 GetCharactersCount(uint32 accountId);
        std::string CalculateShaPassHash(std::string& name, std::string& password);

        bool normalizeString(std::string& utf8String);
        bool IsPlayerAccount(uint32 gmlevel);
        bool IsGMAccount(uint32 gmlevel);
        bool IsConsoleAccount(uint32 gmlevel);
};

#endif
