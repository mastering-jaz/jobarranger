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
** $Date:: 2014-02-20 15:50:58 +0900 #$
** $Revision: 5808 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "comms.h"
#include "db.h"
#include "log.h"

#include "jacommon.h"
#include "jadb.h"
#include "jalog.h"
#include "jahost.h"
#include "jaconnect.h"

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
int ja_connect(zbx_sock_t * s, const char *host, const zbx_uint64_t inner_job_id)
{
    int ret;
    char host_ip[128];
    zbx_uint64_t hostid;
    int port;
    const char *__function_name = "ja_connect";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() host: %s", __function_name, host);

    hostid = ja_host_getip(host, host_ip, inner_job_id);
    if (hostid == 0)
        return FAIL;
    port = ja_host_getport(hostid);

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() connect the host. source_ip: %s, host_ip: %s, port: %d, timeout: %d",
               __function_name, CONFIG_SOURCE_IP, host_ip, port,
               CONFIG_TIMEOUT);
    ret =
        zbx_tcp_connect(s, CONFIG_SOURCE_IP, host_ip, port,
                        CONFIG_TIMEOUT);
    if (ret == FAIL) {
        ja_log("JACONNECT300001", 0, NULL, inner_job_id, __function_name, zbx_tcp_strerror(),
               host_ip, port, CONFIG_SOURCE_IP, CONFIG_TIMEOUT);
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() can not connect the host. %s", __function_name,
                   zbx_tcp_strerror());
    }

    return ret;
}
