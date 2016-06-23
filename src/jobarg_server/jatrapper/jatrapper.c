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
** $Date:: 2013-06-21 09:43:04 +0900 #$
** $Revision: 4934 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include <json/json.h>

#include "common.h"
#include "comms.h"
#include "log.h"
#include "md5.h"
#include "zbxserver.h"
#include "zbxself.h"

#include "jacommon.h"
#include "jajobobject.h"
#include "javalue.h"
#include "jaflow.h"
#include "jastr.h"
#include "jalog.h"
#include "jastatus.h"
#include "jahost.h"
#include "jauser.h"
#include "jatelegram.h"
#include "jatrapkind.h"
#include "jatrapper.h"

extern unsigned char process_type;
extern char serverid[JA_SERVERID_LEN];
extern int CONFIG_ZABBIX_VERSION;

void init_exec_request(JOBARG_EXEC_REQUEST * er)
{
    int i;
    er->username = NULL;
    er->password = NULL;
    er->jobnetid = NULL;
    er->starttime = NULL;
    for (i = 0; i < 1024; i++) {
        er->env[i] = NULL;
        er->value[i] = NULL;
    }
}

void clean_exec_request(JOBARG_EXEC_REQUEST * er, int cnt)
{
    int i;
    zbx_free(er->username);
    zbx_free(er->password);
    zbx_free(er->jobnetid);
    zbx_free(er->starttime);
    if (cnt != 0) {
        for (i = 0; i < cnt; i++) {
            zbx_free(er->env[i]);
            zbx_free(er->value[i]);
        }
    }
}

void init_jobnetinfo(JOBARG_JOBNET_INFO * ji)
{
    ji->jobnetid = NULL;
    ji->jobnetname = NULL;
    ji->jobnetruntype = 0;
    ji->jobnetstatus = 0;
    ji->jobstatus = 0;
    ji->scheduled_time = 0;
    ji->start_time = 0;
    ji->end_time = 0;
}

int get_jobnet_info(zbx_uint64_t registrynumber,
                    JOBARG_EXEC_REQUEST * er, JOBARG_JOBNET_INFO * ji)
{
    DB_RESULT result;
    DB_ROW row;
    int ret = SUCCEED;
    result =
        DBselect
        ("select run_type, status, job_status, scheduled_time, start_time,"
         " end_time, jobnet_id, jobnet_name from ja_run_jobnet_summary_table"
         " where inner_jobnet_id = " ZBX_FS_UI64, registrynumber);
    row = DBfetch(result);
    if (row == NULL) {
        ja_log("JATRAPPER200054", 0, NULL, 0, registrynumber);
        ret = FAIL;
        return ret;
    }
    er->jobnetid = strdup(row[6]);
    ji->jobnetruntype = atoi(row[0]);
    ji->jobnetstatus = atoi(row[1]);
    ji->jobstatus = atoi(row[2]);
    ZBX_STR2UINT64(ji->scheduled_time, row[3]);
    ZBX_STR2UINT64(ji->start_time, row[4]);
    ZBX_STR2UINT64(ji->end_time, row[5]);
    ji->jobnetid = strdup(row[6]);
    ji->jobnetname = strdup(row[7]);
    return ret;
}

int time_passed_check(char *starttime)
{
    time_t now, t;

    now = time(NULL);
    t = ja_str2timestamp(starttime);

    if (t - now < 60)
        return FAIL;
    return SUCCEED;
}

