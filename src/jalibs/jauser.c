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
** $Date:: 2013-06-19 10:27:36 +0900 #$
** $Revision: 4928 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "db.h"
#include "log.h"

#include "jacommon.h"
#include "jastr.h"

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
zbx_uint64_t ja_user_auth(const char *username, const char *password)
{
    zbx_uint64_t userid;
    char *md5_password;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_user_auth";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() username: %s, password: %s",
               __function_name, username, password);
    userid = 0;
    md5_password = ja_md5(password);
    if (md5_password == NULL)
        return userid;
    result =
        DBselect
        ("select userid from users where alias = '%s' and passwd = '%s'",
         username, md5_password);
    row = DBfetch(result);
    if (row == NULL) {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() user authentication is error. username: %s, password: %s",
                   __function_name, username, password);
    } else {
        ZBX_STR2UINT64(userid, row[0]);
    }

    zbx_free(md5_password);
    DBfree_result(result);
    return userid;
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
zbx_uint64_t ja_user_id(const char *username)
{
    zbx_uint64_t userid;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_user_id";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() username: %s", __function_name,
               username);
    userid = 0;
    result =
        DBselect("select userid from users where alias = '%s'", username);
    row = DBfetch(result);
    if (row == NULL) {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() user name is error. username: %s",
                   __function_name, username);
    } else {
        ZBX_STR2UINT64(userid, row[0]);
    }

    DBfree_result(result);
    return userid;
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
zbx_uint64_t ja_user_usrgrpid(zbx_uint64_t userid)
{
    zbx_uint64_t usrgrpid;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_user_usrgrpid";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() userid: " ZBX_FS_UI64,
               __function_name, userid);
    usrgrpid = 0;
    result =
        DBselect
        ("select usrgrpid from users_groups where userid = " ZBX_FS_UI64,
         userid);
    row = DBfetch(result);
    if (row == NULL) {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() can not get the usrgrpid. userid: "
                   ZBX_FS_UI64, __function_name, userid);
    } else {
        ZBX_STR2UINT64(usrgrpid, row[0]);
    }

    DBfree_result(result);
    return usrgrpid;
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
int ja_user_status(zbx_uint64_t userid)
{
    int status;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_user_status";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() userid: " ZBX_FS_UI64,
               __function_name, userid);
    status = -1;
    result =
        DBselect
        ("select g.users_status from users_groups ug, usrgrp g where ug.usrgrpid = g.usrgrpid and ug.userid = "
         ZBX_FS_UI64, userid);

    row = DBfetch(result);
    if (row == NULL) {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() cat not find the user: " ZBX_FS_UI64,
                   __function_name, userid);
    } else {
        status = atoi(row[0]);
    }

    DBfree_result(result);
    return status;
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
int ja_user_type(zbx_uint64_t userid)
{
    int type;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_user_type";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() userid: " ZBX_FS_UI64,
               __function_name, userid);
    type = -1;
    result =
        DBselect("select type from users where userid = " ZBX_FS_UI64,
                 userid);

    row = DBfetch(result);
    if (row == NULL) {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() cat not find the user: " ZBX_FS_UI64,
                   __function_name, userid);
    } else {
        type = atoi(row[0]);
    }

    DBfree_result(result);
    return type;
}
