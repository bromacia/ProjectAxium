#ifndef _LOGINDATABASE_H
#define _LOGINDATABASE_H

#include "DatabaseWorkerPool.h"
#include "MySQLConnection.h"

class LoginDatabaseConnection : public MySQLConnection
{
    public:
        //- Constructors for sync and async connections
        LoginDatabaseConnection(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo) {}
        LoginDatabaseConnection(ACE_Activation_Queue* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo) {}

        //- Loads database type specific prepared statements
        void DoPrepareStatements();
};

typedef DatabaseWorkerPool<LoginDatabaseConnection> LoginDatabaseWorkerPool;

enum LoginDatabaseStatements
{
    /*  Naming standard for defines:
        {DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
        When updating more than one field, consider looking at the calling function
        name for a suiting suffix.
    */

    LOGIN_SEL_REALMLIST,
    LOGIN_DEL_EXPIRED_IP_BANS,
    LOGIN_UPD_EXPIRED_ACCOUNT_BANS,
    LOGIN_SEL_IP_BANNED,
    LOGIN_INS_IP_AUTO_BANNED,
    LOGIN_SEL_ACCOUNT_BANNED,
    LOGIN_INS_ACCOUNT_AUTO_BANNED,
    LOGIN_SEL_SESSIONKEY,
    LOGIN_UPD_VS,
    LOGIN_UPD_LOGONPROOF,
    LOGIN_SEL_LOGONCHALLENGE,
    LOGIN_UPD_FAILEDLOGINS,
    LOGIN_SEL_FAILEDLOGINS,
    LOGIN_SEL_ACCOUNT_ID_BY_NAME,
    LOGIN_SEL_ACCOUNT_BY_IP,
    LOGIN_INS_IP_BANNED,
    LOGIN_DEL_IP_NOT_BANNED,
    LOGIN_INS_ACCOUNT_BANNED,
    LOGIN_UPD_ACCOUNT_NOT_BANNED,
    LOGIN_INS_ACCOUNT,
    LOGIN_DEL_OLD_BANS,
    LOGIN_DEL_OLD_IP_BANS,
    LOGIN_UPD_EXPANSION,
    LOGIN_UPD_ACCOUNT_LOCK,
    LOGIN_INS_LOG,
    LOGIN_UPD_USERNAME,
    LOGIN_UPD_PASSWORD,
    LOGIN_UPD_MUTE_TIME,
    LOGIN_UPD_LAST_IP,
    LOGIN_UPD_ACCOUNT_ONLINE,
    LOGIN_UPD_UPTIME_PLAYERS,
    LOGIN_DEL_OLD_LOGS,
    LOGIN_DEL_ACCOUNT_ACCESS,
    LOGIN_DEL_ACCOUNT_ACCESS_BY_REALM,
    LOGIN_INS_ACCOUNT_ACCESS,
    LOGIN_DEL_ACCOUNT_IP_HISTORY,
    LOGIN_INS_ACCOUNT_IP_HISTORY,

    MAX_LOGINDATABASE_STATEMENTS,
};

#endif
