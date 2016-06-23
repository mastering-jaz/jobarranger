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

#include "common.h"
#include "comms.h"
#include "db.h"
#include "log.h"

#include "jacommon.h"
#include "jalog.h"
#include "javalue.h"
#include "jahost.h"

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
int ja_host_getname(const zbx_uint64_t inner_job_id, const int host_flag,
                    const char *host_name, char *host)
{
    const char *__function_name = "ja_host_getname";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() host_flag: %d, host_name: %s, ",
               __function_name, host_flag, host_name);

    if (host_flag == 0) {
        zbx_snprintf(host, JA_MAX_STRING_LEN, "%s", host_name);
        return SUCCEED;
    }

    return ja_get_value_before(inner_job_id, host_name, host);
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
zbx_uint64_t ja_host_getip(const char *host, char *host_ip)
{
    DB_RESULT result;
    DB_ROW row;
    char *host_esc;
    zbx_uint64_t hostid;
    const char *__function_name = "ja_host_getip";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() host: %s", __function_name, host);

    hostid = 0;
    result = NULL;
    host_esc = DBdyn_escape_string(host);
    switch (CONFIG_ZABBIX_VERSION) {
    case 1:
        // for zabbix 1.8
        result =
            DBselect
            ("select hostid, useip, dns, ip, status from hosts where host = '%s'",
             host_esc);
        break;
    case 2:
        // for zabbix 2.0
        result =
            DBselect
            (" select i.hostid, i.useip, i.dns, i.ip, h.status from hosts h, interface i"
             " where h.hostid = i.hostid and i.main = 1 and host = '%s'",
             host_esc);
        break;
    default:
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() can not support ZABBIX_VERSION: %d",
                   __function_name, CONFIG_ZABBIX_VERSION);
        break;
    }

    if (result == NULL) {
        DBrollback();
        goto error;
    }

    row = DBfetch(result);
    if (row == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() can not find host: %s",
                   __function_name, host);
        goto error;
    }

    if (atoi(row[4]) != HOST_STATUS_MONITORED) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() host: %s is disabled",
                   __function_name, host);
        goto error;
    }

    ZBX_STR2UINT64(hostid, row[0]);
    if (host_ip != NULL) {
        if (atoi(row[1]) == 0) {
            // use dns
            zbx_snprintf(host_ip, strlen(row[2]) + 1, "%s", row[2]);
        } else {
            // use ip
            zbx_snprintf(host_ip, strlen(row[3]) + 1, "%s", row[3]);
        }
    }
    DBfree_result(result);
    result = NULL;

  error:
    zbx_free(host_esc);
    if (result != NULL)
        DBfree_result(result);
    return hostid;
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
int ja_host_getport(zbx_uint64_t hostid)
{
    int port;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_host_getport";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() hostid: " ZBX_FS_UI64,
               __function_name, hostid);
    port = CONFIG_AGENT_LISTEN_PORT;
    result =
        DBselect("select value from hostmacro where hostid = " ZBX_FS_UI64
                 " and macro = '%s'", hostid, JA_AGENT_PORT);
    row = DBfetch(result);
    if (row != NULL)
        port = atoi(row[0]);
    DBfree_result(result);

    return port;
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
int ja_host_auth(zbx_sock_t * sock, const char *host)
{
    char host_ip[128];
    const char *__function_name = "ja_host_auth";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() host: %s", __function_name, host);

    if (ja_host_getip(host, host_ip) == 0) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() can not find host: %s",
                   __function_name, host);
        return FAIL;
    }

    if (zbx_tcp_check_security(sock, host_ip, 0) == FAIL) {
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() authentication failure host or ip: %s",
                   __function_name, host_ip);
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
int ja_host_lockinfo(const char *host)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    char *host_esc;
    const char *__function_name = "ja_host_lockinfo";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() host: %s", __function_name, host);

    ret = FAIL;
    host_esc = DBdyn_escape_string(host);
    result =
        DBselect
        ("select lock_host_name from ja_host_lock_table where lock_host_name = '%s'",
         host_esc);
    row = DBfetch(result);
    if (row != NULL)
        ret = SUCCEED;

    zbx_free(host_esc);
    DBfree_result(result);
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
int ja_host_lock(const char *host)
{
    int db_ret;
    char *host_esc;
    const char *__function_name = "ja_host_lock";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() host: %s", __function_name, host);

    if (ja_host_getip(host, NULL) == 0)
        return FAIL;

    DBfree_result(DBselect
                  ("select * from ja_host_lock_table where lock_host_name = 'HOST_LOCK_RECORD' for update"));

    host_esc = DBdyn_escape_string(host);
    db_ret =
        DBexecute
        ("insert into ja_host_lock_table (lock_host_name) values ('%s')",
         host_esc);
    zbx_free(host_esc);

    if (db_ret < ZBX_DB_OK)
        return FAIL;
    else
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
int ja_host_unlock(const char *host)
{
    int db_ret;
    char *host_esc;
    const char *__function_name = "ja_host_unlock";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() host: %s", __function_name, host);

    DBfree_result(DBselect
                  ("select * from ja_host_lock_table where lock_host_name = 'HOST_LOCK_RECORD' for update"));

    host_esc = DBdyn_escape_string(host);
    db_ret =
        DBexecute
        ("delete from ja_host_lock_table where lock_host_name = '%s'",
         host_esc);
    zbx_free(host_esc);

    if (db_ret < ZBX_DB_OK)
        return FAIL;
    else
        return SUCCEED;
}