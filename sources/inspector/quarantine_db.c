#include "inspector/inspector.h"
#include "logger/logger.h"

#define SQL_INSERT_QUARANTINE                                                  \
  "INSERT OR IGNORE INTO quarantine (filename, filepath, detected, datetime) " \
  "VALUES (?, ?, ?, ?)"

#define SQL_SELECT_QUARANTINE "SELECT * FROM quarantine"

#define SQL_SELECT_WHERE_QUARANTINE            \
  "SELECT * FROM quarantine WHERE filepath = " \
  "?"                                          \
  " "

ERR
insert_quarantine_db(INSPECTOR **inspector, QUARANTINE_FILES **file)
{
  int rc = sqlite3_prepare((*inspector)->db, SQL_INSERT_QUARANTINE, -1,
                           &(*inspector)->stmt, NULL);
  if (rc != SQLITE_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc,
                                 sqlite3_errmsg((*inspector)->db)));
    return ERR_FAILURE;
  }

  sqlite3_bind_text((*inspector)->stmt, 1, (*file)->filename, -1,
                    SQLITE_STATIC);
  sqlite3_bind_text((*inspector)->stmt, 2, (*file)->filepath, -1,
                    SQLITE_STATIC);
  sqlite3_bind_text((*inspector)->stmt, 3, (*file)->detected, -1,
                    SQLITE_STATIC);
  sqlite3_bind_text((*inspector)->stmt, 4, (*file)->datetime, -1,
                    SQLITE_STATIC);

  if ((rc = sqlite3_step((*inspector)->stmt)) != SQLITE_DONE)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc,
                                 sqlite3_errmsg((*inspector)->db)));
    return ERR_FAILURE;
  }

  sqlite3_finalize((*inspector)->stmt);

  return ERR_SUCCESS;
}

ERR
select_quarantine_db(INSPECTOR **inspector,
                     int (*callback)(void *, int, char **, char **))
{
  char *sqlite_err_msg = NULL;
  int rc = sqlite3_exec((*inspector)->db, SQL_SELECT_QUARANTINE, callback, NULL,
                        &sqlite_err_msg);
  if (rc != SQLITE_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc, sqlite_err_msg));
    return ERR_FAILURE;
  }
  return ERR_SUCCESS;
}

ERR
select_where_quarantine_db(INSPECTOR **inspector, QUARANTINE_FILES **file)
{
  int rc = sqlite3_prepare((*inspector)->db, SQL_SELECT_WHERE_QUARANTINE, -1,
                           &(*inspector)->stmt, NULL);
  if (rc != SQLITE_OK)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc,
                                 sqlite3_errmsg((*inspector)->db)));
    return ERR_FAILURE;
  }

  sqlite3_bind_text((*inspector)->stmt, 1, (*file)->filepath, -1,
                    SQLITE_STATIC);

  if ((rc = sqlite3_step((*inspector)->stmt)) != SQLITE_ROW)
  {
    LOG_ERROR(LOG_MESSAGE_FORMAT("ERR_FAILURE %d  (%s)", rc,
                                 sqlite3_errmsg((*inspector)->db)));
    return ERR_FAILURE;
  }

  (*file)->filename = (const char *)sqlite3_column_text((*inspector)->stmt, 0);
  (*file)->filepath = (const char *)sqlite3_column_text((*inspector)->stmt, 1);
  (*file)->detected = (const char *)sqlite3_column_text((*inspector)->stmt, 2);
  (*file)->datetime = (const char *)sqlite3_column_text((*inspector)->stmt, 3);

  sqlite3_finalize((*inspector)->stmt);

  return ERR_SUCCESS;
}