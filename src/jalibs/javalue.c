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
** $Date:: 2014-02-04 13:21:01 +0900 #$
** $Revision: 5771 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jadb.h"
#include "javalue.h"

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
int ja_clean_value_before(const zbx_uint64_t inner_job_id)
{
    int db_ret;
    DB_RESULT result;
    DB_ROW row;
    char value_name[130];
    const char *__function_name = "ja_clean_value_before";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    zbx_snprintf(value_name, sizeof(value_name), "");
    result =
        DBselect
        ("select seq_no, value_name from ja_run_value_before_table "
         " where inner_job_id = " ZBX_FS_UI64
         " order by value_name, seq_no desc", inner_job_id);

    while (NULL != (row = DBfetch(result))) {
        if (strcmp(row[1], value_name) != 0) {
            zbx_snprintf(value_name, sizeof(value_name), "%s", row[1]);
            continue;
        }
        db_ret =
            DBexecute
            ("delete from ja_run_value_before_table where seq_no = %s",
             row[0]);
        if (db_ret < ZBX_DB_OK) {
            DBfree_result(result);
            return FAIL;
        }
    }
    DBfree_result(result);

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
int ja_clean_value_after(const zbx_uint64_t inner_job_id)
{
    int db_ret;
    DB_RESULT result;
    DB_ROW row;
    char value_name[130];
    const char *__function_name = "ja_clean_value_after";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    zbx_snprintf(value_name, sizeof(value_name), "");
    result =
        DBselect("select seq_no, value_name from ja_run_value_after_table "
                 " where inner_job_id = " ZBX_FS_UI64
                 " order by value_name, seq_no desc", inner_job_id);

    while (NULL != (row = DBfetch(result))) {
        if (strcmp(row[1], value_name) != 0) {
            zbx_snprintf(value_name, sizeof(value_name), "%s", row[1]);
            continue;
        }
        db_ret =
            DBexecute
            ("delete from ja_run_value_after_table where seq_no = %s",
             row[0]);
        if (db_ret < ZBX_DB_OK) {
            DBfree_result(result);
            return FAIL;
        }
    }
    DBfree_result(result);

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
int ja_set_value_before(const zbx_uint64_t inner_job_id,
                        const zbx_uint64_t inner_jobnet_id,
                        const char *value_name, const char *before_value)
{
    int ret, db_ret;
    char *value_name_esc, *before_value_esc;
    const char *__function_name = "ja_set_value_before";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64 ", inner_jobnet_id: "
               ZBX_FS_UI64, __function_name, inner_job_id,
               inner_jobnet_id);

    if (inner_job_id <= 0 || inner_jobnet_id <= 0 || value_name == NULL)
        return FAIL;

    ret = SUCCEED;
    value_name_esc = DBdyn_escape_string(value_name);
    if (before_value == NULL) {
        before_value_esc = DBdyn_escape_string("");
    } else {
        before_value_esc = DBdyn_escape_string(before_value);
    }

    db_ret =
        DBexecute("insert into ja_run_value_before_table"
                  " (inner_job_id, inner_jobnet_id, value_name, before_value) values ("
                  ZBX_FS_UI64 ", " ZBX_FS_UI64 ", '%s', '%s')",
                  inner_job_id, inner_jobnet_id, value_name_esc,
                  before_value_esc);
    if (db_ret < ZBX_DB_OK) {
        ja_log("JAVALUE300001", inner_jobnet_id, NULL, inner_job_id,
               __function_name, inner_job_id, inner_jobnet_id);
        ret = FAIL;
    }

    zbx_free(value_name_esc);
    zbx_free(before_value_esc);
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
int ja_set_value_after(const zbx_uint64_t inner_job_id,
                       const zbx_uint64_t inner_jobnet_id,
                       const char *value_name, const char *after_value)
{
    int ret, db_ret;
    char *value_name_esc, *after_value_esc;
    const char *__function_name = "ja_set_value_after";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64 ", inner_jobnet_id: "
               ZBX_FS_UI64, __function_name, inner_job_id,
               inner_jobnet_id);

    if (inner_job_id <= 0 || inner_jobnet_id <= 0 || value_name == NULL)
        return FAIL;

    ret = SUCCEED;
    value_name_esc = DBdyn_escape_string(value_name);
    if (after_value == NULL) {
        after_value_esc = DBdyn_escape_string("");
    } else {
        after_value_esc = DBdyn_escape_string(after_value);
    }

    db_ret =
        DBexecute("insert into ja_run_value_after_table"
                  " (inner_job_id, inner_jobnet_id, value_name, after_value) values ("
                  ZBX_FS_UI64 ", " ZBX_FS_UI64 ", '%s', '%s')",
                  inner_job_id, inner_jobnet_id, value_name_esc,
                  after_value_esc);

    if (db_ret < ZBX_DB_OK) {
        ja_log("JAVALUE300001", inner_jobnet_id, NULL, inner_job_id,
               __function_name, inner_job_id, inner_jobnet_id);
        ret = FAIL;
    }

    zbx_free(value_name_esc);
    zbx_free(after_value_esc);
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
int ja_cpy_value(const zbx_uint64_t inner_job_id, const char *value_src,
                 char *value_dest)
{
    const char *__function_name = "ja_cpy_value";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);

    if (value_src == NULL)
        return FAIL;

    if (*value_src == '$') {
        return ja_get_value_before(inner_job_id, value_src + 1,
                                   value_dest);
    }
    zbx_snprintf(value_dest, strlen(value_src) + 1, "%s", value_src);

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
int ja_get_value_before(const zbx_uint64_t inner_job_id,
                        const char *value_name, char *before_value)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    char *value_name_esc;
    const char *__function_name = "ja_get_value_before";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);

    if (value_name == NULL)
        return FAIL;

    ret = SUCCEED;
    value_name_esc = DBdyn_escape_string(value_name);
    result =
        DBselect
        ("select before_value from ja_run_value_before_table "
         " where inner_job_id = " ZBX_FS_UI64 " and value_name = '%s'",
         inner_job_id, value_name_esc);
    row = DBfetch(result);
    if (row == NULL) {
        ret = FAIL;
    } else {
        if (row[0] == NULL) {
            ret = FAIL;
        } else {
            zbx_snprintf(before_value, strlen(row[0]) + 1, "%s", row[0]);
        }
    }
    DBfree_result(result);
    zbx_free(value_name_esc);

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
int ja_get_value_after(const zbx_uint64_t inner_job_id,
                       const char *value_name, char *after_value)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    char *value_name_esc;
    const char *__function_name = "ja_get_value_after";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);

    if (value_name == NULL)
        return FAIL;

    ret = SUCCEED;
    value_name_esc = DBdyn_escape_string(value_name);
    result =
        DBselect
        ("select after_value from ja_run_value_after_table "
         " where inner_job_id = " ZBX_FS_UI64 " and value_name = '%s'",
         inner_job_id, value_name_esc);
    row = DBfetch(result);
    if (row == NULL) {
        ret = FAIL;
    } else {
        if (row[0] == NULL) {
            ret = FAIL;
        } else {
            zbx_snprintf(after_value, strlen(row[0]) + 1, "%s", row[0]);
        }
    }
    DBfree_result(result);
    zbx_free(value_name_esc);

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
int ja_remove_value_before(const zbx_uint64_t inner_job_id,
                           const char *value_name)
{
    int db_ret;
    char *value_name_esc;
    const char *__function_name = "ja_remove_value_before";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);

    if (value_name == NULL)
        return FAIL;

    value_name_esc = DBdyn_escape_string(value_name);
    db_ret =
        DBexecute
        ("delete from ja_run_value_before_table "
         " where inner_job_id = " ZBX_FS_UI64 " and value_name = '%s'",
         inner_job_id, value_name_esc);
    zbx_free(value_name_esc);

    if (ZBX_DB_OK > db_ret)
        return FAIL;
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
int ja_remove_value_after(const zbx_uint64_t inner_job_id,
                          const char *value_name)
{
    int db_ret;
    char *value_name_esc;
    const char *__function_name = "ja_remove_value_after";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);

    if (value_name == NULL)
        return FAIL;

    value_name_esc = DBdyn_escape_string(value_name);
    db_ret =
        DBexecute
        ("delete from ja_run_value_after_table "
         " where inner_job_id = " ZBX_FS_UI64 " and value_name = '%s'",
         inner_job_id, value_name_esc);
    zbx_free(value_name_esc);

    if (ZBX_DB_OK > db_ret)
        return FAIL;
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
int ja_value_before_after(const zbx_uint64_t inner_job_id)
{
    int db_ret;
    const char *__function_name = "ja_value_before_after";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    db_ret =
        DBexecute("insert into ja_run_value_after_table"
                  " (inner_job_id, inner_jobnet_id, value_name, after_value)"
                  " select inner_job_id, inner_jobnet_id, value_name, before_value"
                  " from ja_run_value_before_table where inner_job_id = "
                  ZBX_FS_UI64, inner_job_id);

    if (db_ret < ZBX_DB_OK) {
        ja_log("JAVALUE300003", 0, NULL, inner_job_id, __function_name,
               inner_job_id);
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
int ja_value_after_before(const zbx_uint64_t inner_job_id,
                          const zbx_uint64_t next_inner_job_id)
{
    int db_ret;
    const char *__function_name = "ja_value_after_before";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64
               ", next_inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id, next_inner_job_id);

    db_ret =
        DBexecute("insert into ja_run_value_before_table"
                  " (inner_job_id, inner_jobnet_id, value_name, before_value) select "
                  ZBX_FS_UI64 ", inner_jobnet_id, value_name, after_value"
                  " from ja_run_value_after_table where inner_job_id = "
                  ZBX_FS_UI64, next_inner_job_id, inner_job_id);

    if (db_ret < ZBX_DB_OK) {
        ja_log("JAVALUE300003", 0, NULL, inner_job_id, __function_name,
               inner_job_id);
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
int ja_clean_value_jobnet_before(const zbx_uint64_t inner_jobnet_id)
{
    int db_ret;
    DB_RESULT result;
    DB_ROW row;
    char value_name[130];
    const char *__function_name = "ja_clean_value_jobnet_before";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_jobnet_id: " ZBX_FS_UI64,
               __function_name, inner_jobnet_id);

    zbx_snprintf(value_name, sizeof(value_name), "");
    result =
        DBselect
        ("select seq_no, value_name from ja_value_before_jobnet_table "
         " where inner_jobnet_id = " ZBX_FS_UI64
         " order by value_name, seq_no desc", inner_jobnet_id);

    while (NULL != (row = DBfetch(result))) {
        if (strcmp(row[1], value_name) != 0) {
            zbx_snprintf(value_name, sizeof(value_name), "%s", row[1]);
            continue;
        }
        db_ret =
            DBexecute
            ("delete from ja_value_before_jobnet_table where seq_no = %s",
             row[0]);
        if (db_ret < ZBX_DB_OK) {
            DBfree_result(result);
            return FAIL;
        }
    }
    DBfree_result(result);

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
int ja_clean_value_jobnet_after(const zbx_uint64_t inner_jobnet_id)
{
    int db_ret;
    DB_RESULT result;
    DB_ROW row;
    char value_name[130];
    const char *__function_name = "ja_clean_value_jobnet_after";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_jobnet_id: " ZBX_FS_UI64,
               __function_name, inner_jobnet_id);

    zbx_snprintf(value_name, sizeof(value_name), "");
    result =
        DBselect
        ("select seq_no, value_name from ja_value_after_jobnet_table "
         " where inner_jobnet_id = " ZBX_FS_UI64
         " order by value_name, seq_no desc", inner_jobnet_id);

    while (NULL != (row = DBfetch(result))) {
        if (strcmp(row[1], value_name) != 0) {
            zbx_snprintf(value_name, sizeof(value_name), "%s", row[1]);
            continue;
        }
        db_ret =
            DBexecute
            ("delete from ja_value_after_jobnet_table where seq_no = %s",
             row[0]);
        if (db_ret < ZBX_DB_OK) {
            DBfree_result(result);
            return FAIL;
        }
    }
    DBfree_result(result);

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
int ja_set_value_jobnet_before(const zbx_uint64_t inner_jobnet_id,
                               const char *value_name,
                               const char *before_value)
{
    int ret, db_ret;
    char *value_name_esc, *before_value_esc;
    const char *__function_name = "ja_set_value_jobnet_before";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobnet_id: " ZBX_FS_UI64, __function_name,
               inner_jobnet_id);

    if (inner_jobnet_id <= 0 || value_name == NULL)
        return FAIL;
    ret = SUCCEED;
    value_name_esc = DBdyn_escape_string(value_name);
    if (before_value == NULL) {
        before_value_esc = DBdyn_escape_string("");
    } else {
        before_value_esc = DBdyn_escape_string(before_value);
    }

    db_ret =
        DBexecute("insert into ja_value_before_jobnet_table"
                  " (inner_jobnet_id, value_name, before_value) values ("
                  ZBX_FS_UI64 ", '%s', '%s')", inner_jobnet_id,
                  value_name_esc, before_value_esc);

    if (db_ret < ZBX_DB_OK) {
        ja_log("JAVALUE300004", inner_jobnet_id, NULL, 0, __function_name,
               inner_jobnet_id, value_name, before_value);
        ret = FAIL;
    }

    zbx_free(value_name_esc);
    zbx_free(before_value_esc);
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
int ja_get_value_jobnet_before(const zbx_uint64_t inner_jobnet_id,
                               const char *value_name, char *before_value)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    char *value_name_esc;
    const char *__function_name = "ja_get_value_jobnet_before";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobnet_id: " ZBX_FS_UI64, __function_name,
               inner_jobnet_id);

    if (value_name == NULL)
        return FAIL;

    ret = SUCCEED;
    value_name_esc = DBdyn_escape_string(value_name);
    result =
        DBselect
        ("select before_value from ja_value_before_jobnet_table "
         " where inner_jobnet_id = " ZBX_FS_UI64 " and value_name = '%s'",
         inner_jobnet_id, value_name_esc);
    row = DBfetch(result);
    if (row == NULL) {
        ret = FAIL;
    } else {
        if (row[0] == NULL) {
            ret = FAIL;
        } else {
            zbx_snprintf(before_value, strlen(row[0]) + 1, "%s", row[0]);
        }
    }
    DBfree_result(result);
    zbx_free(value_name_esc);

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
int ja_value_before_jobnet_in(const zbx_uint64_t inner_job_id,
                              const zbx_uint64_t inner_jobnet_id)
{
    int db_ret;
    const char *__function_name = "ja_value_before_jobnet_in";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64 ", inner_jobnet_id: "
               ZBX_FS_UI64, __function_name, inner_job_id,
               inner_jobnet_id);

    db_ret =
        DBexecute("insert into ja_value_before_jobnet_table"
                  " (inner_jobnet_id, value_name, before_value) select "
                  ZBX_FS_UI64 ", value_name, before_value"
                  " from ja_run_value_before_table where inner_job_id = "
                  ZBX_FS_UI64, inner_jobnet_id, inner_job_id);

    if (db_ret < ZBX_DB_OK) {
        ja_log("JAVALUE300005", inner_jobnet_id, NULL, inner_job_id,
               __function_name, inner_jobnet_id, inner_job_id);
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
int ja_value_before_jobnet_out(const zbx_uint64_t inner_jobnet_id,
                               const zbx_uint64_t inner_job_id)
{
    int db_ret;
    const char *__function_name = "ja_value_before_jobnet_out";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobnet_id: " ZBX_FS_UI64 ", inner_job_id: "
               ZBX_FS_UI64, __function_name, inner_jobnet_id,
               inner_job_id);

    db_ret =
        DBexecute("insert into ja_run_value_before_table"
                  " (inner_job_id, inner_jobnet_id, value_name, before_value) select "
                  ZBX_FS_UI64 ", " ZBX_FS_UI64 ", value_name, before_value"
                  " from ja_value_before_jobnet_table where inner_jobnet_id = "
                  ZBX_FS_UI64, inner_job_id, inner_jobnet_id,
                  inner_jobnet_id);

    if (db_ret < ZBX_DB_OK) {
        ja_log("JAVALUE300005", inner_jobnet_id, NULL, inner_job_id,
               __function_name, inner_jobnet_id, inner_job_id);
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
int ja_get_jobnet_summary_start(const zbx_uint64_t inner_jobnet_id,
                                char *start_time)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_get_jobnet_summary_start";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_jobinet_id: " ZBX_FS_UI64, __function_name,
               inner_jobnet_id);

    ret = SUCCEED;
    result =
        DBselect
        ("select start_time from ja_run_jobnet_summary_table "
         " where inner_jobnet_id = " ZBX_FS_UI64, inner_jobnet_id);
    row = DBfetch(result);
    if (row == NULL) {
        ret = FAIL;
    } else {
        if (row[0] == NULL) {
            ret = FAIL;
        } else {
            zbx_snprintf(start_time, 16, "%s", row[0]);
        }
    }
    DBfree_result(result);

    if (ret == FAIL) {
        ja_log("JAVALUE300002", inner_jobnet_id, NULL, 0, __function_name,
               inner_jobnet_id);
    }
    return ret;
}
