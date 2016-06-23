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
#include "jalog.h"
#include "jastatus.h"
#include "jaflow.h"

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
int jarun_icon_task(const zbx_uint64_t inner_job_id, const int test_flag)
{
    int db_ret, run_type;
    DB_RESULT result;
    DB_ROW row;
    zbx_uint64_t submit_inner_jobnet_id;
    char submit_jobnet_id[40];
    const char *__function_name = "jarun_icon_task";

    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() inner_job_id: " ZBX_FS_UI64, __function_name,
               inner_job_id);

    if (test_flag == JA_JOB_TEST_FLAG_OFF)
        run_type = JA_JOBNET_RUN_TYPE_IMMEDIATE;
    else
        run_type = JA_JOBNET_RUN_TYPE_TEST;

    // get submit_inner_jobnet_id, submit_jobnet_id
    result =
        DBselect
        ("select submit_inner_jobnet_id, submit_jobnet_id from ja_run_icon_task_table where inner_job_id = "
         ZBX_FS_UI64, inner_job_id);

    if (NULL != (row = DBfetch(result))) {
        ZBX_STR2UINT64(submit_inner_jobnet_id, row[0]);
        zbx_snprintf(submit_jobnet_id, sizeof(submit_jobnet_id), "%s",
                     row[1]);
    } else {
        ja_log("JARUNICONTASK200001", 0, NULL, inner_job_id,
               __function_name, inner_job_id);
        DBfree_result(result);
        return ja_set_runerr(inner_job_id);
    }
    DBfree_result(result);

    // check submit_inner_jobnet_id
    if (ja_get_status_jobnet(submit_inner_jobnet_id) != -1) {
        ja_log("JARUNICONTASK200002", 0, NULL, inner_job_id,
               __function_name, submit_inner_jobnet_id);
        return ja_set_runerr(inner_job_id);
    }
    // insert the submit_inner_jobnet_id
    db_ret = DBexecute
        ("insert into ja_run_jobnet_table "
         "(inner_jobnet_id, inner_jobnet_main_id, inner_job_id,"
         " update_date, run_type, main_flag, status,"
         " public_flag, jobnet_id, user_name, jobnet_name, memo)"
         " (select " ZBX_FS_UI64 ", " ZBX_FS_UI64
         ", 0, update_date, %d, 0, %d, public_flag, jobnet_id, user_name, jobnet_name, memo"
         " from ja_jobnet_control_table where valid_flag = 1 and jobnet_id = '%s')",
         submit_inner_jobnet_id, submit_inner_jobnet_id, run_type, 0,
         submit_jobnet_id);

    if (db_ret < ZBX_DB_OK)
        return FAIL;

    if (db_ret == ZBX_DB_OK) {
        ja_log("JARUNICONTASK200004", 0, NULL, inner_job_id,
               __function_name, submit_jobnet_id);
        return ja_set_runerr(inner_job_id);
    }

    return ja_flow(inner_job_id, JA_FLOW_TYPE_NORMAL);
}