int job_exec_auth(JOBARG_EXEC_REQUEST er)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    zbx_uint64_t userid, usrgrpid;
    char *jobnet_id;
    int user_type, public_flag, user_cmp;
    const char *__function_name = "job_exec_auth";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    public_flag = 0;
    user_cmp = 0;
    userid = ja_user_auth(er.username, er.password);
    if (userid == 0) {
        ja_log("JATRAPPER200008", 0, er.jobnetid, 0);
        return FAIL;
    }
    if (ja_user_status(userid) != 0) {
        ja_log("JATRAPPER200050", 0, er.jobnetid, 0, er.username);
        return FAIL;
    }
    user_type = ja_user_type(userid);
    usrgrpid = ja_user_usrgrpid(userid);

    ret = SUCCEED;
    jobnet_id = DBdyn_escape_string(er.jobnetid);
    result =
        DBselect
        ("select user_name, public_flag from ja_jobnet_control_table where jobnet_id = '%s' and valid_flag = 1",
         jobnet_id);
    row = DBfetch(result);
    if (row == NULL) {
        ja_log("JATRAPPER200046", 0, jobnet_id, 0, jobnet_id);
        ret = FAIL;
    } else {
        public_flag = atoi(row[1]);
        if (usrgrpid == ja_user_usrgrpid(ja_user_id(row[0]))
            && usrgrpid > 0)
            user_cmp = 1;
    }
    zbx_free(jobnet_id);
    DBfree_result(result);

    if (ret == FAIL)
        return FAIL;
    if (user_type == USER_TYPE_SUPER_ADMIN || public_flag == 1
        || user_cmp == 1)
        return SUCCEED;
    ja_log("JATRAPPER200047", 0, jobnet_id, 0, er.jobnetid);
    return FAIL;
}

int register_db_table(JOBARG_EXEC_REQUEST er,
                      zbx_uint64_t * inner_jobnet_id, int cnt)
{
    const char *__function_name = "register_db_table";
    char *user_name = NULL;
    char *jobnet_name = NULL;
    char *memo = NULL;
    char *jobnetid;
    char *env;
    char *value;
    int ret = SUCCEED;
    int res;
    int public_flag = 0;
    int run_type;
    int i;
    zbx_uint64_t next_id;
    zbx_uint64_t update_date;
    zbx_uint64_t scheduled_time;
    DB_RESULT result;
    DB_ROW row;
    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobnetid: [%s]",
               __function_name, er.jobnetid);
    jobnetid = DBdyn_escape_string(er.jobnetid);
    result =
        DBselect
        ("select update_date, created_date, public_flag, user_name, jobnet_name, memo"
         " from ja_jobnet_control_table" " where jobnet_id='%s'"
         " and valid_flag=1", jobnetid);
    row = DBfetch(result);
    if (row == NULL) {
        ja_log("JATRAPPER200014", 0, jobnetid, 0, jobnetid);
        zbx_free(jobnetid);
        DBfree_result(result);
        ret = FAIL;
        return ret;
    }
    ZBX_STR2UINT64(update_date, row[0]);
    public_flag = atoi(row[2]);
    user_name = DBdyn_escape_string(row[3]);
    jobnet_name = DBdyn_escape_string(row[4]);
    memo = DBdyn_escape_string(row[5]);
    DBfree_result(result);
    if (er.starttime != NULL) {
        run_type = JA_JOBNET_RUN_TYPE_SCHEDULED;
        ZBX_STR2UINT64(scheduled_time, er.starttime);
    } else {
        run_type = JA_JOBNET_RUN_TYPE_IMMEDIATE;
        scheduled_time = 0;
    }
    next_id = get_next_id(JA_RUN_ID_JOBNET_EX, 0, jobnetid);
    if (next_id == 0) {
        ja_log("JATRAPPER200015", 0, jobnetid, 0);
        zbx_free(jobnetid);
        zbx_free(user_name);
        zbx_free(jobnet_name);
        zbx_free(memo);
        ret = FAIL;
        return ret;
    }
    res = DBexecute("insert into ja_run_jobnet_table"
                    " (inner_jobnet_id, inner_jobnet_main_id,"
                    " update_date, run_type, main_flag, status, scheduled_time, public_flag,"
                    " jobnet_id, user_name, jobnet_name, memo)"
                    " values ( " ZBX_FS_UI64 ", " ZBX_FS_UI64 ", "
                    ZBX_FS_UI64 "," " %d, 0, 0, " ZBX_FS_UI64
                    ", %d, '%s', '%s', '%s', '%s')", next_id, next_id,
                    update_date, run_type, scheduled_time, public_flag,
                    jobnetid, user_name, jobnet_name, memo);
    if (ZBX_DB_OK > res) {
        ja_log("JATRAPPER200016", 0, jobnetid, 0, jobnetid);
        zbx_free(jobnetid);
        zbx_free(user_name);
        zbx_free(jobnet_name);
        zbx_free(memo);
        ret = FAIL;
        return ret;
    }
    for (i = 0; i < cnt; i++) {
        env = NULL;
        value = NULL;
        env = DBdyn_escape_string(er.env[i]);
        value = DBdyn_escape_string(er.value[i]);
        res = DBexecute("insert into ja_value_before_jobnet_table"
                        " (inner_jobnet_id, value_name, before_value)"
                        " values ( " ZBX_FS_UI64 ", '%s', '%s' )",
                        next_id, env, value);
        if (ZBX_DB_OK > res) {
            ja_log("JATRAPPER200016", 0, jobnetid, 0, jobnetid);
            zbx_free(jobnetid);
            zbx_free(user_name);
            zbx_free(jobnet_name);
            zbx_free(memo);
            zbx_free(env);
            zbx_free(value);
            ret = FAIL;
            return ret;
        }
        zbx_free(env);
        zbx_free(value);
    }
    *inner_jobnet_id = next_id;
    zbx_free(jobnetid);
    zbx_free(user_name);
    zbx_free(jobnet_name);
    zbx_free(memo);
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

