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
** $Date:: 2014-04-25 14:25:43 +0900 #$
** $Revision: 5924 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jastr.h"
#include "jajoblog.h"
#include "javalue.h"
#include "jastatus.h"

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
int ja_set_jobstatus(const zbx_uint64_t inner_jobnet_id, const int status,
                     const int jobstatus)
{
    int rc;
    char *ts;
    char str_end[50];
    const char *__function_name = "ja_set_jobstatus";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobnet_id: " ZBX_FS_UI64
               ", status: %d, jobstatus: %d",
               __function_name, inner_jobnet_id, status, jobstatus);

    ts = ja_timestamp2str(time(NULL));
    if (status == JA_JOBNET_STATUS_END || status == JA_JOBNET_STATUS_ENDERR) {
        rc = DBexecute("delete from ja_session_table where inner_jobnet_main_id = " ZBX_FS_UI64,
                       inner_jobnet_id);
        if (rc < ZBX_DB_OK) {
            return FAIL;
        }
        zbx_snprintf(str_end, sizeof(str_end), ", end_time = %s", ts);
    }
    else {
        zbx_snprintf(str_end, sizeof(str_end), "");
    }

    DBfree_result(DBselect
                  ("select status from ja_run_jobnet_summary_table where inner_jobnet_id = "
                   ZBX_FS_UI64 " for update", inner_jobnet_id));

    if (ZBX_DB_OK >
        DBexecute
        ("update ja_run_jobnet_summary_table set status = %d, job_status = %d %s"
         " where inner_jobnet_id = " ZBX_FS_UI64, status, jobstatus,
         str_end, inner_jobnet_id)) {
        return FAIL;
    }

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
int ja_set_status_jobnet_summary(const zbx_uint64_t inner_jobnet_id,
                                 const int status, const int start,
                                 const int end)
{
    int rc;
    char *ts;
    char str_start[50];
    char str_end[50];
    const char *__function_name = "ja_set_status_jobnet_summary";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobnet_id: " ZBX_FS_UI64 ", status: %d",
               __function_name, inner_jobnet_id, status);

    ts = ja_timestamp2str(time(NULL));
    if (start == 0)
        zbx_snprintf(str_start, sizeof(str_start), ", start_time = 0");
    else if (start == 1)
        zbx_snprintf(str_start, sizeof(str_start), ", start_time = %s",
                     ts);
    else
        zbx_snprintf(str_start, sizeof(str_start), "");

    if (end == 0)
        zbx_snprintf(str_end, sizeof(str_end), ", end_time = 0");
    else if (end == 1)
        zbx_snprintf(str_end, sizeof(str_end), ", end_time = %s", ts);
    else
        zbx_snprintf(str_end, sizeof(str_end), "");

    DBfree_result(DBselect
                  ("select status from ja_run_jobnet_summary_table where inner_jobnet_id = "
                   ZBX_FS_UI64 " for update", inner_jobnet_id));

    if (ZBX_DB_OK >
        DBexecute
        ("update ja_run_jobnet_summary_table set status = %d %s %s"
         " where inner_jobnet_id = " ZBX_FS_UI64, status, str_start,
         str_end, inner_jobnet_id)) {
        return FAIL;
    }

    if (status == JA_JOBNET_STATUS_END || status == JA_JOBNET_STATUS_ENDERR) {
        rc = DBexecute("delete from ja_session_table where inner_jobnet_main_id = " ZBX_FS_UI64,
                       inner_jobnet_id);
        if (rc < ZBX_DB_OK) {
            return FAIL;
        }
    }

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
int ja_set_status_jobnet(const zbx_uint64_t inner_jobnet_id,
                         const int status, const int start, const int end)
{
    char *ts;
    char str_start[50];
    char str_end[50];
    const char *__function_name = "ja_set_status_jobnet";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobnet_id: " ZBX_FS_UI64 ", status: %d",
               __function_name, inner_jobnet_id, status);

    ts = ja_timestamp2str(time(NULL));
    if (start == 0)
        zbx_snprintf(str_start, sizeof(str_start), ", start_time = 0");
    else if (start == 1)
        zbx_snprintf(str_start, sizeof(str_start), ", start_time = %s",
                     ts);
    else
        zbx_snprintf(str_start, sizeof(str_start), "");

    if (end == 0)
        zbx_snprintf(str_end, sizeof(str_end), ", end_time = 0");
    else if (end == 1)
        zbx_snprintf(str_end, sizeof(str_end), ", end_time = %s", ts);
    else
        zbx_snprintf(str_end, sizeof(str_end), "");

    DBfree_result(DBselect
                  ("select status from ja_run_jobnet_table where inner_jobnet_id = "
                   ZBX_FS_UI64 " for update", inner_jobnet_id));

    if (ZBX_DB_OK >
        DBexecute
        ("update ja_run_jobnet_table set status = %d %s %s"
         " where inner_jobnet_id = " ZBX_FS_UI64, status, str_start,
         str_end, inner_jobnet_id)) {
        return FAIL;
    }

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
int ja_set_status_job(const zbx_uint64_t inner_job_id,
                      const int status, const int start, const int end)
{
    char *ts;
    char str_start[50];
    char str_end[50];
    const char *__function_name = "ja_set_status_job";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64 ", status: %d",
               __function_name, inner_job_id, status);

    ts = ja_timestamp2str(time(NULL));
    if (start == 0)
        zbx_snprintf(str_start, sizeof(str_start), ", start_time = 0");
    else if (start == 1)
        zbx_snprintf(str_start, sizeof(str_start), ", start_time = %s",
                     ts);
    else
        zbx_snprintf(str_start, sizeof(str_start), "");

    if (end == 0)
        zbx_snprintf(str_end, sizeof(str_end), ", end_time = 0");
    else if (end == 1)
        zbx_snprintf(str_end, sizeof(str_end), ", end_time = %s", ts);
    else
        zbx_snprintf(str_end, sizeof(str_end), "");

    DBfree_result(DBselect
                  ("select status from ja_run_job_table where inner_job_id = "
                   ZBX_FS_UI64 " for update", inner_job_id));

    if (ZBX_DB_OK >
        DBexecute
        ("update ja_run_job_table set status = %d %s %s"
         " where inner_job_id = " ZBX_FS_UI64, status, str_start, str_end,
         inner_job_id)) {
        return FAIL;
    }

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
int ja_get_status_jobnet_summary(const zbx_uint64_t inner_jobnet_id)
{
    DB_RESULT result;
    DB_ROW row;
    int status;
    const char *__function_name = "ja_get_status_jobnet_summary";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_jobnet_id: " ZBX_FS_UI64,
               __function_name, inner_jobnet_id);

    status = -1;
    result =
        DBselect
        ("select status from ja_run_jobnet_summary_table"
         " where inner_jobnet_id = " ZBX_FS_UI64, inner_jobnet_id);

    if (NULL != (row = DBfetch(result))) {
        status = atoi(row[0]);
    }
    DBfree_result(result);
    return status;
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
int ja_get_status_jobnet(const zbx_uint64_t inner_jobnet_id)
{
    DB_RESULT result;
    DB_ROW row;
    int status;
    const char *__function_name = "ja_get_status_jobnet";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_jobnet_id: " ZBX_FS_UI64,
               __function_name, inner_jobnet_id);

    status = -1;
    result =
        DBselect
        ("select status from ja_run_jobnet_table"
         " where inner_jobnet_id = " ZBX_FS_UI64, inner_jobnet_id);

    if (NULL != (row = DBfetch(result))) {
        status = atoi(row[0]);
    }
    DBfree_result(result);
    return status;
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
int ja_get_status_job(const zbx_uint64_t inner_job_id)
{
    DB_RESULT result;
    DB_ROW row;
    int status;
    const char *__function_name = "ja_get_status_job";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    status = -1;
    result =
        DBselect
        ("select status from ja_run_job_table"
         " where inner_job_id = " ZBX_FS_UI64, inner_job_id);

    if (NULL != (row = DBfetch(result))) {
        status = atoi(row[0]);
    }
    DBfree_result(result);
    return status;
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
int ja_set_run_jobnet(const zbx_uint64_t inner_jobnet_id)
{
    const char *__function_name = "ja_set_run_jobnet";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_jobnet_id: " ZBX_FS_UI64,
               __function_name, inner_jobnet_id);

    if (ja_clean_value_jobnet_before(inner_jobnet_id) == FAIL)
        return FAIL;

    if (ja_joblog(JC_JOBNET_START, inner_jobnet_id, 0) == FAIL)
        return FAIL;

    return ja_set_status_jobnet(inner_jobnet_id, JA_JOBNET_STATUS_RUN, 1, 0);
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
int ja_set_end_jobnet(const zbx_uint64_t inner_jobnet_id)
{
    const char *__function_name = "ja_set_run_jobnet";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_jobnet_id: " ZBX_FS_UI64,
               __function_name, inner_jobnet_id);

    if (ja_clean_value_jobnet_after(inner_jobnet_id) == FAIL)
        return FAIL;

    if (ja_joblog(JC_JOBNET_END, inner_jobnet_id, 0) == FAIL)
        return FAIL;

    return ja_set_status_jobnet(inner_jobnet_id, JA_JOBNET_STATUS_END, -1, 1);
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
int ja_set_runerr_jobnet(const zbx_uint64_t inner_jobnet_id)
{
    const char *__function_name = "ja_set_runerr_jobnet";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_jobnet_id: " ZBX_FS_UI64,
               __function_name, inner_jobnet_id);

    if (ja_clean_value_jobnet_after(inner_jobnet_id) == FAIL)
        return FAIL;

    //if (ja_joblog(JC_JOBNET_ERR_END, inner_jobnet_id, 0) == FAIL)
    //    return FAIL;

    return ja_set_status_jobnet(inner_jobnet_id, JA_JOBNET_STATUS_RUNERR, -1, 1);
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
int ja_set_enderr_jobnet(const zbx_uint64_t inner_jobnet_id)
{
    const char *__function_name = "ja_set_enderr_jobnet";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_jobnet_id: " ZBX_FS_UI64,
               __function_name, inner_jobnet_id);

    if (ja_clean_value_jobnet_after(inner_jobnet_id) == FAIL)
        return FAIL;

    return ja_set_status_jobnet(inner_jobnet_id, JA_JOBNET_STATUS_ENDERR, -1, 1);
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
int ja_set_run(const zbx_uint64_t inner_job_id)
{
    const char *__function_name = "ja_set_run";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    if (ja_clean_value_before(inner_job_id) == FAIL)
        return FAIL;

    //if (ja_joblog(JC_JOB_START, 0, inner_job_id) == FAIL)
    //    return FAIL;

    return ja_set_status_job(inner_job_id, JA_JOB_STATUS_RUN, 1, 0);
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
int ja_set_end(const zbx_uint64_t inner_job_id)
{
    const char *__function_name = "ja_set_end";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    if (ja_clean_value_after(inner_job_id) == FAIL)
        return FAIL;

    if (ja_joblog(JC_JOB_END, 0, inner_job_id) == FAIL)
        return FAIL;

    return ja_set_status_job(inner_job_id, JA_JOB_STATUS_END, -1, 1);
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
int ja_set_runerr(const zbx_uint64_t inner_job_id)
{
    DB_RESULT result;
    DB_RESULT result2;
    DB_ROW row;
    DB_ROW row2;
    zbx_uint64_t inner_jobnet_main_id;
    zbx_uint64_t w_inner_job_id;
    int exit_flag, idx, len, sw;
    char w_main_jobnet_id[JA_JOBNET_ID_LEN];
    char w_execution_user_name[JA_USER_NAME_LEN];
    char w_job_id[JA_MAX_DATA_LEN];
    const char *__function_name = "ja_set_runerr";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    if (ja_clean_value_after(inner_job_id) == FAIL)
        return FAIL;

    if (ja_joblog(JC_JOB_ERR_END, 0, inner_job_id) == FAIL)
        return FAIL;

    /* print the job stop error message */
    inner_jobnet_main_id = 0;
    w_main_jobnet_id[0] = '\0';
    w_execution_user_name[0] = '\0';

    /* job id get */
    idx = sizeof(w_job_id) - 1;
    w_job_id[idx] = '\0';
    w_inner_job_id = inner_job_id;

    /* sub jobnet search */
    sw = 0;
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

        if (sw == 0) {
            ZBX_STR2UINT64(inner_jobnet_main_id, row[1]);
            sw = 1;
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

    /* jobnet id and user name get */
    if (inner_jobnet_main_id != 0) {
        result = DBselect("select jobnet_id, execution_user_name from ja_run_jobnet_table"
                          " where inner_jobnet_id = " ZBX_FS_UI64, inner_jobnet_main_id);
        if (NULL != (row = DBfetch(result))) {
            zbx_strlcpy(w_main_jobnet_id, row[0], sizeof(w_main_jobnet_id));
            zbx_strlcpy(w_execution_user_name, row[1], sizeof(w_execution_user_name));
        }
        DBfree_result(result);

        /* jobnet id set */
        len = strlen(w_main_jobnet_id);
        if ((idx - len) >= 0) {
            idx = idx - len;
            memcpy(&w_job_id[idx], w_main_jobnet_id, len);
        }
    }

    ja_log("JAJOBNETRUN000001", 0, NULL, inner_job_id,
           __function_name, inner_job_id, w_main_jobnet_id, &w_job_id[idx], w_execution_user_name);

    return ja_set_status_job(inner_job_id, JA_JOB_STATUS_RUNERR, -1, 1);
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
int ja_set_enderr(const zbx_uint64_t inner_job_id, int msg_flag)
{
    DB_RESULT result;
    DB_RESULT result2;
    DB_ROW row;
    DB_ROW row2;
    zbx_uint64_t inner_jobnet_main_id;
    zbx_uint64_t w_inner_job_id;
    int exit_flag, idx, len, sw;
    char w_main_jobnet_id[JA_JOBNET_ID_LEN];
    char w_execution_user_name[JA_USER_NAME_LEN];
    char w_job_id[JA_MAX_DATA_LEN];
    const char *__function_name = "ja_set_enderr";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    if (ja_clean_value_after(inner_job_id) == FAIL)
        return FAIL;

    if (msg_flag == 1) {
        if (ja_joblog(JC_JOB_ERR_END, 0, inner_job_id) == FAIL)
            return FAIL;

        /* print the job stop error message */
        inner_jobnet_main_id = 0;
        w_main_jobnet_id[0] = '\0';
        w_execution_user_name[0] = '\0';

        /* job id get */
        idx = sizeof(w_job_id) - 1;
        w_job_id[idx] = '\0';
        w_inner_job_id = inner_job_id;

        /* sub jobnet search */
        sw = 0;
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

            if (sw == 0) {
                ZBX_STR2UINT64(inner_jobnet_main_id, row[1]);
                sw = 1;
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

        /* jobnet id and user name get */
        if (inner_jobnet_main_id != 0) {
            result = DBselect("select jobnet_id, execution_user_name from ja_run_jobnet_table"
                              " where inner_jobnet_id = " ZBX_FS_UI64, inner_jobnet_main_id);
            if (NULL != (row = DBfetch(result))) {
                zbx_strlcpy(w_main_jobnet_id, row[0], sizeof(w_main_jobnet_id));
                zbx_strlcpy(w_execution_user_name, row[1], sizeof(w_execution_user_name));
            }
            DBfree_result(result);

            /* jobnet id set */
            len = strlen(w_main_jobnet_id);
            if ((idx - len) >= 0) {
                idx = idx - len;
                memcpy(&w_job_id[idx], w_main_jobnet_id, len);
            }
        }

        ja_log("JAJOBNETRUN200001", 0, NULL, inner_job_id,
               __function_name, inner_job_id, w_main_jobnet_id, &w_job_id[idx], w_execution_user_name);
    }

    return ja_set_status_job(inner_job_id, JA_JOB_STATUS_ENDERR, -1, 1);
}
