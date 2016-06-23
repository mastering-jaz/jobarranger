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
** $Date:: 2014-11-07 17:13:13 +0900 #$
** $Revision: 6638 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include <json.h>
#include "common.h"
#include "log.h"
#include "threads.h"

#include "jacommon.h"
#include "jajobobject.h"
#include "jafile.h"
#include "jasqlite.h"
#include "jajobdb.h"
#include "jakill.h"
#include "jareboot.h"

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
void ja_reboot_killall()
{
    DB_RESULT result;
    DB_ROW row;
    JA_PID pid;
    const char *__function_name = "ja_reboot_killall";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    result =
        ja_db_select
        ("select jobid, pid from jobs where status = %d and type != '%s'",
         JA_AGENT_STATUS_RUN, JA_PROTO_VALUE_REBOOT);
    while (NULL != (row = ja_db_fetch(result))) {
        pid = atoi(row[1]);
        zabbix_log(LOG_LEVEL_INFORMATION,
                   "In %s() kill pid: %d, jobid: %s", __function_name, pid,
                   row[0]);
        ja_kill_ppid(pid);
    }
    ja_db_free_result(result);
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
int ja_reboot_load_arg(ja_job_object * job, ja_reboot_arg * arg)
{
    int ret;
    json_object *jp_arg, *jp_reboot_mode_flag, *jp_reboot_wait_time;
    const char *__function_name = "ja_reboot_load_arg";

    if (job == NULL || arg == NULL)
        return FAIL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64,
               __function_name, job->jobid);

    ret = FAIL;
    jp_arg = json_tokener_parse(job->argument);
    if (is_error(jp_arg)) {
        jp_arg = NULL;
        zbx_snprintf(job->message, sizeof(job->message),
                     "Can not parse job argument [%s]", job->argument);
        goto error;
    }

    jp_reboot_mode_flag =
        json_object_object_get(jp_arg, JA_PROTO_TAG_REBOOT_MODE);
    if (jp_reboot_mode_flag == NULL) {
        zbx_snprintf(job->message, sizeof(job->message),
                     "Can not get reboot_mode_flag from job argument [%s]",
                     job->argument);
        goto error;
    }
    arg->reboot_mode_flag = json_object_get_int(jp_reboot_mode_flag);

    jp_reboot_wait_time =
        json_object_object_get(jp_arg, JA_PROTO_TAG_REBOOT_WAIT_TIME);
    if (jp_reboot_wait_time == NULL) {
        zbx_snprintf(job->message, sizeof(job->message),
                     "Can not get jp_reboot_wait_time from job argument [%s]",
                     job->argument);
        goto error;
    }
    arg->reboot_wait_time = json_object_get_int(jp_reboot_wait_time);

    ret = SUCCEED;
  error:
    json_object_put(jp_arg);
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
int ja_reboot_chkend(ja_job_object * job)
{
    ja_reboot_arg arg;
    int num, i, loop;
    zbx_uint64_t sec;
    char reboot_flag_file[JA_MAX_STRING_LEN];
    const char *__function_name = "ja_reboot_chkend";

    if (job == NULL) {
        return FAIL;
    }

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64, __function_name, job->jobid);

    zbx_snprintf(reboot_flag_file, sizeof(reboot_flag_file),
                 "%s-" ZBX_FS_UI64, CONFIG_REBOOT_FLAG, job->jobid);

    if (ja_file_getsize(reboot_flag_file) < 0) {
        return SUCCEED;
    }

    sec = time(NULL) - job->start_time;
    if (ja_reboot_load_arg(job, &arg) == FAIL) {
        arg.reboot_mode_flag = 0;
    }

    num = ja_jobdb_get_jobs();
    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() reboot_flag_file: %s, job num: %d, reboot_mode_flag: %d, reboot_wait_time: %d, sec: %d, jobid: "
               ZBX_FS_UI64, __function_name, reboot_flag_file, num,
               arg.reboot_mode_flag, arg.reboot_wait_time, sec, job->jobid);

    if (num == 0 || arg.reboot_mode_flag == 0 ||
       (sec >= arg.reboot_wait_time && arg.reboot_wait_time > 0)) {
        if (num != 0) {
            ja_reboot_killall();
        }

        if (ja_file_remove(reboot_flag_file) == SUCCEED) {
            while (1) {
                zabbix_log(LOG_LEVEL_INFORMATION, "In %s() reboot now. jobid: " ZBX_FS_UI64, __function_name, job->jobid);
                zbx_sleep(1);

                if (ja_process_check(job->pid) == SUCCEED) {
                    continue;
                }

                if (job->method == JA_AGENT_METHOD_TEST) {
                    loop = 3;
                }
                else {
                    loop = 120;
                }

                for (i = 0; i < loop; i++) {
                    zbx_sleep(1);
                }
                return SUCCEED;
            }
        }
    }

    return FAIL;
}