void reply_jobresult_response(zbx_sock_t * sock, int ret, char *message)
{
    struct zbx_json json;

    /*JSON Data */
    zbx_json_init(&json, ZBX_JSON_STAT_BUF_LEN);
    zbx_json_addstring(&json, JA_PROTO_TAG_KIND,
                       JA_PROTO_VALUE_JOBRESULT_RES, ZBX_JSON_TYPE_STRING);
    zbx_json_adduint64(&json, JA_PROTO_TAG_VERSION, JA_PROTO_VALUE_VERSION_1);  /*data version */
    zbx_json_addstring(&json, JA_PROTO_TAG_SERVERID, serverid,
                       ZBX_JSON_TYPE_STRING);
    zbx_json_addobject(&json, JA_PROTO_TAG_DATA);
    zbx_json_adduint64(&json, JA_PROTO_TAG_RESULT, FAIL == ret ? 1 : 0);
    zbx_json_addstring(&json, JA_PROTO_TAG_MESSAGE, message,
                       ZBX_JSON_TYPE_STRING);
    zbx_json_close(&json);
    zbx_json_close(&json);
    zabbix_log(LOG_LEVEL_DEBUG, "JSON before sending [%s]", json.buffer);
    if (FAIL == (ret = zbx_tcp_send_to(sock, json.buffer, CONFIG_TIMEOUT))) {
        ja_log("JATRAPPER200038", 0, NULL, 0, zbx_tcp_strerror());
        ret = NETWORK_ERROR;
    }
    zbx_json_free(&json);
}

void reply_jobnetrun_response(zbx_sock_t * sock, int ret, char *message)
{
    struct zbx_json json;

    /*JSON Data */
    zbx_json_init(&json, ZBX_JSON_STAT_BUF_LEN);
    zbx_json_addstring(&json, JA_PROTO_TAG_KIND,
                       JA_PROTO_VALUE_JOBNETRUN_RES, ZBX_JSON_TYPE_STRING);
    zbx_json_adduint64(&json, JA_PROTO_TAG_VERSION, JA_PROTO_VALUE_VERSION_1);  /*data version */
    zbx_json_addstring(&json, JA_PROTO_TAG_SERVERID, serverid,
                       ZBX_JSON_TYPE_STRING);
    zbx_json_addobject(&json, JA_PROTO_TAG_DATA);
    zbx_json_adduint64(&json, JA_PROTO_TAG_RESULT, FAIL == ret ? 1 : 0);
    zbx_json_addstring(&json, JA_PROTO_TAG_MESSAGE, message,
                       ZBX_JSON_TYPE_STRING);
    zbx_json_close(&json);
    zbx_json_close(&json);
    zabbix_log(LOG_LEVEL_DEBUG, "JSON before sending [%s]", json.buffer);
    if (FAIL == (ret = zbx_tcp_send_to(sock, json.buffer, CONFIG_TIMEOUT))) {
        ja_log("JATRAPPER200038", 0, NULL, 0, zbx_tcp_strerror());
        ret = NETWORK_ERROR;
    }
    zbx_json_free(&json);
}

