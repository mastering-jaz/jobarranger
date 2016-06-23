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
** $Date:: 2013-11-26 09:57:12 +0900 #$
** $Revision: 5457 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "javalue.h"
#include "jastatus.h"
#include "jaflow.h"
#include "jalog.h"
#include "jajobnetrun.h"

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
int jajobnet_run_sync(const zbx_uint64_t inner_jobnet_id,
                      const zbx_uint64_t icon_jobnet_id,
                      const zbx_uint64_t icon_end_id, const int status,
                      const int timeout_flag)
{
    int jobnet_summary_status, jobnet_summary_job_status;
    DB_RESULT result;
    DB_ROW row;
    zbx_uint64_t icon_inner_jobnet_id;
    char exit_code[16];
    const char *__function_name = "jajobnet_run_sync";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobnet_id: " ZBX_FS_UI64 ", icon_jobnet_id: "
               ZBX_FS_UI64 ", icon_end_id: " ZBX_FS_UI64
               ", status: %d, timeout_flag: %d", __function_name,
               inner_jobnet_id, icon_jobnet_id, icon_end_id, status,
               timeout_flag);

    // set jobnet status
    switch (status) {
    case JA_JOBNET_STATUS_RUN:
        if (ZBX_DB_OK >
            DBexecute
            ("update ja_run_jobnet_table set status = %d, timeout_flag = %d"
             " where inner_jobnet_id = " ZBX_FS_UI64, status, timeout_flag,
             inner_jobnet_id))
            return FAIL;
        break;
    case JA_JOBNET_STATUS_END:
        if (ja_set_end_jobnet(inner_jobnet_id) == FAIL)
            return FAIL;
        break;
    case JA_JOBNET_STATUS_RUNERR:
        if (ja_set_runerr_jobnet(inner_jobnet_id) == FAIL)
            return FAIL;
        break;
    case JA_JOBNET_STATUS_ENDERR:
        if (ja_set_enderr_jobnet(inner_jobnet_id) == FAIL)
            return FAIL;
        break;
    default:
        return FAIL;
        break;
    }

    if (icon_jobnet_id == 0) {
        // set jobnet summary status
        jobnet_summary_status = JA_JOBNET_STATUS_RUN;
        jobnet_summary_job_status = JA_SUMMARY_JOB_STATUS_NORMAL;
        switch (status) {
        case JA_JOBNET_STATUS_END:
            jobnet_summary_status = JA_JOBNET_STATUS_END;
            if (timeout_flag == 1)
                jobnet_summary_job_status = JA_SUMMARY_JOB_STATUS_TIMEOUT;
            else
                jobnet_summary_job_status = JA_SUMMARY_JOB_STATUS_NORMAL;
            break;
        case JA_JOBNET_STATUS_RUNERR:
        case JA_JOBNET_STATUS_ENDERR:
            jobnet_summary_job_status = JA_SUMMARY_JOB_STATUS_ERROR;
            break;
        default:
            if (timeout_flag == 1) {
                jobnet_summary_job_status = JA_SUMMARY_JOB_STATUS_TIMEOUT;
            }
            break;
        }
        if (ja_set_jobstatus
            (inner_jobnet_id, jobnet_summary_status,
             jobnet_summary_job_status) == FAIL)
            return FAIL;
    } else {
        // set icon jobnet status
        switch (status) {
        case JA_JOBNET_STATUS_RUN:
            if (ZBX_DB_OK >
                DBexecute
                ("update ja_run_job_table set status = %d, timeout_flag = %d"
                 " where inner_job_id = " ZBX_FS_UI64, status,
                 timeout_flag, icon_jobnet_id))
                return FAIL;
            break;
        case JA_JOBNET_STATUS_END:
            // get inner_jobnet_id used icon_jobnet_id
            icon_inner_jobnet_id = 0;
            result =
                DBselect
                ("select inner_jobnet_id from ja_run_icon_jobnet_table "
                 " where inner_job_id = " ZBX_FS_UI64, icon_jobnet_id);
            if (NULL != (row = DBfetch(result))) {
                ZBX_STR2UINT64(icon_inner_jobnet_id, row[0]);
            }
            DBfree_result(result);

            // set icon jobnet: JOB_EXIT_CD
            if (ja_get_value_after(icon_end_id, "JOB_EXIT_CD", exit_code)
                == FAIL)
                return FAIL;
            if (ja_set_value_after
                (icon_jobnet_id, icon_inner_jobnet_id, "JOB_EXIT_CD",
                 exit_code))
                return FAIL;
            if (ja_flow(icon_jobnet_id, JA_FLOW_TYPE_NORMAL) == FAIL)
                return FAIL;
            break;
        case JA_JOBNET_STATUS_RUNERR:
            if (ja_set_runerr(icon_jobnet_id) == FAIL)
                return FAIL;
            break;
        case JA_JOBNET_STATUS_ENDERR:
            if (ja_set_enderr(icon_jobnet_id) == FAIL)
                return FAIL;
            break;
        default:
            return FAIL;
            break;
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
int jajobnet_run(const zbx_uint64_t inner_jobnet_id,
                 const zbx_uint64_t icon_jobnet_id, const int status,
                 const int timeout_flag)
{
    DB_RESULT result;
    DB_RESULT result3;
    DB_RESULT result4;
    DB_RESULT result5;
    DB_ROW row;
    DB_ROW row3;
    DB_ROW row4;
    DB_ROW row5;
    double sec;
    zbx_uint64_t inner_job_id, icon_end_id;
    int renew_flag, renew_status, renew_timeout_flag;
    int job_status, job_type, test_flag, job_timeout_flag;
    int count, ready_cnt, run_cnt, runerr_cnt, end_cnt, start_icons,
        end_icons, end_icons_end, timeout_cnt, exit_flag, idx, len;
    char *w_main_jobnet_id, *w_execution_user_name;
    char w_inner_job_id[40];
    char w_job_id[3000];
    const char *__function_name = "jajobnet_run";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobnet_id: " ZBX_FS_UI64
               ", icon_jobnet_id: " ZBX_FS_UI64
               ", status: %d, timeout_flag: %d", __function_name,
               inner_jobnet_id, icon_jobnet_id, status, timeout_flag);

    sec = zbx_time();
    result = DBselect("select inner_job_id, status, job_type, test_flag, timeout_flag,"
                      " inner_jobnet_main_id, job_id from ja_run_job_table"
                      " where inner_jobnet_id = " ZBX_FS_UI64, inner_jobnet_id);
    sec = zbx_time() - sec;
    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() ja_run_jobnet_table:(inner_jobnet_id: " ZBX_FS_UI64
               ") " ZBX_FS_DBL " sec.", __function_name, inner_jobnet_id,
               sec);

    icon_end_id = 0;
    test_flag = 0;
    count = 0;
    ready_cnt = 0;
    run_cnt = 0;
    runerr_cnt = 0;
    end_cnt = 0;
    start_icons = 0;
    end_icons = 0;
    end_icons_end = 0;
    timeout_cnt = 0;
    while (NULL != (row = DBfetch(result))) {
        ZBX_STR2UINT64(inner_job_id, row[0]);
        job_status = atoi(row[1]);
        job_type = atoi(row[2]);
        test_flag = atoi(row[3]);
        job_timeout_flag = atoi(row[4]);
        w_main_jobnet_id = "";
        w_execution_user_name = "";

        count++;
        if (job_timeout_flag == 1)
            timeout_cnt++;

        if (job_type == JA_JOB_TYPE_START)
            start_icons++;
        if (job_type == JA_JOB_TYPE_END)
            end_icons++;

        switch (job_status) {
        case JA_JOB_STATUS_BEGIN:
            break;
        case JA_JOB_STATUS_READY:
            ready_cnt++;
            break;
        case JA_JOB_STATUS_RUN:
            run_cnt++;
            break;
        case JA_JOB_STATUS_END:
            end_cnt++;
            if (job_type == JA_JOB_TYPE_END) {
                icon_end_id = inner_job_id;
                end_icons_end++;
            }
            break;
        case JA_JOB_STATUS_RUNERR:
        case JA_JOB_STATUS_ABORT:
            runerr_cnt++;
            if (status == JA_JOBNET_STATUS_RUN) {
                result3 = DBselect("select jobnet_id, execution_user_name from ja_run_jobnet_table"
                                   " where inner_jobnet_id = %s", row[5]);
                if (NULL != (row3 = DBfetch(result3))) {
                    w_main_jobnet_id = row3[0];
                    w_execution_user_name = row3[1];
                }

                /* job id get */
                idx = sizeof(w_job_id) - 1;
                w_job_id[idx] = '\0';
                zbx_strlcpy(w_inner_job_id, row[0], sizeof(w_inner_job_id));

                /* sub jobnet search */
                exit_flag = 0;
                while (exit_flag == 0) {
                    result4 = DBselect("select inner_jobnet_id, inner_jobnet_main_id, job_id"
                                       " from ja_run_job_table"
                                       " where inner_job_id = %s", w_inner_job_id);
                    if (NULL == (row4 = DBfetch(result4))) {
                        DBfree_result(result4);
                        exit_flag = 1;
                        continue;
                    }

                    len = strlen(row4[2]);
                    if (((idx - 1) - len) < 0) {
                        DBfree_result(result4);
                        exit_flag = 1;
                        continue;
                    } else {
                        idx = idx - len;
                        memcpy(&w_job_id[idx], row4[2], len);
                        idx = idx - 1;
                        memcpy(&w_job_id[idx], "/", 1);
                    }

                    if (strcmp(row4[0], row4[1]) == 0) {
                        DBfree_result(result4);
                        exit_flag = 1;
                        continue;
                    }

                    /* get the job id of the jobnet icon */
                    result5 = DBselect("select inner_job_id from ja_run_icon_jobnet_table"
                                       " where link_inner_jobnet_id = %s", row4[0]);
                    if (NULL == (row5 = DBfetch(result5))) {
                        DBfree_result(result4);
                        DBfree_result(result5);
                        exit_flag = 1;
                        continue;
                    }
                    zbx_strlcpy(w_inner_job_id, row5[0], sizeof(w_inner_job_id));
                    DBfree_result(result4);
                    DBfree_result(result5);
                }

                /* jobnet id set */
                len = strlen(w_main_jobnet_id);
                if ((idx - len) >= 0) {
                    idx = idx - len;
                    memcpy(&w_job_id[idx], w_main_jobnet_id, len);
                }

                ja_log("JAJOBNETRUN000001", inner_jobnet_id, NULL, 0,
                       __function_name, inner_job_id, w_main_jobnet_id, &w_job_id[idx],
                       w_execution_user_name);
                DBfree_result(result3);
            }
            break;
        case JA_JOB_STATUS_ENDERR:
            result3 = DBselect("select jobnet_id, execution_user_name from ja_run_jobnet_table"
                               " where inner_jobnet_id = %s", row[5]);
            if (NULL != (row3 = DBfetch(result3))) {
                w_main_jobnet_id = row3[0];
                w_execution_user_name = row3[1];
            }

            /* job id get */
            idx = sizeof(w_job_id) - 1;
            w_job_id[idx] = '\0';
            zbx_strlcpy(w_inner_job_id, row[0], sizeof(w_inner_job_id));

            /* sub jobnet search */
            exit_flag = 0;
            while (exit_flag == 0) {
                result4 = DBselect("select inner_jobnet_id, inner_jobnet_main_id, job_id"
                                   " from ja_run_job_table"
                                   " where inner_job_id = %s", w_inner_job_id);
                if (NULL == (row4 = DBfetch(result4))) {
                    DBfree_result(result4);
                    exit_flag = 1;
                    continue;
                }

                len = strlen(row4[2]);
                if (((idx - 1) - len) < 0) {
                    DBfree_result(result4);
                    exit_flag = 1;
                    continue;
                } else {
                    idx = idx - len;
                    memcpy(&w_job_id[idx], row4[2], len);
                    idx = idx - 1;
                    memcpy(&w_job_id[idx], "/", 1);
                }

                if (strcmp(row4[0], row4[1]) == 0) {
                    DBfree_result(result4);
                    exit_flag = 1;
                    continue;
                }

                /* get the job id of the jobnet icon */
                result5 = DBselect("select inner_job_id from ja_run_icon_jobnet_table"
                                   " where link_inner_jobnet_id = %s", row4[0]);
                if (NULL == (row5 = DBfetch(result5))) {
                    DBfree_result(result4);
                    DBfree_result(result5);
                    exit_flag = 1;
                    continue;
                }
                zbx_strlcpy(w_inner_job_id, row5[0], sizeof(w_inner_job_id));
                DBfree_result(result4);
                DBfree_result(result5);
            }

            /* jobnet id set */
            len = strlen(w_main_jobnet_id);
            if ((idx - len) >= 0) {
                idx = idx - len;
                memcpy(&w_job_id[idx], w_main_jobnet_id, len);
            }

            ja_log("JAJOBNETRUN200001", inner_jobnet_id, NULL, 0,
                   __function_name, inner_job_id, w_main_jobnet_id, &w_job_id[idx],
                   w_execution_user_name);
            DBfree_result(result3);
            goto error;
            break;
        default:
            break;
        }
        if (end_icons_end > 1 && test_flag == 0) {
            ja_log("JAJOBNETRUN200002", inner_jobnet_id, NULL, 0,
                   __function_name, inner_jobnet_id);
            goto error;
        }
    }
    DBfree_result(result);
    result = NULL;

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobnet_id: "
               ZBX_FS_UI64
               ", start_icons: %d, end_icons: %d, end_icons_end: %d,"
               " ready: %d, run: %d, runerr: %d, end: %d, timeout: %d",
               __function_name, inner_jobnet_id, start_icons, end_icons,
               end_icons_end, ready_cnt, run_cnt, runerr_cnt, end_cnt,
               timeout_cnt);

    renew_flag = 0;
    renew_status = status;
    renew_timeout_flag = timeout_flag;
    if (start_icons != 1) {
        ja_log("JAJOBNETRUN200003", inner_jobnet_id, NULL, 0,
               __function_name, inner_jobnet_id);
        goto error;
    }

    if (end_icons == 0) {
        ja_log("JAJOBNETRUN200004", inner_jobnet_id, NULL, 0,
               __function_name, inner_jobnet_id);
        goto error;
    }

    if (test_flag == 0) {
        if (end_icons_end == 0) {
            // reset status
            if (status == JA_JOBNET_STATUS_RUN && runerr_cnt > 0) {
                renew_status = JA_JOBNET_STATUS_RUNERR;
                renew_flag = 1;
            }
            if (status == JA_JOBNET_STATUS_RUNERR && runerr_cnt == 0) {
                renew_status = JA_JOBNET_STATUS_RUN;
                renew_flag = 1;
            }
        } else {
            if (ready_cnt > 0 || run_cnt > 0 || runerr_cnt > 0) {
                ja_log("JAJOBNETRUN200005", inner_jobnet_id, NULL, 0,
                       __function_name, inner_jobnet_id, ready_cnt,
                       run_cnt);
                goto error;
            } else {
                // set status: END
                renew_status = JA_JOBNET_STATUS_END;
                renew_flag = 1;
            }
        }
    } else {
        if (count == end_cnt) {
            renew_status = JA_JOBNET_STATUS_END;
            renew_flag = 1;
        } else {
            // reset status
            if (status == JA_JOBNET_STATUS_RUN && runerr_cnt > 0) {
                renew_status = JA_JOBNET_STATUS_RUNERR;
                renew_flag = 1;
            }
            if (status == JA_JOBNET_STATUS_RUNERR && runerr_cnt == 0) {
                renew_status = JA_JOBNET_STATUS_RUN;
                renew_flag = 1;
            }
        }
    }

    // reset timeout_flag
    if (timeout_flag == 0 && timeout_cnt > 0) {
        renew_timeout_flag = 1;
        renew_flag = 1;
    }
    if (timeout_flag == 1 && timeout_cnt == 0) {
        renew_timeout_flag = 0;
        renew_flag = 1;
    }

    if (renew_flag == 1)
        return jajobnet_run_sync(inner_jobnet_id, icon_jobnet_id,
                                 icon_end_id, renew_status,
                                 renew_timeout_flag);
    return SUCCEED;
  error:
    if (DBfree_result != NULL)
        DBfree_result(result);
    return jajobnet_run_sync(inner_jobnet_id, icon_jobnet_id, icon_end_id,
                             JA_JOBNET_STATUS_ENDERR, 0);
}
