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

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jalog.h"
#include "jajoblog.h"
#include "jaself.h"
#include "jarunnormal.h"
#include "jarunskip.h"
#include "jarun.h"

extern unsigned char process_type;
extern int process_num;

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
static int process_jarun()
{
    int ret;
    DB_RESULT result;
    DB_RESULT result2;
    DB_ROW row;
    DB_ROW row2;
    double sec;
    zbx_uint64_t inner_job_id, inner_jobnet_id;
    int method_flag, job_type, test_flag;
    const char *__function_name = "process_jarun";

    ret = FAIL;
    sec = zbx_time();
    result =
        DBselect
        ("select inner_job_id, inner_jobnet_id, method_flag, job_type, test_flag from ja_run_job_table"
         " where status = %d ", JA_JOB_STATUS_READY);
    sec = zbx_time() - sec;
    zabbix_log(LOG_LEVEL_DEBUG,
               "In %s() ja_run_job_table(status = READY): " ZBX_FS_DBL
               " sec.", __function_name, sec);

    while (NULL != (row = DBfetch(result))) {
        ZBX_STR2UINT64(inner_job_id, row[0]);
        ZBX_STR2UINT64(inner_jobnet_id, row[1]);
        method_flag = atoi(row[2]);
        job_type = atoi(row[3]);
        test_flag = atoi(row[4]);
        zabbix_log(LOG_LEVEL_DEBUG,
                   "In %s() inner_job_id: " ZBX_FS_UI64
                   ", inner_jobnet_id: " ZBX_FS_UI64
                   ", method_flag: %d, job_type: %d, test_flag: %d",
                   __function_name, inner_job_id, inner_jobnet_id,
                   method_flag, job_type, test_flag);

        ret = SUCCEED;
        DBbegin();
        result2 = DBselect("select inner_job_id from ja_run_job_table where inner_job_id = "
                           ZBX_FS_UI64 " and status = %d for update", inner_job_id, JA_JOB_STATUS_READY);
	if (NULL == (row2 = DBfetch(result2)))
	{
            DBcommit();
            DBfree_result(result2);
            continue;
	}
        DBfree_result(result2);

        switch (method_flag) {
        case JA_JOB_METHOD_NORMAL:
            ja_joblog(JC_JOB_START, inner_jobnet_id, inner_job_id);
            ret = jarun_normal(inner_job_id, job_type, test_flag);
            break;
        case JA_JOB_METHOD_WAIT:
            ret = FAIL;
            break;
        case JA_JOB_METHOD_SKIP:
            ret = jarun_skip(inner_job_id, inner_jobnet_id, job_type);
            break;
        case JA_JOB_METHOD_ABORT:
            ja_log("JARUN300001", inner_jobnet_id, NULL, inner_job_id,
                   __function_name, inner_job_id);
            break;
        case JA_JOB_METHOD_RERUN:
            ja_joblog(JC_JOB_RERUN, inner_jobnet_id, inner_job_id);
            ret = jarun_normal(inner_job_id, job_type, test_flag);
            break;
        default:
            ja_log("JARUN200001", inner_jobnet_id, NULL, inner_job_id,
                   method_flag, inner_job_id, job_type);
            break;
        }

        if (ret == SUCCEED) {
            DBcommit();
        } else {
            DBrollback();
        }
    }
    DBfree_result(result);

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
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
void main_jarun_loop()
{
    zabbix_log(LOG_LEVEL_DEBUG,
               "In main_jarun_loop() process_type:'%s' process_num:%d",
               ja_get_process_type_string(process_type), process_num);

    zbx_setproctitle("%s [connecting to the database]",
                     ja_get_process_type_string(process_type));

    DBconnect(ZBX_DB_CONNECT_NORMAL);
    for (;;) {
        zbx_setproctitle("%s [processing data]",
                         ja_get_process_type_string(process_type));
        if (process_jarun() == SUCCEED)
            continue;
        ja_sleep_loop(CONFIG_JARUN_INTERVAL);
    }
}
