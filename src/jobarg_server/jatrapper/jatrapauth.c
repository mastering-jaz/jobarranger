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
** $Date:: 2013-06-06 11:15:53 +0900 #$
** $Revision: 4864 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include <json/json.h>
#include "common.h"
#include "comms.h"
#include "log.h"

#include "jatelegram.h"
#include "jauser.h"
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
int jatrap_auth_host(zbx_sock_t * sock, ja_telegram_object * obj)
{
    int ret;
    json_object *jp_data, *jp;
    char *request;
    char *hostname, *err;
    const char *__function_name = "jatrap_auth_host";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    if (ja_telegram_check(obj) == FAIL)
        return FAIL;

    ret = FAIL;
    err = NULL;
    request = (char *) json_object_to_json_string(obj->request);
    jp_data = json_object_object_get(obj->request, JA_PROTO_TAG_DATA);
    jp = json_object_object_get(jp_data, JA_PROTO_TAG_HOSTNAME);
    if (jp == NULL) {
        err =
            zbx_dsprintf(NULL,
                         "can not find the tag '%s' from the request telegram: %s",
                         JA_PROTO_TAG_HOSTNAME, request);
        goto error;
    }
    hostname = (char *) json_object_get_string(jp);
    if (ja_host_auth(sock, hostname) == FAIL) {
        err =
            zbx_dsprintf(NULL, "host '%s' is not authenticated", hostname);
        goto error;
    }

    ret = SUCCEED;
  error:
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() error: %s", __function_name,
                   err);
        ja_telegram_seterr(obj, err);
    }
    if (err != NULL)
        zbx_free(err);

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
zbx_uint64_t jatrap_auth_user(ja_telegram_object * obj)
{
    zbx_uint64_t userid;
    json_object *jp_data, *jp;
    char *username, *password, *err;
    const char *__function_name = "jatrap_auth_user";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    userid = 0;
    err = NULL;
    if (ja_telegram_check(obj) == FAIL)
        return userid;

    jp_data = json_object_object_get(obj->request, JA_PROTO_TAG_DATA);
    jp = json_object_object_get(jp_data, JA_PROTO_TAG_USERNAME);
    if (jp == NULL) {
        err =
            zbx_dsprintf(NULL,
                         "can not find the tag '%s' from the request telegram: %s",
                         JA_PROTO_TAG_USERNAME,
                         json_object_to_json_string(obj->request));
        goto error;
    }
    username = (char *) json_object_get_string(jp);
    jp = json_object_object_get(jp_data, JA_PROTO_TAG_PASSWORD);
    if (jp == NULL) {
        err =
            zbx_dsprintf(NULL,
                         "can not find the tag '%s' from the request telegram: %s",
                         JA_PROTO_TAG_PASSWORD,
                         json_object_to_json_string(obj->request));
        goto error;
    }
    password = (char *) json_object_get_string(jp);

    userid = ja_user_auth(username, password);
    if (userid == 0) {
        err =
            zbx_dsprintf(NULL, "user authentication error, username: %s",
                         username);
    }

  error:
    if (userid == 0) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() error: %s", __function_name,
                   err);
        ja_telegram_seterr(obj, err);
    }
    if (err != NULL)
        zbx_free(err);
    return userid;
}
