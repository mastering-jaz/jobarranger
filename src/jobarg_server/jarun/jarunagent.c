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
** $Date:: 2013-05-27 13:50:15 +0900 #$
** $Revision: 4664 $
** $Author: ossinfra@FITECHLABS.CO.JP $
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
#include "jarunagent.h"

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
int jarun_agent(ja_job_object * job, const char *host_name,
                const int host_flag)
{
    int ret;
    char host[128];
    zbx_sock_t sock;
    pid_t pid;
    ja_job_object job_res;
    const char *__function_name = "jarun_agent";

    if (job == NULL)
        return FAIL;
    zabbix_log(LOG_LEVEL_DEBUG, "In %s() job id: " ZBX_FS_UI64,
               __function_name, job->jobid);

    if (ja_host_getname(job->jobid, host_flag, host_name, host) == FAIL) {
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() can get host name %s. host_flag: %d, job id: "
                   ZBX_FS_UI64, __function_name, host_name, host_flag,
                   job->jobid);
        return ja_set_runerr(job->jobid);
    }

    if (ja_host_lockinfo(host) == SUCCEED) {
        zabbix_log(LOG_LEVEL_DEBUG,
                   "In %s() host '%s' is locked. job id: "
                   ZBX_FS_UI64, __function_name, host, job->jobid);
        return FAIL;
    }

    zbx_snprintf(job->kind, sizeof(job->kind), "%s",
                 JA_PROTO_VALUE_JOBRUN);
    job->version = JA_PROTO_TELE_VERSION;
    zbx_snprintf(job->serverid, sizeof(job->serverid), "%s", serverid);
    zbx_snprintf(job->hostname, sizeof(job->hostname), "%s", host);

    if (job->method == JA_JOB_METHOD_ABORT) {
        if (ja_set_status_job(job->jobid, JA_JOB_STATUS_ABORT, -1, -1) ==
            FAIL)
            return FAIL;
    }

    pid = ja_fork();
    if (pid == -1) {
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() can not fork(). job id: " ZBX_FS_UI64,
                   __function_name, job->jobid);
        if (job->method == JA_JOB_METHOD_ABORT)
            return FAIL;
        else
            return ja_set_runerr(job->jobid);
    } else if (pid != 0) {
        waitpid(pid, NULL, WNOHANG);
        return SUCCEED;
    }

    ret = FAIL;
    ja_job_object_init(&job_res);
    DBconnect(ZBX_DB_CONNECT_ONCE);
    if (ja_connect(&sock, host) == FAIL) {
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() can not connect the host %s. job id: "
                   ZBX_FS_UI64, __function_name, host, job->jobid);
        ja_set_runerr(job->jobid);
        DBclose();
        exit(1);
    }
    DBclose();

    zbx_snprintf(job_res.message, sizeof(job_res.message),
                 "Can not get the response from host: %s", job->hostname);
    if (ja_tcp_send_to(&sock, job, CONFIG_TIMEOUT) == FAIL)
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
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() job id: " ZBX_FS_UI64 " message: %s",
                   __function_name, job->jobid, job_res.message);
        if (job->method != JA_JOB_METHOD_ABORT)
            ja_set_runerr(job->jobid);
        DBclose();
        exit(1);
    }
}