void reply_jobnetstatusrq_response(zbx_sock_t * sock, int ret,
                                   JOBARG_JOBNET_INFO * ji, char *message)
{
    struct zbx_json json;

    /*JSON Data */
    zbx_json_init(&json, ZBX_JSON_STAT_BUF_LEN);
    zbx_json_addstring(&json, JA_PROTO_TAG_KIND,
                       JA_PROTO_VALUE_JOBNETSTATUSRQ_RES,
                       ZBX_JSON_TYPE_STRING);
    zbx_json_adduint64(&json, JA_PROTO_TAG_VERSION, JA_PROTO_VALUE_VERSION_1);  /*data version */
    zbx_json_addstring(&json, JA_PROTO_TAG_SERVERID, serverid,
                       ZBX_JSON_TYPE_STRING);
    zbx_json_addobject(&json, JA_PROTO_TAG_DATA);
    zbx_json_adduint64(&json, JA_PROTO_TAG_RESULT, FAIL == ret ? 1 : 0);
    if (ret != FAIL) {
        zbx_json_addstring(&json, JA_PROTO_TAG_JOBNETID, ji->jobnetid,
                           ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&json, JA_PROTO_TAG_JOBNETNAME, ji->jobnetname,
                           ZBX_JSON_TYPE_STRING);
        zbx_json_adduint64(&json, JA_PROTO_TAG_SCHEDULEDTIME,
                           ji->scheduled_time);
        zbx_json_adduint64(&json, JA_PROTO_TAG_STARTTIME, ji->start_time);
        zbx_json_adduint64(&json, JA_PROTO_TAG_ENDTIME, ji->end_time);
        zbx_json_adduint64(&json, JA_PROTO_TAG_JOBNETRUNTYPE,
                           ji->jobnetruntype);
        zbx_json_adduint64(&json, JA_PROTO_TAG_JOBNETSTATUS,
                           ji->jobnetstatus);
        zbx_json_adduint64(&json, JA_PROTO_TAG_JOBSTATUS, ji->jobstatus);
    } else {
        zbx_json_addstring(&json, JA_PROTO_TAG_MESSAGE, message,
                           ZBX_JSON_TYPE_STRING);
    }
    zbx_json_close(&json);
    zbx_json_close(&json);
    zabbix_log(LOG_LEVEL_DEBUG, "JSON before sending [%s]", json.buffer);
    if (FAIL == (ret = zbx_tcp_send_to(sock, json.buffer, CONFIG_TIMEOUT))) {
        ja_log("JATRAPPER200038", 0, NULL, 0, zbx_tcp_strerror());
        ret = NETWORK_ERROR;
    }
    zbx_json_free(&json);
}

