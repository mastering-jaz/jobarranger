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

#include <json.h>
#include "common.h"
#include "comms.h"
#include "log.h"

#include "jacommon.h"
#include "jastr.h"
#include "jajobobject.h"
#include "jatelegram.h"
#include "jatcp.h"

#ifndef ZBX_SOCKLEN_T
#define ZBX_SOCKLEN_T socklen_t
#endif

#ifdef HAVE_IPV6
#define ZBX_SOCKADDR struct sockaddr_storage
#else
#define ZBX_SOCKADDR struct sockaddr_in
#endif

#ifdef _WINDOWS
#define EINTR       WSAEINTR
#endif

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
int ja_tcp_accept(zbx_sock_t * s)
{
    ZBX_SOCKADDR serv_addr;
    ZBX_SOCKLEN_T nlen;
    const char *__function_name = "ja_tcp_accept";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    zbx_tcp_unaccept(s);
    nlen = sizeof(serv_addr);

    if (ZBX_SOCK_ERROR ==
        (s->socket =
         (ZBX_SOCKET) accept(s->sockets[0], (struct sockaddr *) &serv_addr,
                             &nlen))) {
        if (zbx_sock_last_error() == EINTR)
            return SUCCEED;
        zabbix_log(LOG_LEVEL_WARNING, "accept() failed: %s %d",
                   strerror_from_system(zbx_sock_last_error()),
                   zbx_sock_last_error());

        return FAIL;
    }

    s->socket_orig = ZBX_SOCK_ERROR;
    s->accepted = 1;

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
int ja_tcp_send_to(zbx_sock_t * s, ja_job_object * job, int timeout)
{
    int ret;
    char *data;
    const char *__function_name = "ja_tcp_send_to";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    data = ja_telegram_to(job);
    if (data == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() message: %s", __function_name,
                   job->message);
        return FAIL;
    }

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() %s", __function_name, data);
    ret = zbx_tcp_send_to(s, data, timeout);
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() error: %s", __function_name,
                   zbx_tcp_strerror());
    }
    zbx_free(data);
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
int ja_tcp_recv_to(zbx_sock_t * s, ja_job_object * job, int timeout)
{
    int ret;
    char *data;
    const char *__function_name = "ja_tcp_recv_to";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    ret = zbx_tcp_recv_to(s, &data, timeout);
    if (ret == FAIL) {
        zbx_snprintf(job->message, sizeof(job->message), "%s",
                     zbx_tcp_strerror());
        goto error;
    }
    zabbix_log(LOG_LEVEL_DEBUG, "In %s() %s", __function_name, data);
    if (strlen(data) == 0) {
        zbx_snprintf(job->message, sizeof(job->message),
                     "received data is null");
        goto error;
    }

    zbx_rtrim(data, "\r\n");
    ret = ja_telegram_from(data, job);

  error:
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() message: %s", __function_name,
                   job->message);
    }
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
void ja_tcp_timeout_set(int fd, int timeout)
{
#ifdef _WINDOWS
    timeout *= 1000;

    if (ZBX_TCP_ERROR ==
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout,
                   sizeof(timeout)))
        zabbix_log(LOG_LEVEL_ERR, "setsockopt() failed.");

    if (ZBX_TCP_ERROR ==
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *) &timeout,
                   sizeof(timeout)))
        zabbix_log(LOG_LEVEL_ERR, "setsockopt() failed.");
#else
    alarm(timeout);
#endif
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
int ja_tcp_send(zbx_sock_t * s, int timeout, json_object * json)
{
    int ret;
    char *data;
    const char *__function_name = "ja_tcp_send";

    if (json == NULL)
        return FAIL;
    data = (char *) json_object_to_json_string(json);
    zabbix_log(LOG_LEVEL_DEBUG, "In %s() data: %s", __function_name, data);

    ret = zbx_tcp_send_to(s, data, timeout);
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() error: %s", __function_name,
                   zbx_tcp_strerror());
    }

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
int ja_tcp_recv(zbx_sock_t * s, int timeout, json_object * json)
{
    int ret;
    char *data, *err;
    json_object *jp;
    const char *__function_name = "ja_tcp_recv";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    ret = FAIL;
    jp = NULL;
    err = NULL;
    if (zbx_tcp_recv_to(s, &data, timeout) == FAIL) {
        err = zbx_dsprintf(NULL, "%s", zbx_tcp_strerror());
        goto error;
    }

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() data: %s", __function_name, data);
    if (strlen(data) == 0) {
        err = zbx_dsprintf(NULL, "received data is null");
        goto error;
    }

    jp = json_tokener_parse(data);
    if (is_error(jp)) {
        err = zbx_dsprintf(NULL, "can not parse json data: %s", data);
        goto error;
    }

    json_object_object_add(json, "data", jp);
    ret = SUCCEED;
  error:
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() error: %s", __function_name,
                   err);
        json_object_object_add(json, "data", json_object_new_string(err));
    }
    if (err != NULL)
        zbx_free(err);
    return ret;
}
