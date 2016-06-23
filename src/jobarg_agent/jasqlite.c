/*
** Job Arranger for ZABBIX
** Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.
** Copyright (C) 2013 Daiwa Institute of Research Business Innovation Ltd. All Rights Reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

/*
** $Date:: 2014-10-17 16:00:02 +0900 #$
** $Revision: 6528 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include <sqlite3.h>
#include "common.h"
#include "threads.h"
#include "log.h"

#include "jacommon.h"
#include "jajournal.h"
#include "jasqlite.h"

const char *SQL_BEGIN = "BEGIN IMMEDIATE;";
const char *SQL_COMMIT = "COMMIT;";
const char *SQL_ROLLBACK = "ROLLBACK;";
const int MAX_DB_ERR = 30;

static sqlite3 *conn = NULL;

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
sqlite3 *ja_db_getconn()
{
    return conn;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int ja_db_connect(const char *filename)
{
    zabbix_log(LOG_LEVEL_INFORMATION, "open the database. filename: %s",
               filename);

    if (SQLITE_OK == sqlite3_open(filename, &conn))
        return SUCCEED;

    zabbix_log(LOG_LEVEL_ERR, "connection to database '%s' failed",
               sqlite3_errmsg(conn));
    ja_db_close();
    return FAIL;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
void ja_db_close()
{
    zabbix_log(LOG_LEVEL_INFORMATION, "close the database");
    sqlite3_close(conn);
    conn = NULL;
    return;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int ja_db_begin()
{
    int i, ret;
    for (i = 0; i < MAX_DB_ERR; i++) {
        ret = ja_db_execute("%s", SQL_BEGIN);
        if (ret == SUCCEED)
            break;
        zbx_sleep(1);
    }
    return ret;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int ja_db_commit()
{
    int i, ret;
    for (i = 0; i < MAX_DB_ERR; i++) {
        ret = ja_db_execute("%s", SQL_COMMIT);
        if (ret == SUCCEED)
            break;
        zbx_sleep(1);
    }
    return ret;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int ja_db_rollback()
{
    int i, ret;
    for (i = 0; i < MAX_DB_ERR; i++) {
        ret = ja_db_execute("%s", SQL_ROLLBACK);
        if (ret == SUCCEED)
            break;
        zbx_sleep(1);
    }
    return ret;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int __ja_jobarg_db_execute(const char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = ja_db_vexecute(fmt, args);
    va_end(args);

    return ret;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int ja_db_vexecute(const char *fmt, va_list args)
{
    int err, ret;
    char *error, *sql;

    ret = SUCCEED;
    error = NULL;
    sql = NULL;

    if (conn == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "database is not connected");
        return FAIL;
    }

    sql = zbx_dvsprintf(sql, fmt, args);
    zabbix_log(LOG_LEVEL_DEBUG, "query [%s]", sql);
    while (1) {
        err = sqlite3_exec(conn, sql, NULL, 0, &error);
        if (err == SQLITE_OK)
            break;
        if (err != SQLITE_BUSY) {
            zabbix_log(LOG_LEVEL_ERR, "query failed: [%s] error [%s]", sql,
                       error);
            ret = FAIL;
            break;
        }
        zabbix_log(LOG_LEVEL_WARNING, "the database is busy.");
        zbx_sleep(1);
        sqlite3_free(error);
    }

    if (ret == SUCCEED) {
        if (ja_journal_save(sql) == FAIL) {
            zabbix_log(LOG_LEVEL_ERR, "journal save [%s] failed.", sql);
            ret = FAIL;
        }
    }

    zbx_free(sql);
    sqlite3_free(error);
    return ret;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
DB_RESULT __ja_jobarg_db_select(const char *fmt, ...)
{
    va_list args;
    DB_RESULT result;

    va_start(args, fmt);
    result = ja_db_vselect(fmt, args);
    va_end(args);

    return result;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
DB_RESULT ja_db_vselect(const char *fmt, va_list args)
{
    int err;
    char *error, *sql;
    DB_RESULT result;

    error = NULL;
    sql = NULL;
    result = NULL;

    if (conn == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "database is not connected");
        return NULL;
    }

    result = (DB_RESULT) zbx_malloc(NULL, sizeof(SQ_DB_RESULT));
    result->curow = 0;

    sql = zbx_dvsprintf(sql, fmt, args);
    zabbix_log(LOG_LEVEL_DEBUG, "query [%s]", sql);
    while (1) {
        err =
            sqlite3_get_table(conn, sql, &result->data, &result->nrow,
                              &result->ncolumn, &error);
        if (err == SQLITE_OK)
            break;
        if (err != SQLITE_BUSY) {
            zabbix_log(LOG_LEVEL_WARNING,
                       "query failed: [%64s] error [%s]", sql, error);
            if (result->data)
                sqlite3_free_table(result->data);
            zbx_free(result);
            result = NULL;
            break;
        }
        sqlite3_free(error);
    }

    zbx_free(sql);
    sqlite3_free(error);
    return result;
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
DB_ROW ja_db_fetch(DB_RESULT result)
{
    if (result->curow >= result->nrow)
        return NULL;
    if (NULL == result->data)
        return NULL;

    result->curow++;
    return &(result->data[result->curow * result->ncolumn]);
}

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
void ja_db_free_result(DB_RESULT result)
{
    if (NULL == result)
        return;
    if (NULL != result->data)
        sqlite3_free_table(result->data);

    zbx_free(result);
}
