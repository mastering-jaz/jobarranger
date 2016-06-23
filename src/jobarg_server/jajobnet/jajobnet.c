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

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jastatus.h"
#include "jalog.h"
#include "jajoblog.h"
#include "jaself.h"
#include "jajobnetsummaryready.h"
#include "jajobnetkill.h"
#include "jajobnetready.h"
#include "jajobnetrun.h"
#include "jajobnet.h"

extern unsigned char process_type;
extern int process_num;

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
static void process_jajobnet_summary()
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    double sec;
    zbx_uint64_t inner_jobnet_id;
    int status, jobnet_abort_flag;
    const char *__function_name = "process_jajobnet_summary";

    sec = zbx_time();
    result =
        DBselect
        ("select inner_jobnet_id, status, jobnet_abort_flag"
         " from ja_run_jobnet_summary_table where status in (%d, %d)",
         JA_JOBNET_STATUS_READY, JA_JOBNET_STATUS_RUN);
    sec = zbx_time() - sec;
    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() ja_run_jobnet_summary_table(status): " ZBX_FS_DBL
               " sec.", __function_name, sec);

    while (NULL != (row = DBfetch(result))) {
        ZBX_STR2UINT64(inner_jobnet_id, row[0]);
        status = atoi(row[1]);
        jobnet_abort_flag = atoi(row[2]);

        zabbix_log(LOG_LEVEL_DEBUG, "In %s() ja_run_jobnet_summary_table read."
                   " inner_jobnet_id[%s] status[%s] jobnet_abort_flag[%s]",
                   __function_name, row[0], row[1], row[2]);

        ret = SUCCEED;
        DBbegin();
        switch (status) {
        case JA_JOBNET_STATUS_READY:
            ret = jajobnet_summary_ready(inner_jobnet_id);
            break;
        case JA_JOBNET_STATUS_RUN:
            if (jobnet_abort_flag == 1) {
                ret = jajobnet_kill(inner_jobnet_id);
                ja_joblog(JC_JOBNET_ERR_END, inner_jobnet_id, 0);
            }
            break;
        default:
            break;
        }

        if (ret == SUCCEED) {
            DBcommit();
        } else {
            zabbix_log(LOG_LEVEL_ERR, "In %s() rollback", __function_name);
            DBrollback();
        }
    }
    DBfree_result(result);

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
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
static void process_jajobnet()
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    double sec;
    zbx_uint64_t inner_jobnet_id, inner_job_id;
    int timeout_flag, status;
    const char *__function_name = "process_jajobnet";

    sec = zbx_time();
    result =
        DBselect
        ("select inner_jobnet_id, inner_job_id, timeout_flag, status"
         " from ja_run_jobnet_table where status in (%d, %d ,%d)",
         JA_JOBNET_STATUS_READY, JA_JOBNET_STATUS_RUN,
         JA_JOBNET_STATUS_RUNERR);
    sec = zbx_time() - sec;
    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() ja_run_jobnet_table(status): " ZBX_FS_DBL " sec.",
               __function_name, sec);

    while (NULL != (row = DBfetch(result))) {
        ZBX_STR2UINT64(inner_jobnet_id, row[0]);
        ZBX_STR2UINT64(inner_job_id, row[1]);
        timeout_flag = atoi(row[2]);
        status = atoi(row[3]);

        zabbix_log(LOG_LEVEL_DEBUG, "In %s() ja_run_jobnet_table read."
                   " inner_jobnet_id[%s] inner_job_id[%s] timeout_flag[%s] status[%s]",
                   __function_name, row[0], row[1], row[2], row[3]);

        ret = SUCCEED;
        DBbegin();
        switch (status) {
        case JA_JOBNET_STATUS_READY:
            ret = jajobnet_ready(inner_jobnet_id);
            break;
        case JA_JOBNET_STATUS_RUN:
        case JA_JOBNET_STATUS_RUNERR:
            ret = jajobnet_run(inner_jobnet_id, inner_job_id, status, timeout_flag);
            break;
        default:
            break;
        }
        if (ret == SUCCEED) {
            DBcommit();
        } else {
            zabbix_log(LOG_LEVEL_ERR, "In %s() rollback", __function_name);
            DBrollback();
        }
    }
    DBfree_result(result);

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
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
void main_jajobnet_loop()
{
    zabbix_log(LOG_LEVEL_DEBUG,
               "In main_jajobnet_loop() process_type:'%s' process_num:%d",
               ja_get_process_type_string(process_type), process_num);

    zbx_setproctitle("%s [connecting to the database]",
                     ja_get_process_type_string(process_type));

    DBconnect(ZBX_DB_CONNECT_NORMAL);
    for (;;) {
        zbx_setproctitle("%s [processing data]",
                         ja_get_process_type_string(process_type));
        process_jajobnet_summary();
        process_jajobnet();
        ja_sleep_loop(CONFIG_JAJOBNET_INTERVAL);
    }
}
