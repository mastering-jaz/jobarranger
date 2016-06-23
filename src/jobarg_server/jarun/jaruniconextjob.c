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
** $Date:: 2013-05-17 16:53:37 +0900 #$
** $Revision: 4641 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jathreads.h"
#include "jajobfile.h"
#include "jastr.h"
#include "jaenv.h"
#include "jalog.h"
#include "javalue.h"
#include "jastatus.h"
#include "jaruniconextjob.h"

extern char *CONFIG_TMPDIR;
extern char *CONFIG_EXTJOB_PATH;
extern char *jobext[];

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
int jarun_icon_extjob_run(const zbx_uint64_t inner_job_id,
                          const zbx_uint64_t inner_jobnet_id,
                          const char *command)
{
    int ret, db_ret;
    pid_t pid;
    char filepath[JA_MAX_STRING_LEN];
    char full_command[JA_MAX_STRING_LEN];
    char jacmd[JA_MAX_STRING_LEN];
    const char *__function_name = "jarun_icon_extjob_run";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64
               ", command: %s", __function_name, inner_job_id, command);

    ret = SUCCEED;
    zbx_snprintf(filepath, JA_MAX_STRING_LEN, "%s%c%s-" ZBX_FS_UI64,
                 CONFIG_TMPDIR, JA_DLM, progname, inner_job_id);
    zbx_snprintf(full_command, JA_MAX_STRING_LEN, "%s.%s", filepath,
                 JA_EXE);
    zbx_snprintf(jacmd, JA_MAX_STRING_LEN, "%s%c%s", CONFIG_EXTJOB_PATH,
                 JA_DLM, JA_JOBARG_COMMAND);
    if (ja_jobfile_create(filepath, jobext, command) != SUCCEED) {
        return ja_set_runerr(inner_job_id);
    }

    pid = ja_fork();
    if (pid == -1) {
        ja_log("JARUNICONEXTJOB200001", inner_jobnet_id, NULL,
               inner_job_id, __function_name);
        return ja_set_runerr(inner_job_id);
    } else if (pid != 0) {
        waitpid(pid, NULL, WNOHANG);
        db_ret =
            DBexecute
            ("update ja_run_icon_extjob_table set pid = %d where inner_job_id = "
             ZBX_FS_UI64, pid, inner_job_id);
        if (db_ret < ZBX_DB_OK) {
            ja_log("JARUNICONEXTJOB200006", inner_jobnet_id, NULL,
                   inner_job_id, __function_name, pid, inner_job_id);
            return FAIL;
        }
        zabbix_log(LOG_LEVEL_DEBUG,
                   "End of %s() inner_job_id: " ZBX_FS_UI64,
                   __function_name, inner_job_id);
        return SUCCEED;
    }

    DBconnect(ZBX_DB_CONNECT_ONCE);
    if (ja_setenv(inner_job_id) == FAIL) {
        DBclose();
        goto error;
    }
    DBclose();
    execl(jacmd, JA_JOBARG_COMMAND, filepath, full_command, NULL);

  error:
    DBconnect(ZBX_DB_CONNECT_ONCE);
    ja_log("JARUNICONEXTJOB200003", inner_jobnet_id, NULL, inner_job_id,
           __function_name, jacmd, zbx_strerror(errno));
    ja_set_runerr(inner_job_id);
    DBclose();
    exit(0);
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
int jarun_icon_extjob(const zbx_uint64_t inner_job_id, const int test_flag)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    zbx_uint64_t inner_jobnet_id;
    char command_id[JA_MAX_STRING_LEN], value[JA_MAX_STRING_LEN],
        start_time[16], command[JA_MAX_DATA_LEN];
    const char *__function_name = "jarun_icon_extjob";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    result =
        DBselect
        ("select inner_jobnet_id, command_id, value from ja_run_icon_extjob_table"
         " where inner_job_id = " ZBX_FS_UI64, inner_job_id);
    row = DBfetch(result);
    if (row == NULL) {
        ja_log("JARUNICONEXTJOB200004", 0, NULL, inner_job_id,
               __function_name, inner_job_id);
        DBfree_result(result);
        return ja_set_runerr(inner_job_id);
    }

    ZBX_STR2UINT64(inner_jobnet_id, row[0]);
    zbx_snprintf(command_id, sizeof(command_id), "%s", row[1]);
    ja_format_extjob(row[2], value);
    DBfree_result(result);

    if (test_flag == JA_JOB_TEST_FLAG_ON) {
        zbx_snprintf(command, sizeof(command), " ");
    } else {
        if (strcmp(command_id, JA_CMD_TIME) == 0) {
            if (ja_get_jobnet_summary_start(inner_jobnet_id, start_time) ==
                FAIL) {
                ja_log("JARUNICONEXTJOB200005", inner_jobnet_id, NULL,
                       inner_job_id, __function_name, inner_jobnet_id);
                return ja_set_runerr(inner_job_id);
            }
            zbx_snprintf(command, sizeof(command), "%s%c%s %s %s",
                         CONFIG_EXTJOB_PATH, JA_DLM, command_id, value,
                         start_time);
        } else {
            zbx_snprintf(command, sizeof(command), "%s%c%s %s",
                         CONFIG_EXTJOB_PATH, JA_DLM, command_id, value);
        }
    }
    ret = jarun_icon_extjob_run(inner_job_id, inner_jobnet_id, command);

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);
    return ret;
}
