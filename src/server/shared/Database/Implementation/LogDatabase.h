#ifndef _LOGDATABASE_H
#define _LOGDATABASE_H

#include "DatabaseWorkerPool.h"
#include "MySQLConnection.h"

class LogDatabaseConnection : public MySQLConnection
{
    public:
        //- Constructors for sync and async connections
        LogDatabaseConnection(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo) {}
        LogDatabaseConnection(ACE_Activation_Queue* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo) {}

        //- Loads database type specific prepared statements
        void DoPrepareStatements();
};

typedef DatabaseWorkerPool<LogDatabaseConnection> LogDatabaseWorkerPool;

enum LogDatabaseStatements
{
    /*  Naming standard for defines:
        {DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
        When updating more than one field, consider looking at the calling function
        name for a suiting suffix.
    */

    LOG_INS_COMMAND_LOG,
    LOG_INS_CHAT_LOG,
    LOG_INS_ERROR_LOG,
    LOG_INS_TRADE_LOG,

    MAX_LOGDATABASE_STATEMENTS,
};

#endif
