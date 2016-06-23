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
** $Date:: 2013-05-16 16:03:58 +0900 #$
** $Revision: 4633 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jalog.h"
#include "jajobiconjob.h"
#include "jajobiconextjob.h"
#include "../jarun/jaruniconreboot.h"
#include "jajob.h"

extern unsigned char process_type;
extern int process_num;

static int timeout_cnt = 0;
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
static void process_jajob()
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    double sec;
    zbx_uint64_t inner_job_id, inner_jobnet_id;
    int job_type, method_flag, timeout_flag;
    char *start_time;
    const char *__function_name = "process_jajob";

    sec = zbx_time();
    result =
        DBselect
        ("select inner_job_id, inner_jobnet_id, job_type, method_flag, timeout_flag, start_time"
         " from ja_run_job_table where status = %d", JA_JOB_STATUS_RUN);
    sec = zbx_time() - sec;
    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() ja_run_job_table(status: RUN): " ZBX_FS_DBL
               " sec.", __function_name, sec);

    while (NULL != (row = DBfetch(result))) {
        ZBX_STR2UINT64(inner_job_id, row[0]);
        ZBX_STR2UINT64(inner_jobnet_id, row[1]);
        job_type = atoi(row[2]);
        method_flag = atoi(row[3]);
        timeout_flag = atoi(row[4]);
        start_time = row[5];
        zabbix_log(LOG_LEVEL_DEBUG,
                   "inner_job_id: " ZBX_FS_UI64
                   ", method_flag: %d, job_type: %d",
                   inner_job_id, method_flag, job_type);

        ret = SUCCEED;
        DBbegin();
        DBfree_result(DBselect
                      ("select inner_job_id from ja_run_job_table where inner_job_id = "
                       ZBX_FS_UI64 " for update", inner_job_id));
        switch (job_type) {
        case JA_JOB_TYPE_JOB:
            if (method_flag == JA_JOB_METHOD_ABORT) {
                jajob_icon_job_abort(inner_job_id);
            }
            if (timeout_flag == 0 && timeout_cnt == 0) {
                jajob_icon_job_timeout(inner_job_id, start_time);
            }
            break;
        case JA_JOB_TYPE_EXTJOB:
            if (method_flag == JA_JOB_METHOD_ABORT) {
                jajob_icon_extjob_kill(inner_job_id);
            }
            ret =
                jajob_icon_extjob(inner_job_id, inner_jobnet_id,
                                  method_flag == JA_JOB_METHOD_ABORT);
            break;
        case JA_JOB_TYPE_REBOOT:
            if (method_flag == JA_JOB_METHOD_ABORT) {
                jarun_icon_reboot(inner_job_id, JA_AGENT_METHOD_KILL);
            }
            break;
        default:
            break;
        }

        if (ret == SUCCEED) {
            DBcommit();
        } else {
            ja_log("JAJOB300001", inner_jobnet_id, NULL, 0,
                   __function_name, inner_job_id, job_type);
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
void main_jajob_loop()
{
    zabbix_log(LOG_LEVEL_DEBUG,
               "In main_jajob_loop() process_type:'%s' process_num:%d",
               ja_get_process_type_string(process_type), process_num);

    zbx_setproctitle("%s [connecting to the database]",
                     ja_get_process_type_string(process_type));

    DBconnect(ZBX_DB_CONNECT_NORMAL);
    for (;;) {
        zbx_setproctitle("%s [processing data]",
                         ja_get_process_type_string(process_type));
        process_jajob();
        ja_sleep_loop(CONFIG_JAJOB_INTERVAL);
        if (timeout_cnt >= CONFIG_JA_JOB_TIMEOUT)
            timeout_cnt = 0;
        else
            timeout_cnt += CONFIG_JAJOB_INTERVAL;
    }
}
