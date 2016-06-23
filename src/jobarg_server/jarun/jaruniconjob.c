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
** $Date:: 2013-11-14 11:51:12 +0900 #$
** $Revision: 5320 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include <json/json.h>
#include "common.h"
#include "comms.h"
#include "log.h"
#include "db.h"
#include "threads.h"

#include "jacommon.h"
#include "jajobobject.h"
#include "jalog.h"
#include "javalue.h"
#include "jatcp.h"
#include "jaconnect.h"
#include "jastatus.h"
#include "jathreads.h"
#include "jahost.h"
#include "jaruniconjob.h"

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
int jarun_icon_job_getenv(zbx_uint64_t inner_job_id, ja_job_object * job)
{
    json_object *jp_env;
    DB_RESULT result;
    DB_ROW row;
    char *p;
    char value[JA_MAX_DATA_LEN];
    const char *__function_name = "jarun_icon_job_getenv";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    jp_env = json_object_new_object();
    result =
        DBselect("select value_name from ja_run_value_jobcon_table"
                 " where inner_job_id = " ZBX_FS_UI64, inner_job_id);
    while (NULL != (row = DBfetch(result))) {
        zbx_snprintf(value, sizeof(value), "");
        ja_get_value_before(inner_job_id, row[0], value);
        json_object_object_add(jp_env, row[0],
                               json_object_new_string(value));
    }
    DBfree_result(result);

    result =
        DBselect("select value_name, value from ja_run_value_job_table"
                 " where inner_job_id = " ZBX_FS_UI64, inner_job_id);
    while (NULL != (row = DBfetch(result))) {
        zbx_snprintf(value, sizeof(value), "");
        p = row[1];
        if (*p == '$') {
            ja_get_value_before(inner_job_id, p + 1, value);
        } else {
            zbx_strlcpy(value, row[1], sizeof(value));
        }
        
        json_object_object_add(jp_env, row[0],
                               json_object_new_string(value));
    }
    DBfree_result(result);

    zbx_snprintf(job->env, sizeof(job->env), "%s",
                 json_object_to_json_string(jp_env));
    json_object_put(jp_env);
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
int jarun_icon_job(zbx_uint64_t inner_job_id, int flag)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    ja_job_object job, job_res;
    zbx_uint64_t inner_jobnet_id;
    int host_flag;
    int command_type;
    char host_name[128];
    char host[128];
    pid_t pid;
    zbx_sock_t sock;
    const char *__function_name = "jarun_icon_job";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    ja_job_object_init(&job);
    result = DBselect
        ("select inner_jobnet_id, host_flag, command_type, host_name from ja_run_icon_job_table where inner_job_id = "
         ZBX_FS_UI64, inner_job_id);
    row = DBfetch(result);
    if (row != NULL) {
        ZBX_STR2UINT64(inner_jobnet_id, row[0]);
        host_flag = atoi(row[1]);
        command_type = atoi(row[2]);
        zbx_snprintf(host_name, sizeof(host_name), "%s", row[3]);
    } else {
        ja_log("JARUNICONJOB200017", inner_jobnet_id, NULL, inner_job_id, inner_job_id);
        DBfree_result(result);
        return ja_set_runerr(inner_job_id);
    }
    DBfree_result(result);

    if (ja_host_getname(inner_job_id, host_flag, host_name, host) == FAIL) {
        ja_log("JARUNICONJOB200028", inner_jobnet_id, NULL, inner_job_id, host_name, host_flag, inner_job_id);
        return ja_set_runerr(inner_job_id);
    }

    if (ja_host_lockinfo(host) == SUCCEED) {
        zabbix_log(LOG_LEVEL_DEBUG,
                   "In %s() host '%s' is locked. inner_job_id: "
                   ZBX_FS_UI64, __function_name, host, inner_job_id);
        return FAIL;
    }

    zbx_snprintf(job.kind, sizeof(job.kind), "%s", JA_PROTO_VALUE_JOBRUN);
    job.version = JA_PROTO_TELE_VERSION;
    job.jobid = inner_job_id;
    zbx_snprintf(job.serverid, sizeof(job.serverid), "%s", serverid);
    zbx_snprintf(job.hostname, sizeof(job.hostname), "%s", host);
    job.method = flag;

    if (flag == JA_AGENT_METHOD_ABORT || flag == JA_AGENT_METHOD_KILL) {
        if (ja_set_status_job(inner_job_id, JA_JOB_STATUS_ABORT, -1, -1) ==
            FAIL)
            return FAIL;
    }

    if (flag == JA_AGENT_METHOD_ABORT) {
        command_type = 2;
        result =
            DBselect("select inner_job_id_fs_link from ja_run_job_table"
                     " where inner_job_id = " ZBX_FS_UI64, inner_job_id);
        row = DBfetch(result);
        if (row == NULL) {
            ja_log("JARUNICONJOB200024", inner_jobnet_id, NULL, inner_job_id, inner_job_id);
            DBfree_result(result);
            return ja_set_runerr(inner_job_id);
        }
        ZBX_STR2UINT64(job.jobid, row[0]);
        DBfree_result(result);

    }

    if (job.method != JA_AGENT_METHOD_KILL) {
        zbx_snprintf(job.type, sizeof(job.type), "%s",
                     JA_PROTO_VALUE_COMMAND);
        result =
            DBselect("select command from ja_run_job_command_table"
                     " where inner_job_id = " ZBX_FS_UI64
                     " and command_cls = %d", inner_job_id, command_type);
        row = DBfetch(result);
        if (row == NULL) {
            ja_log("JARUNICONJOB200024", inner_jobnet_id, NULL, inner_job_id, inner_job_id);
            DBfree_result(result);
            return ja_set_runerr(inner_job_id);
        }
        zbx_snprintf(job.script, sizeof(job.script), "%s", row[0]);
        DBfree_result(result);
        jarun_icon_job_getenv(inner_job_id, &job);
    }

    pid = ja_fork();
    if (pid == -1) {
        ja_log("JARUNICONJOB200027", inner_jobnet_id, NULL, inner_job_id, inner_job_id);
        return ja_set_runerr(inner_job_id);
    } else if (pid != 0) {
        waitpid(pid, NULL, WNOHANG);
        return SUCCEED;
    }

    ret = FAIL;
    ja_job_object_init(&job_res);
    DBconnect(ZBX_DB_CONNECT_ONCE);
    if (ja_connect(&sock, host) == FAIL) {
        zbx_snprintf(job_res.message, sizeof(job_res.message),
                     "Can not connect the host: %s", host);
        ja_log("JARUNICONJOB200012", inner_jobnet_id, NULL, inner_job_id, inner_job_id, job_res.message);
        ja_set_runerr(inner_job_id);
        DBclose();
        exit(1);
    }
    DBclose();

    zbx_snprintf(job_res.message, sizeof(job_res.message),
                 "Can not get the response from host: %s", job.hostname);
    if (ja_tcp_send_to(&sock, &job, CONFIG_TIMEOUT) == FAIL)
        goto error;
    job_res.result = JA_RESPONSE_FAIL;
    if (ja_tcp_recv_to(&sock, &job_res, CONFIG_TIMEOUT) == FAIL)
        goto error;
    if (job_res.result != JA_RESPONSE_SUCCEED)
        goto error;

    ret = SUCCEED;
  error:
    zbx_tcp_close(&sock);
    if (ret == SUCCEED) {
        exit(0);
    } else {
        DBconnect(ZBX_DB_CONNECT_ONCE);
        ja_log("JARUNICONJOB200012", inner_jobnet_id, NULL, inner_job_id, inner_job_id, job_res.message);
        ja_set_runerr(inner_job_id);
        DBclose();
        exit(1);
    }
}
