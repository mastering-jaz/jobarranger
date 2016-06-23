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
** $Date:: 2013-04-12 10:28:21 +0900 #$
** $Revision: 4390 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jastr.h"
#include "jakill.h"
#include "jajobfile.h"
#include "jajobobject.h"
#include "javalue.h"
#include "jastatus.h"
#include "jaflow.h"
#include "jajobiconextjob.h"

extern char *CONFIG_TMPDIR;
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
int jajob_icon_extjob(const zbx_uint64_t inner_job_id,
                      const zbx_uint64_t inner_jobnet_id,
                      const int kill_flag)
{
    int chk;
    char str_return_code[20], filepath[JA_MAX_STRING_LEN];
    ja_job_object job;
    const char *__function_name = "jajob_icon_extjob";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);

    zbx_snprintf(filepath, MAX_STRING_LEN,
                 "%s%c%s-" ZBX_FS_UI64, CONFIG_TMPDIR, JA_DLM, progname,
                 inner_job_id);

    chk = ja_jobfile_chkend(filepath, 0);
    if (chk == 0)
        return SUCCEED;
    if (chk == -1) {
        return ja_set_runerr(inner_job_id);
    }

    ja_job_object_init(&job);
    if (ja_jobfile_load(filepath, &job) == FAIL) {
        return ja_set_runerr(inner_job_id);
    }
    ja_jobfile_remove(filepath, jobext);
    zbx_snprintf(str_return_code, sizeof(str_return_code), "%d",
                 job.return_code);

    ja_set_value_after(inner_job_id, inner_jobnet_id, "JOB_EXIT_CD",
                       str_return_code);
    ja_set_value_after(inner_job_id, inner_jobnet_id, "STD_OUT",
                       job.std_out);
    ja_set_value_after(inner_job_id, inner_jobnet_id, "STD_ERR",
                       job.std_err);

    if (job.signal == 0 && kill_flag == 0) {
        return ja_flow(inner_job_id, JA_FLOW_TYPE_NORMAL);
    } else {
        return ja_set_runerr(inner_job_id);
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
int jajob_icon_extjob_kill(const zbx_uint64_t inner_job_id)
{
    pid_t pid;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "jajob_icon_extjob_kill";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);

    pid = 0;
    result =
        DBselect
        ("select pid from ja_run_icon_extjob_table"
         " where inner_job_id = " ZBX_FS_UI64, inner_job_id);
    if (NULL != (row = DBfetch(result))) {
        pid = atoi(row[0]);
    }
    DBfree_result(result);

    if (pid <= 0)
        return SUCCEED;

    if (ja_kill(pid) == SUCCEED)
        return ja_set_runerr(inner_job_id);

    return SUCCEED;
}
