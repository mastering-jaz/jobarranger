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
** $Date:: 2013-04-22 16:38:33 +0900 #$
** $Revision: 4446 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "comms.h"
#include "log.h"

#if defined(ZABBIX_SERVICE)
#include "service.h"
#elif defined(ZABBIX_DAEMON)
#include "daemon.h"
#endif

#include "jacommon.h"
#include "jajobobject.h"
#include "jatcp.h"
#include "jatelegram.h"
#include "jaagent.h"
#include "jafcopy.h"
#include "jajobdb.h"
#include "listener.h"

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
static int process_listener(zbx_sock_t * s)
{
    int ret;
    ja_job_object job;

    if (zbx_tcp_check_security(s, CONFIG_HOSTS_ALLOWED, 0) == FAIL) {
        zbx_tcp_unaccept(s);
        return FAIL;
    }

    ja_job_object_init(&job);
    ret = ja_tcp_recv_to(s, &job, CONFIG_TIMEOUT);
    job.result = JA_RESPONSE_SUCCEED;
    if (strcmp(job.kind, JA_PROTO_VALUE_JOBRUN) == 0) {
        zbx_snprintf(job.kind, sizeof(job.kind), "%s",
                     JA_PROTO_VALUE_JOBRUN_RES);
        if (ret == SUCCEED) {
            if (ja_agent_begin(&job) != SUCCEED) {
                if (job.result == JA_RESPONSE_SUCCEED)
                    job.result = JA_RESPONSE_FAIL;
            }
        } else {
            if (job.result == JA_RESPONSE_SUCCEED)
                job.result = JA_RESPONSE_FAIL;
        }
    } else if (strcmp(job.kind, JA_PROTO_VALUE_FCOPY) == 0) {
        zbx_snprintf(job.kind, sizeof(job.kind), "%s",
                     JA_PROTO_VALUE_FCOPY_RES);
        if (ret == SUCCEED) {
            ja_fcopy_begin(&job, s);
            return SUCCEED;
        } else {
            if (job.result == JA_RESPONSE_SUCCEED)
                job.result = JA_RESPONSE_FAIL;
        }
    } else {
        zbx_snprintf(job.kind, sizeof(job.kind), "%s",
                     JA_PROTO_VALUE_JOBRUN_RES);
        job.version = JA_PROTO_TELE_VERSION;
        job.result = JA_RESPONSE_FAIL;
    }

    ja_tcp_send_to(s, &job, CONFIG_TIMEOUT);
    zbx_tcp_unaccept(s);
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
ZBX_THREAD_ENTRY(listener_thread, args)
{
    int ret, local_request_failed = 0;
    zbx_sock_t sock;

    assert(args);
    assert(((zbx_thread_args_t *) args)->args);

    zabbix_log(LOG_LEVEL_INFORMATION,
               "jobarg_agentd #%d started [listener]",
               ((zbx_thread_args_t *) args)->thread_num);

    memcpy(&sock, (zbx_sock_t *) ((zbx_thread_args_t *) args)->args,
           sizeof(zbx_sock_t));
    zbx_free(args);

    while (ZBX_IS_RUNNING()) {
        zbx_setproctitle("listener [waiting for connection]");
        if (SUCCEED == (ret = ja_tcp_accept(&sock))
            && sock.socket != ZBX_SOCK_ERROR) {
            local_request_failed = 0;
            zbx_setproctitle("listener [processing request]");
            ret = process_listener(&sock);
        }

        if (SUCCEED == ret)
            continue;

        zabbix_log(LOG_LEVEL_WARNING, "Listener error: %s",
                   zbx_tcp_strerror());

        if (local_request_failed++ > 1000) {
            zabbix_log(LOG_LEVEL_WARNING,
                       "Too many consecutive errors on accept() call.");
            local_request_failed = 0;
        }
        if (ZBX_IS_RUNNING())
            zbx_sleep(1);
    }

    zabbix_log(LOG_LEVEL_INFORMATION, "jobarg_agentd listener stopped");
    ZBX_DO_EXIT();
    zbx_thread_exit(0);
}
