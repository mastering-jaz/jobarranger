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
** $Date:: 2013-12-13 15:44:50 +0900 #$
** $Revision: 5621 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jastr.h"
#include "jatimeout.h"
#include "jastatus.h"
#include "jalog.h"
#include "../jarun/jaruniconjob.h"
#include "jajobiconjob.h"

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
int jajob_icon_job_abort(zbx_uint64_t inner_job_id)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "jajob_icon_job_abort";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);

    ret = FAIL;
    result =
        DBselect
        ("select stop_flag from ja_run_icon_job_table"
         " where inner_job_id = " ZBX_FS_UI64, inner_job_id);

    if (NULL != (row = DBfetch(result))) {
        if (atoi(row[0]) == 0) {
            ret = jarun_icon_job(inner_job_id, JA_AGENT_METHOD_KILL);
        } else {
            ret = jarun_icon_job(inner_job_id, JA_AGENT_METHOD_ABORT);
        }
    } else {
        ja_log("JAJOBICONJOB200002", 0, NULL, inner_job_id,
               __function_name, inner_job_id);
        ret = ja_set_runerr(inner_job_id);
    }
    DBfree_result(result);
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
int jajob_icon_job_timeout(zbx_uint64_t inner_job_id, char *start_time)
{
    int db_ret;
    zbx_uint64_t inner_jobnet_id;
    char timeout[64];
    DB_RESULT result;
    DB_RESULT result2;
    DB_ROW row;
    DB_ROW row2;
    int exit_flag, idx, len;
    zbx_uint64_t w_inner_job_id;
    char w_main_jobnet_id[70];
    char w_execution_user_name[110];
    char w_job_id[3000];
    const char *__function_name = "jajob_icon_job_timeout";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    result =
        DBselect
        ("select inner_jobnet_id, timeout from ja_run_icon_job_table"
         " where inner_job_id = " ZBX_FS_UI64, inner_job_id);
    if (NULL != (row = DBfetch(result))) {
        ZBX_STR2UINT64(inner_jobnet_id, row[0]);
        zbx_snprintf(timeout, sizeof(timeout), "%s", row[1]);
    } else {
        ja_log("JAJOBICONJOB200002", 0, NULL, inner_job_id,
               __function_name, inner_job_id);
        DBfree_result(result);
        return ja_set_runerr(inner_job_id);
    }
    DBfree_result(result);

    if (ja_timeout_check(timeout, start_time) == FAIL)
        return SUCCEED;

    w_main_jobnet_id[0] = '\0';
    w_execution_user_name[0] = '\0';

    /* main jobnet id and user name get */
    result = DBselect("select inner_jobnet_main_id, job_id from ja_run_job_table"
                      " where inner_job_id = " ZBX_FS_UI64, inner_job_id);
    if (NULL != (row = DBfetch(result))) {
        result2 = DBselect("select jobnet_id, execution_user_name from ja_run_jobnet_table"
                           " where inner_jobnet_id = %s", row[0]);
        if (NULL != (row2 = DBfetch(result2))) {
            zbx_strlcpy(w_main_jobnet_id, row2[0], sizeof(w_main_jobnet_id));
            zbx_strlcpy(w_execution_user_name, row2[1], sizeof(w_execution_user_name));
        }
        DBfree_result(result2);
    }
    DBfree_result(result);

    /* job id get */
    idx = sizeof(w_job_id) - 1;
    w_job_id[idx] = '\0';
    w_inner_job_id = inner_job_id;

    /* sub jobnet search */
    exit_flag = 0;
    while (exit_flag == 0) {
        result = DBselect("select inner_jobnet_id, inner_jobnet_main_id, job_id"
                           " from ja_run_job_table"
                           " where inner_job_id = " ZBX_FS_UI64, w_inner_job_id);
        if (NULL == (row = DBfetch(result))) {
            DBfree_result(result);
            exit_flag = 1;
            continue;
        }

        len = strlen(row[2]);
        if (((idx - 1) - len) < 0) {
            DBfree_result(result);
            exit_flag = 1;
            continue;
        } else {
            idx = idx - len;
            memcpy(&w_job_id[idx], row[2], len);
            idx = idx - 1;
            memcpy(&w_job_id[idx], "/", 1);
        }

        if (strcmp(row[0], row[1]) == 0) {
            DBfree_result(result);
            exit_flag = 1;
            continue;
        }

        /* get the job id of the jobnet icon */
        result2 = DBselect("select inner_job_id from ja_run_icon_jobnet_table"
                           " where link_inner_jobnet_id = %s", row[0]);
        if (NULL == (row2 = DBfetch(result2))) {
            DBfree_result(result);
            DBfree_result(result2);
            exit_flag = 1;
            continue;
        }
        ZBX_STR2UINT64(w_inner_job_id, row2[0]);
        DBfree_result(result);
        DBfree_result(result2);
    }

    /* jobnet id set */
    len = strlen(w_main_jobnet_id);
    if ((idx - len) >= 0 && len > 0) {
        idx = idx - len;
        memcpy(&w_job_id[idx], w_main_jobnet_id, len);
    }

    ja_log("JAJOBICONJOB300001", inner_jobnet_id, NULL, inner_job_id,
           __function_name, inner_job_id, timeout, start_time,
           w_main_jobnet_id, &w_job_id[idx], w_execution_user_name);

    db_ret =
        DBexecute
        ("update ja_run_job_table set timeout_flag = 1 where inner_job_id = "
         ZBX_FS_UI64, inner_job_id);
    if (db_ret < ZBX_DB_OK)
        return FAIL;
    ja_joblog(JC_JOB_TIMEOUT, inner_jobnet_id, inner_job_id);

    return SUCCEED;
}
