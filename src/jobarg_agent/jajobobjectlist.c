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
** $Date:: 2016-04-15 14:38:43 +0900 #$
** $Revision: 7058 $
** $Author: sypark@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "threads.h"

#include "jasqlite.h"
#include "jafile.h"
#include "jajobdb.h"
#include "jajobobject.h"
#include "jaagent.h"
#include "jajobobjectlist.h"

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
ja_job_object_list *ja_job_object_list_new(void)
{
    ja_job_object_list *jobs, *pre, *cur;
    DB_RESULT result;
    DB_ROW row;
    const char *__function_name = "ja_job_object_list_new";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    jobs =
        (ja_job_object_list *) zbx_malloc(NULL,
                                          sizeof(ja_job_object_list));

    jobs->next = NULL;
    jobs->data = NULL;
    pre = jobs;
    result =
        ja_db_select("select * from jobs where status <> %d",
                     JA_AGENT_STATUS_CLOSE);
    while (NULL != (row = ja_db_fetch(result))) {
        cur =
            (ja_job_object_list *) zbx_malloc(NULL,
                                              sizeof(ja_job_object_list));
        pre->next = cur;
        cur->next = NULL;
        cur->data =
            (ja_job_object *) zbx_malloc(NULL, sizeof(ja_job_object));
        ja_jobdb_load(row, cur->data);
        pre = cur;
    }
    ja_db_free_result(result);
    return jobs;
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
void ja_job_object_list_delete(ja_job_object_list * jobs)
{
    ja_job_object_list *next;
    const char *__function_name = "ja_job_object_list_delete";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    while (jobs != NULL) {
        next = jobs->next;
        if (jobs->data != NULL) {
            free(jobs->data);
            jobs->data = NULL;
        }
        free(jobs);
        jobs = next;
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
int ja_job_object_list_add(ja_job_object_list * jobs)
{
    DB_RESULT result;
    DB_ROW row;
    ja_job_object_list *cur, *pre;
    ja_job_object job_db;
    const char *__function_name = "ja_job_object_list_add";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    if (jobs == NULL)
        return FAIL;

    result = ja_db_select("select * from jobs where status = 0");
    while (NULL != (row = ja_db_fetch(result))) {
        ja_jobdb_load(row, &job_db);
        pre = jobs;
        cur = pre->next;
        while (cur != NULL) {
            if (cur->data->jobid == job_db.jobid)
                break;
            pre = cur;
            cur = pre->next;
        }
        if (cur != NULL)
            continue;
        cur =
            (ja_job_object_list *) zbx_malloc(NULL,
                                              sizeof(ja_job_object_list));
        pre->next = cur;
        cur->next = NULL;
        cur->data =
            (ja_job_object *) zbx_malloc(NULL, sizeof(ja_job_object));
        ja_jobdb_load(row, cur->data);
        zabbix_log(LOG_LEVEL_DEBUG, "jobid " ZBX_FS_UI64 " is added",
                   cur->data->jobid);
    }

    ja_db_free_result(result);

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
int ja_job_object_list_executive(ja_job_object_list * jobs)
{
    int ret,cnt;
    ja_job_object *job;
    ja_job_object_list *pre, *cur;
    const char *__function_name = "ja_job_object_list_executive";

    ret = SUCCEED;
    if (ja_file_getsize(CONFIG_REQUEST_FLAG) >= 0) {
        if (ja_file_remove(CONFIG_REQUEST_FLAG) == FAIL) {
            ret = FAIL;
        }
        ja_job_object_list_add(jobs);
    }

    pre = jobs;
    cur = pre->next;
    cnt = 0;
    while (cur != NULL) {

    	job = cur->data;
        if (job != NULL) {

        	if(cnt > 2){
        		zbx_sleep(1);
        	    cnt=0;
        	}
        	cnt++;

            switch (job->status) {
            case JA_AGENT_STATUS_BEGIN:
                zabbix_log(LOG_LEVEL_INFORMATION,
                           "In %s () jobid: " ZBX_FS_UI64
                           ", type: %s, status: BEGIN", __function_name,
                           job->jobid, job->type);
                if (ja_agent_run(job) == SUCCEED)
                    continue;
                break;
            case JA_AGENT_STATUS_RUN:
                if (ja_agent_end(job) == SUCCEED) {
                    continue;
                }
                break;
            case JA_AGENT_STATUS_END:
                zabbix_log(LOG_LEVEL_INFORMATION,
                           "In %s() jobid: " ZBX_FS_UI64 ", status: END",
                           __function_name, job->jobid);
                if (ja_agent_close(job) == SUCCEED)
                    continue;
                break;
            case JA_AGENT_STATUS_CLOSE:
                zabbix_log(LOG_LEVEL_INFORMATION,
                           "In %s() jobid: " ZBX_FS_UI64 ", status: CLOSE",
                           __function_name, job->jobid);
                pre->next = cur->next;
                free(job);
                free(cur);
                cur = pre;
                break;
            default:
                break;
            }
        }
        pre = cur;
        cur = pre->next;
    }
    return ret;
}
