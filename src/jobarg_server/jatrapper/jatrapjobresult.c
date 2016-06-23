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
** $Date:: 2013-06-13 10:17:48 +0900 #$
** $Revision: 4907 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include <json/json.h>

#include "common.h"
#include "comms.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jaflow.h"
#include "jalog.h"
#include "jastr.h"
#include "jahost.h"
#include "jastatus.h"
#include "javalue.h"
#include "jatelegram.h"
#include "jatrapjobresult.h"

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
int jatrap_jobresult_load(ja_jobresult_object * job,
                          ja_telegram_object * obj)
{
    int ret;
    json_object *jp_data, *jp;
    char *request, *err;
    const char *__function_name = "jatrap_jobresult_load";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    if (ja_telegram_check(obj) == FAIL)
        return FAIL;
    if (job == NULL)
        return FAIL;

    ret = FAIL;
    job->jobid = 0;
    job->hostname = NULL;
    job->message = NULL;
    job->std_out = NULL;
    job->std_err = NULL;
    err = NULL;
    request = (char *) json_object_to_json_string(obj->request);
    jp_data = json_object_object_get(obj->request, JA_PROTO_TAG_DATA);

    // jobid
    jp = json_object_object_get(jp_data, JA_PROTO_TAG_JOBID);
    if (jp == NULL) {
        err =
            zbx_dsprintf(NULL,
                         "can not find the tag '%s' from the request telegram: %s",
                         JA_PROTO_TAG_JOBID, request);
        goto error;
    }
    ZBX_STR2UINT64(job->jobid, (char *) json_object_get_string(jp));

    // hostname
    jp = json_object_object_get(jp_data, JA_PROTO_TAG_HOSTNAME);
    if (jp == NULL) {
        err =
            zbx_dsprintf(NULL,
                         "can not find the tag '%s' from the request telegram: %s",
                         JA_PROTO_TAG_HOSTNAME, request);
        goto error;
    }
    job->hostname = zbx_strdup(NULL, (char *) json_object_get_string(jp));

    jp = json_object_object_get(jp_data, JA_PROTO_TAG_RESULT);
    if (jp == NULL) {
        err =
            zbx_dsprintf(NULL,
                         "can not find the tag '%s' from the request telegram: %s",
                         JA_PROTO_TAG_RESULT, request);
        goto error;
    }
    job->result = json_object_get_int(jp);

    if (job->result == JA_JOBRESULT_FAIL) {
        // message
        jp = json_object_object_get(jp_data, JA_PROTO_TAG_MESSAGE);
        if (jp == NULL) {
            err =
                zbx_dsprintf(NULL,
                             "can not find the tag '%s' from the request telegram: %s",
                             JA_PROTO_TAG_MESSAGE, request);
            goto error;
        }
        job->message =
            zbx_strdup(NULL, (char *) json_object_get_string(jp));
    } else {
        // std_out
        jp = json_object_object_get(jp_data, JA_PROTO_TAG_JOBSTDOUT);
        if (jp == NULL) {
            err =
                zbx_dsprintf(NULL,
                             "can not find the tag '%s' from the request telegram: %s",
                             JA_PROTO_TAG_JOBSTDOUT, request);
            goto error;
        }
        job->std_out =
            zbx_strdup(NULL, (char *) json_object_get_string(jp));

        // std_err
        jp = json_object_object_get(jp_data, JA_PROTO_TAG_JOBSTDERR);
        if (jp == NULL) {
            err =
                zbx_dsprintf(NULL,
                             "can not find the tag '%s' from the request telegram: %s",
                             JA_PROTO_TAG_JOBSTDERR, request);
            goto error;
        }
        job->std_err =
            zbx_strdup(NULL, (char *) json_object_get_string(jp));

        // return_code
        jp = json_object_object_get(jp_data, JA_PROTO_TAG_RET);
        if (jp == NULL) {
            err =
                zbx_dsprintf(NULL,
                             "can not find the tag '%s' from the request telegram: %s",
                             JA_PROTO_TAG_RET, request);
            goto error;
        }
        job->return_code = json_object_get_int(jp);

        // signal
        jp = json_object_object_get(jp_data, JA_PROTO_TAG_SIGNAL);
        if (jp == NULL) {
            err =
                zbx_dsprintf(NULL,
                             "can not find the tag '%s' from the request telegram: %s",
                             JA_PROTO_TAG_SIGNAL, request);
            goto error;
        }
        job->signal = json_object_get_int(jp);
    }

    ret = SUCCEED;
  error:
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() error: %s", __function_name,
                   err);
        ja_telegram_seterr(obj, err);
        jatrap_jobresult_free(job);
    }
    if (err != NULL)
        zbx_free(err);

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
void jatrap_jobresult_free(ja_jobresult_object * job)
{
    const char *__function_name = "jatrap_jobresult_free";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    if (job == NULL)
        return;
    if (job->hostname != NULL)
        zbx_free(job->hostname);
    if (job->message != NULL)
        zbx_free(job->message);
    if (job->std_out != NULL)
        zbx_free(job->std_out);
    if (job->std_err != NULL)
        zbx_free(job->std_err);
    job->jobid = 0;
    job->hostname = NULL;
    job->message = NULL;
    job->std_out = NULL;
    job->std_err = NULL;
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
int jatrap_jobresult_set_value(ja_jobresult_object * job,
                               zbx_uint64_t inner_jobnet_id)
{
    char value[24];
    const char *__function_name = "jatrap_jobresult_set_value";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() ", __function_name);

    if (job->message != NULL) {
        if (ja_set_value_after
            (job->jobid, inner_jobnet_id, "JOBARG_MESSAGE",
             job->message) == FAIL)
            return FAIL;
    }

    if (job->std_out != NULL) {
        if (ja_set_value_after
            (job->jobid, inner_jobnet_id, "STD_OUT", job->std_out) == FAIL)
            return FAIL;
    }

    if (job->std_err != NULL) {
        if (ja_set_value_after
            (job->jobid, inner_jobnet_id, "STD_ERR", job->std_err) == FAIL)
            return FAIL;
    }

    zbx_snprintf(value, sizeof(value), "%d", job->return_code);
    if (ja_set_value_after
        (job->jobid, inner_jobnet_id, "JOB_EXIT_CD", value) == FAIL)
        return FAIL;

    zbx_snprintf(value, sizeof(value), "%d", job->signal);
    if (ja_set_value_after(job->jobid, inner_jobnet_id, "SIGNAL", value) ==
        FAIL)
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
int jatrap_jobresult(ja_telegram_object * obj)
{
    int ret;
    DB_RESULT result;
    DB_ROW row;
    ja_jobresult_object job;
    json_object *jp_data;
    zbx_uint64_t inner_jobnet_id;
    int job_type, status;
    char *err;
    int run_err, cmp;
    char *msg, value[24];
    const char *__function_name = "jatrap_jobresult";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    ret = FAIL;
    inner_jobnet_id = 0;
    err = NULL;
    msg = NULL;


    if (ja_telegram_check(obj) == FAIL)
        return FAIL;
    if (jatrap_jobresult_load(&job, obj) == FAIL)
        return FAIL;

    DBbegin();
    result =
        DBselect
        ("select inner_jobnet_id, job_type, status from ja_run_job_table"
         " where inner_job_id = " ZBX_FS_UI64 " for update", job.jobid);
    if ((row = DBfetch(result)) == NULL) {
        err =
            zbx_dsprintf(NULL,
                         "can not match the inner_jobnet_id " ZBX_FS_UI64,
                         job.jobid);
        goto error;
    }
    ZBX_STR2UINT64(inner_jobnet_id, row[0]);
    job_type = atoi(row[1]);
    status = atoi(row[2]);
    DBfree_result(result);

    if (status != JA_JOB_STATUS_RUN && status != JA_JOB_STATUS_ABORT) {
        err =
            zbx_dsprintf(NULL,
                         "the job is not running. inner_job_id: "
                         ZBX_FS_UI64, job.jobid);
        goto error;
    }

    if (jatrap_jobresult_set_value(&job, inner_jobnet_id) == FAIL) {
        err =
            zbx_dsprintf(NULL,
                         "can not set the values. inner_job_id: "
                         ZBX_FS_UI64, job.jobid);
        goto error;
    }

    if (job_type == JA_JOB_TYPE_REBOOT) {
        ja_host_unlock(job.hostname);
    }

    run_err = 1;
    if (job.result != JA_JOBRESULT_SUCCEED) {
        msg = zbx_dsprintf(NULL, "%s", job.message);
    } else if (status == JA_JOB_STATUS_ABORT) {
        msg = zbx_dsprintf(NULL, "Aborted the job");
    } else if (job.signal == 1) {
        msg = zbx_dsprintf(NULL, "The job catch a signal");
    } else {
        switch (job_type) {
        case JA_JOB_TYPE_JOB:
            cmp = 0;
            result = DBselect("select stop_code from ja_run_icon_job_table"
                              " where inner_job_id = " ZBX_FS_UI64,
                              job.jobid);
            row = DBfetch(result);
            if (row != NULL) {
                zbx_snprintf(value, sizeof(value), "%d", job.return_code);
                cmp = ja_number_match(value, row[0]);
            }
            if (cmp == 0)
                run_err = 0;
            else
                msg =
                    zbx_dsprintf(NULL,
                                 "The job return code '%d' is range of the stop code '%s'",
                                 job.return_code, row[0]);
            DBfree_result(result);
            break;
        case JA_JOB_TYPE_FWAIT:
            if (job.return_code == 0 || job.return_code == 1)
                run_err = 0;
            else
                msg = zbx_dsprintf(NULL, "The job failed");
            break;
        case JA_JOB_TYPE_REBOOT:
            if (job.return_code == 0)
                run_err = 0;
            else
                msg = zbx_dsprintf(NULL, "The job failed");
            break;
        default:
            err = zbx_dsprintf(NULL, "Unknow the job type '%d'", job_type);
            goto error;
        }
    }
    if (run_err == 0) {
        ja_flow(job.jobid, JA_FLOW_TYPE_NORMAL);
    } else {
        if (msg != NULL) {
            if (ja_set_value_after
                (job.jobid, inner_jobnet_id, "JOBARG_MESSAGE",
                 msg) == FAIL) {
                err =
                    zbx_dsprintf(NULL,
                                 "can not set the values. inner_job_id: "
                                 ZBX_FS_UI64, job.jobid);

                goto error;
            }
        }
        ja_set_runerr(job.jobid);
    }

    ret = SUCCEED;
  error:
    jatrap_jobresult_free(&job);
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() error: %s", __function_name,
                   err);
        ja_telegram_seterr(obj, err);
        DBrollback();
    } else {
        jp_data = json_object_object_get(obj->response, JA_PROTO_TAG_DATA);
        json_object_object_add(jp_data, JA_PROTO_TAG_RESULT,
                               json_object_new_int(SUCCEED));
        DBcommit();
    }

    if (err != NULL)
        zbx_free(err);
    if (msg != NULL)
        zbx_free(msg);

    return ret;
}
