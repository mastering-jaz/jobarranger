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
** $Date:: 2014-03-18 09:52:42 +0900 #$
** $Revision: 5909 $
** $Author: nagata@FITECHLABS.CO.JP $
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

/******************************************************************************
 *                                                                            *
 * Function: ja_user_lang                                                     *
 *                                                                            *
 * Purpose: get the user language to the key user id                          *
 *                                                                            *
 * Parameters: userid (in) - search target user id                            *
 *                                                                            *
 * Return value: zabbix user language (convert to lower case)                 *
 *                                                                            *
 * Comments: return the "en_gb" if the user id is not found                   *
 *                                                                            *
 ******************************************************************************/
char *ja_user_lang(zbx_uint64_t userid)
{
    DB_RESULT result;
    DB_ROW row;
    char *p;
    char *lang;
    const char *__function_name = "ja_user_lang";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() userid: " ZBX_FS_UI64, __function_name, userid);

    result = DBselect("select lang from users where userid = " ZBX_FS_UI64, userid);

    row = DBfetch(result);
    if (row == NULL) {
        zabbix_log(LOG_LEVEL_WARNING, "In %s() cat not find the user: " ZBX_FS_UI64, __function_name, userid);
        lang = strdup("en_gb");
    } else {
        lang = strdup(row[0]);
        for (p = lang; *p != '\0'; p++) {
            *p = tolower(*p);
        }
    }

    DBfree_result(result);
    return lang;
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
int ja_user_groups(zbx_uint64_t userid1, zbx_uint64_t userid2)
{
    int cnt;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_user_groups";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() userid1: " ZBX_FS_UI64 " userid2: " ZBX_FS_UI64,
               __function_name, userid1, userid2);

    cnt = 0;
    result =
        DBselect
        ("select count(*) from users_groups g1, users_groups g2 where g1.usrgrpid = g2.usrgrpid and g1.userid = "
         ZBX_FS_UI64 " and g2.userid = " ZBX_FS_UI64, userid1, userid2);
    row = DBfetch(result);
    if (row != NULL) {
        cnt = atoi(row[0]);
    }

    DBfree_result(result);
    return cnt;
}
