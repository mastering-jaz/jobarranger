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
** $Date:: 2013-04-25 08:35:48 +0900 #$
** $Revision: 4472 $
** $Author: komatsu@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"

#include "jacommon.h"
#include "jastr.h"
#include "jajobobject.h"
#include "jasqlite.h"
#include "jadbmain.h"
#include "jajournal.h"
#include "jajobdb.h"

const char *sql_jobs_create =
    "CREATE TABLE IF NOT EXISTS jobs (                                           "
    "jobid                    bigint                                    NOT NULL,"
    "version                  integer         DEFAULT '0'               NOT NULL,"
    "serverid                 varchar(16)     DEFAULT ''                NOT NULL,"
    "method                   integer         DEFAULT '0'               NOT NULL,"
    "type                     varchar(256)    DEFAULT ''                NOT NULL,"
    "argument                 varchar(4096)                                     ,"
    "script                   varchar(4096)   DEFAULT ''                NOT NULL,"
    "env                      varchar(4096)                                     ,"
    "result                   integer         DEFAULT '0'               NOT NULL,"
    "status                   integer         DEFAULT '0'               NOT NULL,"
    "pid                      integer                                           ,"
    "start_time               bigint          DEFAULT '0'               NOT NULL,"
    "end_time                 bigint          DEFAULT '0'               NOT NULL,"
    "message                  varchar(1024)                                     ,"
    "std_out                  varchar(4096)                                     ,"
    "std_err                  varchar(4096)                                     ,"
    "return_code              integer                                           ,"
    "signal                   integer                                           ,"
    "PRIMARY KEY (jobid)                                                       );";
const char *sql_index_1 =
    "CREATE INDEX IF NOT EXISTS jobs_method_index ON jobs (method);";