char *evaluate_jobnetrun(zbx_sock_t * sock, struct zbx_json_parse *jp,
                         int *ret)
{
    struct zbx_json_parse jp_row;
    struct zbx_json_parse jp_row2;
    char *message = NULL;
    char value[MAX_STRING_LEN];
    const char *p;
    const char *p2;
    const char *p3 = NULL;
    int version;
    int res;
    int i;
    zbx_uint64_t inner_jobnet_id;
    static JOBARG_EXEC_REQUEST er;
    init_exec_request(&er);
    if (SUCCEED ==
        zbx_json_value_by_name(jp, JA_PROTO_TAG_VERSION, value,
                               sizeof(value))) {
        version = atoi(value);
        if (version != JA_PROTO_VALUE_VERSION_1) {
            ja_log("JATRAPPER200027", 0, NULL, 0,
                   JA_PROTO_VALUE_VERSION_1);
            *ret = FAIL;
            return zbx_dsprintf(message,
                                "Received message error: [version] is not correct.");
        }
    } else {
        ja_log("JATRAPPER200028", 0, NULL, 0);
        *ret = FAIL;
        return zbx_dsprintf(message,
                            "Received message error: [version] not found");
    }
    if (NULL == (p = zbx_json_pair_by_name(jp, JA_PROTO_TAG_DATA))) {
        ja_log("JATRAPPER200021", 0, NULL, 0);
        *ret = FAIL;
        return zbx_dsprintf(message,
                            "Received message error: [data] not found");
    } else {
        if (FAIL == (*ret = zbx_json_brackets_open(p, &jp_row))) {
            ja_log("JATRAPPER200022", 0, NULL, 0);
            *ret = FAIL;
            return zbx_dsprintf(message,
                                "Received message error: Cannot open [data] object");
        } else {
            if (SUCCEED ==
                zbx_json_value_by_name(&jp_row, JA_PROTO_TAG_USERNAME,
                                       value, sizeof(value))) {
                er.username = strdup(value);
            } else {
                ja_log("JATRAPPER200032", 0, NULL, 0);
                *ret = FAIL;
                return zbx_dsprintf(message,
                                    "Received message error: [username] not found");
            }
            if (SUCCEED ==
                zbx_json_value_by_name(&jp_row, JA_PROTO_TAG_PASSWORD,
                                       value, sizeof(value))) {
                er.password = strdup(value);
            } else {
                ja_log("JATRAPPER200033", 0, NULL, 0);
                zbx_free(er.username);
                *ret = FAIL;
                return zbx_dsprintf(message,
                                    "Received message error: [password] not found");
            }
            if (SUCCEED ==
                zbx_json_value_by_name(&jp_row, JA_PROTO_TAG_JOBNETID,
                                       value, sizeof(value))) {
                er.jobnetid = strdup(value);
            } else {
                ja_log("JATRAPPER200034", 0, NULL, 0);
                zbx_free(er.username);
                zbx_free(er.password);
                *ret = FAIL;
                return zbx_dsprintf(message,
                                    "Received message error: [jobnetid] not found");
            }
            if (SUCCEED ==
                zbx_json_value_by_name(&jp_row, JA_PROTO_TAG_STARTTIME,
                                       value, sizeof(value))) {
                er.starttime = strdup(value);
                res = time_passed_check(er.starttime);
                if (res == FAIL) {
                    zbx_free(er.username);
                    zbx_free(er.password);
                    zbx_free(er.jobnetid);
                    zbx_free(er.starttime);
                    *ret = FAIL;
                    return zbx_dsprintf(message,
                                        "Received message error: [start_time] already passed.");
                }
            }
            i = 0;
            if (NULL !=
                (p2 = zbx_json_pair_by_name(&jp_row, JA_PROTO_TAG_ENV))) {
                if (FAIL == (*ret = zbx_json_brackets_open(p2, &jp_row2))) {
                    ja_log("JATRAPPER200051", 0, NULL, 0);
                    *ret = FAIL;
                    return zbx_dsprintf(message,
                                        "Received message error: Cannot open [env] object");
                } else {
                    while (NULL !=
                           (p3 =
                            zbx_json_pair_next(&jp_row2, p3, value,
                                               sizeof(value)))) {
                        er.env[i] = strdup(value);
                        zbx_json_value_by_name(&jp_row2, er.env[i], value,
                                               sizeof(value));
                        er.value[i] = strdup(value);
                        i++;
                    }
                    er.env[i] = '\0';
                }
            }
            if (SUCCEED == (job_exec_auth(er))) {
                DBbegin();
                if (SUCCEED ==
                    (register_db_table(er, &inner_jobnet_id, i))) {
                    DBcommit();
                    clean_exec_request(&er, i);
                    return zbx_dsprintf(message,
                                        "Registry number :  [" ZBX_FS_UI64
                                        "]", inner_jobnet_id);
                } else {
                    DBrollback();
                    clean_exec_request(&er, i);
                    *ret = FAIL;
                    return zbx_dsprintf(message, "Insert table error.");
                }
            } else {
                clean_exec_request(&er, i);
                *ret = FAIL;
                return zbx_dsprintf(message, "Authentication failure.");
            }
        }
    }
}

