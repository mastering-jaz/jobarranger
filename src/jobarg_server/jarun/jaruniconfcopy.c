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
** $Date:: 2013-07-10 15:16:37 +0900 #$
** $Revision: 5154 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include <json/json.h>
#include "common.h"
#include "comms.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jastatus.h"
#include "javalue.h"
#include "jaflow.h"
#include "jajobobject.h"
#include "jaconnect.h"
#include "jatcp.h"
#include "jathreads.h"
#include "jahost.h"
#include "jaruniconfcopy.h"

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
int jarun_icon_fcopy(const zbx_uint64_t inner_job_id, const int method)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    int from_host_flag, to_host_flag, overwrite_flag;
    char from_host_name[128], to_host_name[128];
    char from_host[JA_MAX_STRING_LEN], to_host[JA_MAX_STRING_LEN];
    char from_directory[JA_MAX_STRING_LEN];
    char from_file_name[JA_MAX_STRING_LEN];
    char to_directory[JA_MAX_STRING_LEN];
    char errmsg[JA_MAX_STRING_LEN];
    zbx_sock_t sock_from, sock_to;
    ja_job_object job, job_res;
    json_object *jp_arg;
    char buf[JA_MAX_DATA_LEN];
    ssize_t nbytes, written, mbytes;
    pid_t pid;
    const char *__function_name = "jarun_icon_fcopy";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    result =
        DBselect
        ("select * from ja_run_icon_fcopy_table where inner_job_id = "
         ZBX_FS_UI64, inner_job_id);
    if (NULL != (row = DBfetch(result))) {
        from_host_flag = atoi(row[2]);
        to_host_flag = atoi(row[3]);
        overwrite_flag = atoi(row[4]);
        zbx_snprintf(from_host_name, sizeof(from_host_name), "%s", row[5]);
        zbx_snprintf(to_host_name, sizeof(to_host_name), "%s", row[8]);

        if (ja_cpy_value(inner_job_id, row[6], from_directory) == FAIL) {
            zabbix_log(LOG_LEVEL_ERR,
                       "In %s() cat not find the value [%s]. inner_job_id: "
                       ZBX_FS_UI64, __function_name, row[6], inner_job_id);
            DBfree_result(result);
            return ja_set_runerr(inner_job_id);
        }

        if (ja_cpy_value(inner_job_id, row[7], from_file_name) == FAIL) {
            zabbix_log(LOG_LEVEL_ERR,
                       "In %s() cat not find the value [%s]. inner_job_id: "
                       ZBX_FS_UI64, __function_name, row[7], inner_job_id);
            DBfree_result(result);
            return ja_set_runerr(inner_job_id);
        }

        if (ja_cpy_value(inner_job_id, row[9], to_directory) == FAIL) {
            zabbix_log(LOG_LEVEL_ERR,
                       "In %s() cat not find the value [%s]. inner_job_id: "
                       ZBX_FS_UI64, __function_name, row[9], inner_job_id);
            DBfree_result(result);
            return ja_set_runerr(inner_job_id);
        }
    } else {
        ja_log("JARUNICONFCOPY200001", 0, NULL, inner_job_id,
               __function_name, inner_job_id);
        DBfree_result(result);
        return ja_set_runerr(inner_job_id);
    }
    DBfree_result(result);

    if (ja_host_getname
        (inner_job_id, from_host_flag, from_host_name,
         from_host) == FAIL) {
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() can get host name %s. from_host_flag: %d, inner_job_id: "
                   ZBX_FS_UI64, __function_name, from_host_name,
                   from_host_flag, inner_job_id);
        return ja_set_runerr(inner_job_id);
    }

    if (ja_host_getname(inner_job_id, to_host_flag, to_host_name, to_host)
        == FAIL) {
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() can get host name %s. from_host_flag: %d, inner_job_id: "
                   ZBX_FS_UI64, __function_name, from_host_name,
                   from_host_flag, inner_job_id);
        return ja_set_runerr(inner_job_id);
    }

    if (ja_host_lockinfo(from_host) == SUCCEED) {
        zabbix_log(LOG_LEVEL_DEBUG,
                   "In %s() host '%s' is locked. inner_job_id: "
                   ZBX_FS_UI64, __function_name, from_host, inner_job_id);
        return FAIL;
    }
    if (ja_host_lockinfo(to_host) == SUCCEED) {
        zabbix_log(LOG_LEVEL_INFORMATION,
                   "In %s() host '%s' is locked. inner_job_id: "
                   ZBX_FS_UI64, __function_name, to_host, inner_job_id);
        return FAIL;
    }

    pid = ja_fork();
    if (pid == -1) {
        ja_log("JARUNICONFCOPY200002", 0, NULL, inner_job_id,
               __function_name, inner_job_id);
        return ja_set_runerr(inner_job_id);
    } else if (pid != 0) {
        waitpid(pid, NULL, WNOHANG);
        return SUCCEED;
    }

    ret = FAIL;
    ja_job_object_init(&job);
    ja_job_object_init(&job_res);
    sock_from.socket = ZBX_SOCK_ERROR;
    sock_to.socket = ZBX_SOCK_ERROR;
    memset(errmsg, 0, sizeof(errmsg));

    jp_arg = json_object_new_object();
    json_object_object_add(jp_arg, JA_PROTO_TAG_FROMDIR,
                           json_object_new_string(from_directory));
    json_object_object_add(jp_arg, JA_PROTO_TAG_FILENAME,
                           json_object_new_string(from_file_name));
    json_object_object_add(jp_arg, JA_PROTO_TAG_TODIR,
                           json_object_new_string(to_directory));
    json_object_object_add(jp_arg, JA_PROTO_TAG_OVERWRITE,
                           json_object_new_int(overwrite_flag));
    zbx_snprintf(job.argument, sizeof(job.argument), "%s",
                 json_object_to_json_string(jp_arg));
    json_object_put(jp_arg);

    zbx_snprintf(job.kind, sizeof(job.kind), "%s", JA_PROTO_VALUE_FCOPY);
    job.version = JA_PROTO_TELE_VERSION;
    job.jobid = inner_job_id;
    zbx_snprintf(job.serverid, sizeof(job.serverid), "%s", serverid);
    job.method = method;

    // connet to_host
    DBconnect(ZBX_DB_CONNECT_ONCE);
    if (ja_connect(&sock_to, to_host) == FAIL) {
        zbx_snprintf(errmsg, sizeof(errmsg),
                     "can not connect the host '%s'", to_host);
        DBclose();
        goto error;
    }
    DBclose();

    zbx_snprintf(job.hostname, sizeof(job.hostname), "%s", to_host);
    zbx_snprintf(job.type, sizeof(job.type), "%s", JA_PROTO_VALUE_PUTFILE);
    if (ja_tcp_send_to(&sock_to, &job, CONFIG_TIMEOUT) == FAIL)
        goto error;
    if (ja_tcp_recv_to(&sock_to, &job_res, CONFIG_TIMEOUT) == FAIL)
        goto error;
    if (job_res.result != JA_RESPONSE_SUCCEED)
        goto error;

    DBconnect(ZBX_DB_CONNECT_ONCE);
    if (ja_connect(&sock_from, from_host) == FAIL) {
        zbx_snprintf(errmsg, sizeof(errmsg),
                     "can not connect the host '%s'", from_host);
        DBclose();
        goto error;
    }
    DBclose();

    // connect from_host
    zbx_snprintf(job.hostname, sizeof(job.hostname), "%s", from_host);
    zbx_snprintf(job.type, sizeof(job.type), "%s", JA_PROTO_VALUE_GETFILE);
    if (ja_tcp_send_to(&sock_from, &job, CONFIG_TIMEOUT) == FAIL)
        goto error;

    // copy files
    while (1) {
        ja_tcp_timeout_set(sock_from.socket, CONFIG_FCOPY_TIMEOUT);
        nbytes = ZBX_TCP_READ(sock_from.socket, (void *) buf, sizeof(buf));
        if (nbytes == ZBX_TCP_ERROR) {
            zbx_snprintf(errmsg, sizeof(errmsg), "%s",
                         strerror_from_system(zbx_sock_last_error()));
            goto error;
        }
        zabbix_log(LOG_LEVEL_DEBUG, "In %s() nbytes: %d", __function_name,
                   nbytes);
        if (nbytes == 0)
            break;

        mbytes = 0;
        written = 0;
        while (written < nbytes) {
            ja_tcp_timeout_set(sock_to.socket, CONFIG_FCOPY_TIMEOUT);
            mbytes =
                ZBX_TCP_WRITE(sock_to.socket, buf + written,
                              (int) (nbytes - written));
            if (mbytes == ZBX_TCP_ERROR) {
                zbx_snprintf(errmsg, sizeof(errmsg), "%s",
                             strerror_from_system(zbx_sock_last_error()));
                goto error;
            }
            written += mbytes;
        }
    }
    if (ja_tcp_recv_to(&sock_to, &job_res, CONFIG_FCOPY_TIMEOUT) == FAIL)
        goto error;
    if (job_res.result != JA_RESPONSE_SUCCEED)
        goto error;

    ret = SUCCEED;
  error:
    if (sock_from.socket >= 0)
        zbx_tcp_close(&sock_from);
    if (sock_to.socket >= 0)
        zbx_tcp_close(&sock_to);
    DBconnect(ZBX_DB_CONNECT_ONCE);
    if (ret == SUCCEED) {
        ja_flow(inner_job_id, JA_FLOW_TYPE_NORMAL);
    } else {
        if (strlen(errmsg) == 0) {
            zbx_snprintf(errmsg, sizeof(errmsg), "%s", job_res.message);
        }
        ja_log("JARUNICONFCOPY200003", 0, NULL, inner_job_id,
               __function_name, errmsg, inner_job_id);
        ja_set_runerr(inner_job_id);
    }
    DBclose();
    zabbix_log(LOG_LEVEL_INFORMATION,
               "In %s() END. inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);
    exit(0);
}
