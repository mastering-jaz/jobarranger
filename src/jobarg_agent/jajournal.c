/*
** Job Arranger for ZABBIX
** Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.
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
** $Date:: 2012-11-26 13:15:38 +0900 #$
** $Revision: 2725 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include <sqlite3.h>
#include "common.h"
#include "log.h"

#include "jacommon.h"

static char journal_filename[JA_MAX_STRING_LEN];
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
int ja_journal_create(const char *filename)
{
    FILE *fp;
    const char *__function_name = "ja_journal_create";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filename: %s", __function_name,
               filename);
    fp = fopen(filename, "a");
    if (fp == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "Can not open the journal file %s. [%s]",
                   filename, zbx_strerror(errno));
        return FAIL;
    }

    zbx_snprintf(journal_filename, JA_MAX_STRING_LEN, "%s", filename);
    fclose(fp);

    return SUCCEED;
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
int ja_journal_save(const char *str)
{
    int ret;
    FILE *fp;

    ret = SUCCEED;
    fp = fopen(journal_filename, "a");
    if (fp == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "Can not open the journal file %s. [%s]",
                   journal_filename, zbx_strerror(errno));
        return FAIL;
    }

    if (fprintf
        (fp, ZBX_FS_UI64 " %d %s\n", (zbx_uint64_t) time(NULL),
         (int) strlen(str), str)
        < 0) {
        zabbix_log(LOG_LEVEL_ERR, "Can not write journal file %s. [%s]",
                   journal_filename, zbx_strerror(errno));
        ret = FAIL;
    }

    if (fp != NULL)
        fclose(fp);
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
int ja_journal_vacate()
{
    FILE *fp;
    const char *__function_name = "ja_journal_vacate";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filename: %s", __function_name,
               journal_filename);

    fp = fopen(journal_filename, "w");
    if (fp == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "Can not open the journal file %s. [%s]",
                   journal_filename, zbx_strerror(errno));
        return FAIL;
    }
    fclose(fp);
    return SUCCEED;
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
int ja_journal_load(sqlite3 * conn)
{
    FILE *fp;
    int ret, size, err;
    char sql[JA_MAX_DATA_LEN];
    char *error;
    const char *__function_name = "ja_journal_load";

    zabbix_log(LOG_LEVEL_WARNING, "In %s()", __function_name);

    if (conn == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() database is not connected",
                   __function_name);
        return FAIL;
    }

    fp = fopen(journal_filename, "r");
    if (fp == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "Can not open journal file %s [%s]",
                   journal_filename, zbx_strerror(errno));
        return FAIL;
    }

    ret = SUCCEED;
    while (1) {
        memset(sql, 0, sizeof(sql));
        if (fscanf(fp, "%*s %d ", &size) != 1)
            break;
        if (fread(sql, sizeof(char), size, fp) == EOF)
            break;
        err = sqlite3_exec(conn, sql, NULL, 0, &error);
        if (err != SQLITE_OK) {
            zabbix_log(LOG_LEVEL_ERR, "query failed: [%s] error [%s]", sql,
                       error);
            sqlite3_free(error);
            ret = FAIL;
            break;
        }
        sqlite3_free(error);
    }

    if (fp != NULL)
        fclose(fp);
    return ret;
}