const char *sql_index_2 =
    "CREATE INDEX IF NOT EXISTS jobs_status_index ON jobs (status);";

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
int ja_jobdb_init()
{
    char dbjournal[JA_MAX_STRING_LEN];
    char dbbackup[JA_MAX_STRING_LEN];
    const char *__function_name = "ja_jobdb_init";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    zbx_snprintf(dbjournal, sizeof(dbjournal), "%s.jajournal",
                 CONFIG_DATABASE_FILE);
    zbx_snprintf(dbbackup, sizeof(dbbackup), "%s.backup",
                 CONFIG_DATABASE_FILE);

    if (ja_journal_create(dbjournal) != SUCCEED)
        return FAIL;
    if (ja_dbmain_create(dbbackup) != SUCCEED)
        return FAIL;
    if (ja_db_connect(CONFIG_DATABASE_FILE) != SUCCEED)
        return FAIL;

    if (ja_db_begin() != SUCCEED)
        goto restore;
    if (ja_db_execute("%s", sql_jobs_create) != SUCCEED)
        goto restore;
    if (ja_db_execute("%s", sql_index_1) != SUCCEED)
        goto restore;
    if (ja_db_execute("%s", sql_index_2) != SUCCEED)
        goto restore;
    if (ja_db_commit() != SUCCEED)
        goto restore;
    if (ja_db_execute("%s", "select * from jobs") != SUCCEED)
        goto restore;

    return SUCCEED;
  restore:
    zabbix_log(LOG_LEVEL_ERR, "Restoring database ...");
    ja_db_close();
    ja_dbmain_restore();
    return FAIL;
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
int ja_jobdb_load(DB_ROW row, ja_job_object * job)
{
    const char *__function_name = "ja_jobdb_load";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    if (row == NULL || job == NULL)
        return FAIL;
    ja_job_object_init(job);

    if (row[0] != NULL)
        str2uint64(row[0], "KMGT", &(job->jobid));
    if (row[1] != NULL)
        job->version = atoi(row[1]);
    if (row[2] != NULL)
        zbx_snprintf(job->serverid, sizeof(job->serverid), "%s", row[2]);
    if (row[3] != NULL)
        job->method = atoi(row[3]);
    if (row[4] != NULL)
        zbx_snprintf(job->type, sizeof(job->type), "%s", row[4]);
    if (row[5] != NULL)
        zbx_snprintf(job->argument, sizeof(job->argument), "%s", row[5]);
    if (row[6] != NULL)
        zbx_snprintf(job->script, sizeof(job->script), "%s", row[6]);
    if (row[7] != NULL)
        zbx_snprintf(job->env, sizeof(job->env), "%s", row[7]);
    if (row[8] != NULL)
        job->result = atoi(row[8]);
    if (row[9] != NULL)
        job->status = atoi(row[9]);
    if (row[10] != NULL)
        job->pid = atol(row[10]);
    if (row[11] != NULL)
        str2uint64(row[11], "KMGT", &(job->start_time));
    if (row[12] != NULL)
        str2uint64(row[12], "KMGT", &(job->end_time));
    if (row[13] != NULL)
        zbx_snprintf(job->message, sizeof(job->message), "%s", row[13]);
    if (row[14] != NULL)
        zbx_snprintf(job->std_out, sizeof(job->std_out), "%s", row[14]);
    if (row[15] != NULL)
        zbx_snprintf(job->std_err, sizeof(job->std_err), "%s", row[15]);
    if (row[16] != NULL)
        job->return_code = atoi(row[16]);
    if (row[17] != NULL)
        job->signal = atoi(row[17]);

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
int ja_jobdb_load_jobid(const zbx_uint64_t jobid, ja_job_object * job)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_jobdb_load_jobid";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64,
               __function_name, jobid);
    if (job == NULL)
        return FAIL;

    ret = FAIL;
    result =
        ja_db_select("select * from jobs where jobid = " ZBX_FS_UI64,
                     jobid);
    row = ja_db_fetch(result);
    if (row != NULL) {
        ret = ja_jobdb_load(row, job);
    } else {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() can not find jobid: " ZBX_FS_UI64,
                   __function_name, jobid);
    }
    ja_db_free_result(result);

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
int ja_jobdb_get_status(const zbx_uint64_t jobid)
{
    int status;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_jobdb_get_status";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64,
               __function_name, jobid);

    status = JA_AGENT_STATUS_INIT;
    result =
        ja_db_select("select status from jobs where jobid = "
                     ZBX_FS_UI64, jobid);
    row = ja_db_fetch(result);
    if (row != NULL) {
        status = atoi(row[0]);
    } else {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() can not find jobid: " ZBX_FS_UI64,
                   __function_name, jobid);
    }
    ja_db_free_result(result);

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
JA_PID ja_jobdb_get_pid(const zbx_uint64_t jobid)
{
    JA_PID pid;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_jobdb_get_pid";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64,
               __function_name, jobid);

    pid = 0;
    result =
        ja_db_select("select pid from jobs where jobid = "
                     ZBX_FS_UI64, jobid);
    row = ja_db_fetch(result);
    if (row != NULL) {
        pid = atoi(row[0]);
    } else {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() can not find jobid: " ZBX_FS_UI64,
                   __function_name, jobid);
    }
    ja_db_free_result(result);

    return pid;
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
int ja_jobdb_replace(ja_job_object * job)
{
    int ret;
    char *serverid_esc, *type_esc;
    char *argument_esc, *script_esc, *env_esc;
    char *message_esc, *std_out_esc, *std_err_esc;
    const char *__function_name = "ja_jobdb_replace";

    if (job == NULL)
        return FAIL;
    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64,
               __function_name, job->jobid);

    serverid_esc = ja_db_dyn_escape_string(job->serverid);
    type_esc = ja_db_dyn_escape_string(job->type);
    argument_esc = ja_db_dyn_escape_string(job->argument);
    script_esc = ja_db_dyn_escape_string(job->script);
    env_esc = ja_db_dyn_escape_string(job->env);
    message_esc = ja_db_dyn_escape_string(job->message);
    std_out_esc = ja_db_dyn_escape_string(job->std_out);
    std_err_esc = ja_db_dyn_escape_string(job->std_err);

    ret = ja_db_execute
        ("replace into jobs values ("
         ZBX_FS_UI64
         ", %d, '%s' , %d, '%s', '%s', '%s', '%s', %d, %d, %d, "
         ZBX_FS_UI64 ", " ZBX_FS_UI64 ", '%s', '%s', '%s', %d, %d)",
         job->jobid, job->version, serverid_esc, job->method, type_esc,
         argument_esc, script_esc, env_esc, job->result, job->status,
         job->pid, job->start_time, job->end_time, message_esc,
         std_out_esc, std_err_esc, job->return_code, job->signal);

    zbx_free(serverid_esc);
    zbx_free(type_esc);
    zbx_free(argument_esc);
    zbx_free(script_esc);
    zbx_free(env_esc);
    zbx_free(message_esc);
    zbx_free(std_out_esc);
    zbx_free(std_err_esc);

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
int ja_jobdb_insert(ja_job_object * job)
{
    int ret, status;
    const char *__function_name = "ja_jobdb_insert";

    if (job == NULL)
        return FAIL;
    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64,
               __function_name, job->jobid);

    ret = FAIL;
    if (ja_db_begin() != SUCCEED)
        goto close;

    if (job->jobid <= 0) {
        zbx_snprintf(job->message, sizeof(job->message),
                     "Invalid jobid: " ZBX_FS_UI64, job->jobid);
        goto close;
    }

    status = ja_jobdb_get_status(job->jobid);

    // delete the jobid to restart the job
    if (status == JA_AGENT_STATUS_CLOSE) {
        zabbix_log(LOG_LEVEL_WARNING,
                   "In %s() jobid: " ZBX_FS_UI64
                   " is restarted. status: CLOSE", __function_name,
                   job->jobid);
        if (FAIL ==
            ja_db_execute("delete from jobs where jobid = " ZBX_FS_UI64,
                          job->jobid))
            goto close;
        status = JA_AGENT_STATUS_INIT;
    }
    // create new job
    if (status == JA_AGENT_STATUS_INIT) {
        if (FAIL == ja_jobdb_replace(job))
            goto close;
    } else {
        job->result = JA_RESPONSE_ALREADY_RUN;
        zbx_snprintf(job->message, sizeof(job->message),
                     "jobid: " ZBX_FS_UI64
                     " is alreay running. status: %d", job->jobid, status);
        goto close;
    }
    ret = SUCCEED;

  close:
    if (ret == SUCCEED) {
        zabbix_log(LOG_LEVEL_INFORMATION,
                   "In %s() jobid: " ZBX_FS_UI64 " is created",
                   __function_name, job->jobid);
        ja_db_commit();
    } else {
        if (strlen(job->message) == 0) {
            zbx_snprintf(job->message, sizeof(job->message),
                         "Can not insert jobid: ", ZBX_FS_UI64,
                         job->jobid);
        }
        zabbix_log(LOG_LEVEL_ERR, "In %s() %s", __function_name,
                   job->message);
        ja_db_rollback();
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
int ja_jobdb_update(ja_job_object * job)
{
    int ret;
    const char *__function_name = "ja_jobdb_update";

    if (job == NULL)
        return FAIL;
    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64,
               __function_name, job->jobid);

    ret = FAIL;
    if (ja_db_begin() != SUCCEED) {
        zbx_snprintf(job->message, sizeof(job->message),
                     "can not update jobid:", ZBX_FS_UI64, job->jobid);
        goto close;
    }

    if (FAIL == ja_jobdb_replace(job)) {
        zbx_snprintf(job->message, sizeof(job->message),
                     "can not update jobid:", ZBX_FS_UI64, job->jobid);
        goto close;
    }
    ret = SUCCEED;

  close:
    if (ret == SUCCEED) {
        ja_db_commit();
    } else {
        zabbix_log(LOG_LEVEL_ERR, "In %s() %s", __function_name,
                   job->message);
        ja_db_rollback();
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
int ja_jobdb_get_jobs()
{
    int num;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_jobdb_get_jobs";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    num = -1;
    result =
        ja_db_select
        ("select count(*) from jobs where status <> %d and type != '%s'",
         JA_AGENT_STATUS_CLOSE, JA_PROTO_VALUE_REBOOT);
    if (NULL != (row = ja_db_fetch(result))) {
        num = atoi(row[0]);
    }
    ja_db_free_result(result);
    return num;
}
