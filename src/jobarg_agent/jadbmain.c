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
#include "log.h"

#include "jacommon.h"
#include "jasqlite.h"
#include "jajournal.h"
#include "jadbmain.h"

static char dbbackup_filename[JA_MAX_STRING_LEN];
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
int ja_dbmain_create(const char *filename)
{
    int ret;
    sqlite3 *conn;
    const char *__function_name = "ja_dbmain_create";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filename: %s", __function_name,
               filename);
    ret = FAIL;
    if (SQLITE_OK == sqlite3_open(filename, &conn)) {
        zbx_snprintf(dbbackup_filename, JA_MAX_STRING_LEN, "%s", filename);
        ret = SUCCEED;
    } else {
        zabbix_log(LOG_LEVEL_ERR,
                   "Can not open the database backup filename: %s",
                   __function_name, filename);
        ret = FAIL;
    }
    sqlite3_close(conn);

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
int ja_dbmain_restore()
{
    int ret;
    sqlite3 *conn;
    char dbtemp[JA_MAX_STRING_LEN];
    const char *__function_name = "ja_dbmain_restore";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    zbx_snprintf(dbtemp, sizeof(dbtemp), "%s.%ld",
                 CONFIG_DATABASE_FILE, time(NULL));
    zabbix_log(LOG_LEVEL_WARNING, "In %s() rename %s to %s",
               __function_name, CONFIG_DATABASE_FILE, dbtemp);
    if (rename(CONFIG_DATABASE_FILE, dbtemp) != 0) {
        zabbix_log(LOG_LEVEL_CRIT, "Can not rename %s to %s. [%s]",
                   CONFIG_DATABASE_FILE, dbtemp, zbx_strerror(errno));
        return FAIL;
    }

    ret = FAIL;
    if (SQLITE_OK != sqlite3_open(CONFIG_DATABASE_FILE, &conn)) {
        zabbix_log(LOG_LEVEL_ERR, "connection to database '%s' failed",
                   sqlite3_errmsg(conn));
        goto exit;
    }

    if (ja_dbmain_recovery(conn) != SUCCEED)
        goto exit;
    if (ja_journal_load(conn) != SUCCEED)
        goto exit;

    ret = SUCCEED;

  exit:
    sqlite3_close(conn);
    /*
       if (ret == SUCCEED) {
       zabbix_log(LOG_LEVEL_WARNING, "In %s() remove %s", __function_name,
       dbtemp);
       remove(dbtemp);
       } else {
       rename(dbtemp, CONFIG_DATABASE_FILE);
       }
     */

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
int ja_dbmain_recovery(sqlite3 * conn)
{
    int ret;
    sqlite3 *conn_bk;
    const char *__function_name = "ja_dbmain_recovery";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    if (conn == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() database is not connected",
                   __function_name);
        return FAIL;
    }

    ret = FAIL;
    zabbix_log(LOG_LEVEL_DEBUG, "In %s() database backup file: %s",
               __function_name, dbbackup_filename);

    if (SQLITE_OK != sqlite3_open(dbbackup_filename, &conn_bk)) {
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() can not connect the database backup file %s [%s]",
                   dbbackup_filename, sqlite3_errmsg(conn_bk));
        sqlite3_close(conn_bk);
        return FAIL;
    }

    ret = ja_dbmain_mem_backup(conn_bk, conn);
    sqlite3_close(conn_bk);
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
int ja_dbmain_mem_backup(sqlite3 * conn_src, sqlite3 * conn_dest)
{
    sqlite3_backup *pBackup;
    const char *__function_name = "ja_dbmain_mem_backup";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    pBackup = sqlite3_backup_init(conn_dest, "main", conn_src, "main");
    if (pBackup) {
        (void) sqlite3_backup_step(pBackup, -1);
        (void) sqlite3_backup_finish(pBackup);
        return SUCCEED;
    } else {
        zabbix_log(LOG_LEVEL_ERR, "Can not backup database. [%s]",
                   sqlite3_errmsg(conn_dest));
        return FAIL;
    }

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
int ja_dbmain_backup()
{
    int ret;
    sqlite3 *conn, *conn_bk;
    const char *__function_name = "ja_dbmain_backup";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    conn = ja_db_getconn();
    if (conn == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() database is not connected",
                   __function_name);
        return FAIL;
    }
    if (SQLITE_OK != sqlite3_open(dbbackup_filename, &conn_bk)) {
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() can not connect the database backup file %s [%s]",
                   dbbackup_filename, sqlite3_errmsg(conn_bk));
        sqlite3_close(conn_bk);
        return FAIL;
    }
    ret = ja_dbmain_mem_backup(conn, conn_bk);
    if (ret == SUCCEED)
        ret = ja_journal_vacate();
    sqlite3_close(conn_bk);

    if (ret == SUCCEED)
        ja_dbmain_remove_oldjob(CONFIG_JOB_HISTORY);

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
int ja_dbmain_remove_oldjob(int day)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    time_t history;
    const char *__function_name = "ja_jobmain_remove_oldjob";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() day: %d", __function_name, day);

    history = time(NULL) - (day * 24 * 60 * 60);

    ret = SUCCEED;
    if (ja_db_begin() == FAIL)
        return FAIL;

    result =
        ja_db_select
        ("select jobid from jobs where status = %d and end_time < "
         ZBX_FS_UI64, JA_AGENT_STATUS_CLOSE, history);
    while ((row = ja_db_fetch(result)) != NULL) {
        zabbix_log(LOG_LEVEL_INFORMATION, "In %s() delete jobid = %s",
                   __function_name, row[0]);
        ret = ja_db_execute("delete from jobs where jobid = %s", row[0]);
        if (ret == FAIL) {
            zabbix_log(LOG_LEVEL_ERR, "can not delete jobid = %s", row[0]);
            break;
        }
    }
    if (ret == SUCCEED) {
        ja_db_commit();
        ja_db_execute("VACUUM;");
    } else {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() can not delete old jobs %d day ago",
                   __function_name, day);
        ja_db_rollback();
    }
    ja_db_free_result(result);

    return ret;
}
