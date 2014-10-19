#include "LogDatabase.h"

void LogDatabaseConnection::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_LOGDATABASE_STATEMENTS);

    PREPARE_STATEMENT(LOG_INS_COMMAND_LOG, "INSERT INTO command_logs (sender_accid, sender_guid, target_accid, target_guid, target_typeid, command) VALUES (?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(LOG_INS_CHAT_LOG, "INSERT INTO chat_logs (sender_accid, sender_guid, text, receiver_accid, receiver_guid, type) VALUES (?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(LOG_INS_ERROR_LOG, "INSERT INTO error_logs (type, string) VALUES (?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(LOG_INS_TRADE_LOG, "INSERT INTO trade_logs (trader_accid, trader_guid, item_id, item_count, receiver_accid, receiver_guid) VALUES (?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
}