char *evaluate_jobnetstatusrq(zbx_sock_t * sock,
                              struct zbx_json_parse *jp, int *ret,
                              JOBARG_JOBNET_INFO * ji)
{
    struct zbx_json_parse jp_row;
    char *message = NULL;
    char value[MAX_STRING_LEN];
    const char *p;
    int version;
    zbx_uint64_t registrynumber;
    static JOBARG_EXEC_REQUEST er;
    init_exec_request(&er);
    if (SUCCEED ==
        zbx_json_value_by_name(jp, JA_PROTO_TAG_VERSION, value,
                               sizeof(value))) {
        version = atoi(value);
        if (version != JA_PROTO_VALUE_VERSION_1) {
            ja_log("JATRAPPER200027", 0, NULL, 0,
                   JA_PROTO_VALUE_VERSION_1);
            *ret = FAIL;
            return zbx_dsprintf(message,
                                "Received message error: [version] is not correct.");
        }
    } else {
        ja_log("JATRAPPER200028", 0, NULL, 0);
        *ret = FAIL;
        return zbx_dsprintf(message,
                            "Received message error: [version] not found");
    }
    if (NULL == (p = zbx_json_pair_by_name(jp, JA_PROTO_TAG_DATA))) {
        ja_log("JATRAPPER200029", 0, NULL, 0);
        *ret = FAIL;
        return zbx_dsprintf(message,
                            "Received message error: [data] not found");
    } else {
        if (FAIL == (*ret = zbx_json_brackets_open(p, &jp_row))) {
            ja_log("JATRAPPER200030", 0, NULL, 0);
            *ret = FAIL;
            return zbx_dsprintf(message,
                                "Received message error: Cannot open [data] object");
        } else {
            if (SUCCEED ==
                zbx_json_value_by_name(&jp_row, JA_PROTO_TAG_USERNAME,
                                       value, sizeof(value))) {
                er.username = strdup(value);
            } else {
                ja_log("JATRAPPER200032", 0, NULL, 0);
                *ret = FAIL;
                return zbx_dsprintf(message,
                                    "Received message error: [username] not found");
            }
            if (SUCCEED ==
                zbx_json_value_by_name(&jp_row, JA_PROTO_TAG_PASSWORD,
                                       value, sizeof(value))) {
                er.password = strdup(value);
            } else {
                ja_log("JATRAPPER200033", 0, NULL, 0);
                zbx_free(er.username);
                *ret = FAIL;
                return zbx_dsprintf(message,
                                    "Received message error: [password] not found");
            }
            if (SUCCEED ==
                zbx_json_value_by_name(&jp_row,
                                       JA_PROTO_TAG_REGISTRYNUMBER,
                                       value, sizeof(value))) {
                ZBX_STR2UINT64(registrynumber, value);
            } else {
                ja_log("JATRAPPER200052", 0, NULL, 0);
                zbx_free(er.username);
                zbx_free(er.password);
                *ret = FAIL;
                return zbx_dsprintf(message,
                                    "Received message error: [registrynumber] not found");
            }
            if (SUCCEED != get_jobnet_info(registrynumber, &er, ji)) {
                ja_log("JATRAPPER200053", 0, NULL, 0);
                zbx_free(er.username);
                zbx_free(er.password);
                *ret = FAIL;
                return zbx_dsprintf(message,
                                    "[registrynumber] is already cleaned up in DB");
            }
            if (SUCCEED == (job_exec_auth(er))) {
                zbx_free(er.username);
                zbx_free(er.password);
                *ret = SUCCEED;
                return zbx_dsprintf(message, "OK.");
            } else {
                zbx_free(er.username);
                zbx_free(er.password);
                *ret = FAIL;
                return zbx_dsprintf(message, "Authentication failure.");
            }
        }
    }
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
static int process_jatrap(zbx_sock_t * sock, char *s)
{
    int ret = SUCCEED;
    struct zbx_json_parse jp;
    char value[MAX_STRING_LEN];
    char *message = NULL;
    static JOBARG_JOBNET_INFO ji;
    zbx_rtrim(s, " \r\n");
    zabbix_log(LOG_LEVEL_DEBUG, "Jatrapper got [%s] len %zd", s,
               strlen(s));

    /*JSON Format check */
    if (SUCCEED != zbx_json_open(s, &jp)) {
        ja_log("JATRAPPER200037", 0, NULL, 0);
        message =
            zbx_dsprintf(message,
                         "Received message error: [JSON format error]");
        ret = FAIL;
        reply_jobresult_response(sock, ret, message);
        zbx_free(message);
        return ret;
    }

    /*[kind] check */
    if (SUCCEED !=
        zbx_json_value_by_name(&jp, JA_PROTO_TAG_KIND, value,
                               sizeof(value))) {
        ja_log("JATRAPPER200036", 0, NULL, 0);
        message =
            zbx_dsprintf(message,
                         "Received message error: [kind] not found");
        ret = FAIL;
        reply_jobresult_response(sock, ret, message);
        zbx_free(message);
        return ret;
    }
    if (0 == strcmp(value, JA_PROTO_VALUE_JOBRESULT)) {
        /*from Job agent */
    } else if (0 == strcmp(value, JA_PROTO_VALUE_JOBNETRUN)) {
        /*from external I/F */
        message = evaluate_jobnetrun(sock, &jp, &ret);
        reply_jobnetrun_response(sock, ret, message);
    } else if (0 == strcmp(value, JA_PROTO_VALUE_JOBNETSTATUSRQ)) {
        /*from jobnet-status request */
        init_jobnetinfo(&ji);
        message = evaluate_jobnetstatusrq(sock, &jp, &ret, &ji);
        reply_jobnetstatusrq_response(sock, ret, &ji, message);
    } else {
        ja_log("JATRAPPER200035", 0, NULL, 0);
        message =
            zbx_dsprintf(message,
                         "Received message error: [kind] is not correct.");
        ret = FAIL;
        reply_jobresult_response(sock, ret, message);
    }
    zbx_free(message);
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
static void jatraper_reply(zbx_sock_t * sock, ja_telegram_object * obj)
{
    char *data;
    const char *__function_name = "jatraper_reply";
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    json_object_object_add(obj->response, JA_PROTO_TAG_KIND,
                           json_object_new_string("response"));
    json_object_object_add(obj->response, JA_PROTO_TAG_VERSION,
                           json_object_new_int(JA_PROTO_TELE_VERSION));
    json_object_object_add(obj->response, JA_PROTO_TAG_SERVERID,
                           json_object_new_string(serverid));
    data = (char *) json_object_to_json_string(obj->response);

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() %s", __function_name, data);
    if (zbx_tcp_send_to(sock, data, CONFIG_TIMEOUT) == FAIL) {
        ja_log("JATRAPPER200038", 0, NULL, 0, zbx_tcp_strerror());
    }
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
static void process_jatrapper(zbx_sock_t * sock)
{
    ja_telegram_object obj;
    char *data;
    const char *__function_name = "process_jatrapper";
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    ja_telegram_new(&obj);
    if (ja_telegram_recv(&obj, sock, CONFIG_TIMEOUT) == FAIL)
        goto error;

    data = (char *) json_object_to_json_string(obj.request);
    DBconnect(ZBX_DB_CONNECT_NORMAL);
    if (jatrap_kind(sock, &obj) == SUCCEED) {
        jatraper_reply(sock, &obj);
    } else {
        process_jatrap(sock, data);
    }
    DBclose();

  error:
    ja_telegram_clear(&obj);
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
void main_jatrapper_loop(zbx_sock_t * s)
{
    const char *__function_name = "main_jatrapper_loop";
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    zbx_setproctitle("%s [connecting to the database]",
                     ja_get_process_type_string(process_type));
    for (;;) {
        zbx_setproctitle("%s [waiting for connection]",
                         ja_get_process_type_string(process_type));
        ja_update_selfmon_counter(ZBX_PROCESS_STATE_IDLE);
        if (SUCCEED == zbx_tcp_accept(s)) {
            ja_update_selfmon_counter(ZBX_PROCESS_STATE_BUSY);
            zbx_setproctitle("%s [processing data]",
                             get_process_type_string(process_type));
            process_jatrapper(s);
            zbx_tcp_unaccept(s);
        } else {
            ja_log("JATRAPPER200039", 0, NULL, 0);
        }
    }
}
