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
** $Date:: 2014-02-20 16:52:27 +0900 #$
** $Revision: 5809 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include <json/json.h>
#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "javalue.h"
#include "jastatus.h"
#include "jaflow.h"
#include "jathreads.h"
#include "jajobobject.h"
#include "jahost.h"
#include "jatcp.h"
#include "jaconnect.h"
#include "jaruniconreboot.h"

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
int jarun_icon_reboot(const zbx_uint64_t inner_job_id, const int method)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    int host_flag, reboot_mode_flag, reboot_wait_time;
    char host_name[128];
    char host[128];

    zbx_sock_t sock;
    ja_job_object job, job_res;
    json_object *jp_arg;
    pid_t pid;
    const char *__function_name = "jarun_icon_reboot";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    result =
        DBselect
        ("select host_flag, reboot_mode_flag, reboot_wait_time, host_name from ja_run_icon_reboot_table where inner_job_id = "
         ZBX_FS_UI64, inner_job_id);
    row = DBfetch(result);
    if (row != NULL) {
        host_flag = atoi(row[0]);
        reboot_mode_flag = atoi(row[1]);
        reboot_wait_time = atoi(row[2]);
        zbx_snprintf(host_name, sizeof(host_name), "%s", row[3]);
    } else {
        ja_log("JARUNICONREBOOT200001", 0, NULL, inner_job_id,
               __function_name, inner_job_id);
        DBfree_result(result);
        return ja_set_runerr(inner_job_id);
    }
    DBfree_result(result);

    if (ja_host_getname(inner_job_id, host_flag, host_name, host) == FAIL) {
        ja_log("JARUNICONREBOOT200002", 0, NULL, inner_job_id,
               __function_name, host_name, host_flag, inner_job_id);
        return ja_set_runerr(inner_job_id);
    }

    if (method == JA_JOB_METHOD_ABORT) {
        if (ja_set_status_job(inner_job_id, JA_JOB_STATUS_ABORT, -1, -1) ==
            FAIL)
            return FAIL;
    } else {

        if (ja_host_lockinfo(host) == SUCCEED) {
            zabbix_log(LOG_LEVEL_DEBUG,
                       "In %s() host '%s' is locked. inner_job_id: "
                       ZBX_FS_UI64, __function_name, host, inner_job_id);
            return FAIL;
        }

        if (ja_host_lock(host, inner_job_id) == FAIL) {
            return ja_set_runerr(inner_job_id);
        }
    }

    pid = ja_fork();
    if (pid == -1) {
        ja_log("JARUNICONREBOOT200004", 0, NULL, inner_job_id,
               __function_name, inner_job_id);
        if (method != JA_JOB_METHOD_ABORT) {
            ja_host_unlock(host);
            return ja_set_runerr(inner_job_id);
        } else {
            return FAIL;
        }

    } else if (pid != 0) {
        waitpid(pid, NULL, WNOHANG);
        return SUCCEED;
    }

    ret = FAIL;
    DBconnect(ZBX_DB_CONNECT_ONCE);
    if (ja_connect(&sock, host, inner_job_id) == FAIL) {
        if (method != JA_JOB_METHOD_ABORT) {
            ja_host_unlock(host);
            ja_set_runerr(inner_job_id);
        }
        DBclose();
        exit(1);
    }
    DBclose();

    ja_job_object_init(&job);
    ja_job_object_init(&job_res);

    jp_arg = json_object_new_object();
    json_object_object_add(jp_arg, JA_PROTO_TAG_REBOOT_MODE,
                           json_object_new_int(reboot_mode_flag));
    json_object_object_add(jp_arg, JA_PROTO_TAG_REBOOT_WAIT_TIME,
                           json_object_new_int(reboot_wait_time));

    zbx_snprintf(job.kind, sizeof(job.kind), "%s", JA_PROTO_VALUE_JOBRUN);
    job.version = JA_PROTO_TELE_VERSION;
    job.jobid = inner_job_id;
    zbx_snprintf(job.serverid, sizeof(job.serverid), "%s", serverid);
    zbx_snprintf(job.hostname, sizeof(job.hostname), "%s", host);
    job.method = method;
    zbx_snprintf(job.type, sizeof(job.type), "%s", JA_PROTO_VALUE_REBOOT);
    zbx_snprintf(job.argument, sizeof(job.argument), "%s",
                 json_object_to_json_string(jp_arg));
    json_object_put(jp_arg);

    if (ja_tcp_send_to(&sock, &job, CONFIG_TIMEOUT) == FAIL)
        goto error;
    job_res.result = JA_RESPONSE_FAIL;
    if (ja_tcp_recv_to(&sock, &job_res, CONFIG_TIMEOUT) == FAIL)
        goto error;
    if (job_res.result != JA_RESPONSE_SUCCEED) {
        ja_log("JARUNICONREBOOT200006", 0, NULL, inner_job_id,
               __function_name, inner_job_id, job_res.message);
        goto error;
    }
    ret = SUCCEED;

  error:
    zbx_tcp_close(&sock);
    if (ret == SUCCEED) {
        exit(0);
    } else {
        DBconnect(ZBX_DB_CONNECT_ONCE);
        if (method != JA_JOB_METHOD_ABORT) {
            ja_host_unlock(job.hostname);
            ja_set_runerr(inner_job_id);
        }
        DBclose();
        exit(1);
    }
}
