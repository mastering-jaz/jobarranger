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
** $Date:: 2014-11-07 15:14:44 +0900 #$
** $Revision: 6635 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "threads.h"

#if defined(ZABBIX_SERVICE)
#include "service.h"
#elif defined(ZABBIX_DAEMON)
#include "daemon.h"
#endif

#include "jacommon.h"
#include "jastr.h"
#include "jafile.h"

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
int ja_jobfile_create(const char *filepath, char *jobext[],
                      const char *script)
{
    char filename[JA_MAX_STRING_LEN];
    char *buf;
    FILE *fp;
    int ret, err, i;
    const char *__function_name = "ja_jobfile_create";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filepath: %s", __function_name, filepath);

    buf = NULL;
    /* create script file */
    zbx_snprintf(filename, sizeof(filename), "%s.%s", filepath, JA_EXE);
    fp = fopen(filename, "w");
    if (fp == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "Can not open script file: %s (%s)", filename, strerror(errno));
        return FAIL;
    }

#ifdef _WINDOWS
    buf = (char *) ja_utf8_to_acp(script);
    err = fprintf(fp, "%s", buf);
    zbx_free(buf);
#else
    err = fprintf(fp, "%s", script);
#endif
    fclose(fp);

    if (err < 0) {
        zabbix_log(LOG_LEVEL_ERR, "Can not write to script file: %s (%s)", filename, strerror(errno));
        return FAIL;
    }

    if (chmod(filename, 0755) < 0) {
        zabbix_log(LOG_LEVEL_ERR, "Can not chmod script file: %s (%s)", filename, strerror(errno));
        return FAIL;
    }

    /* create result files */
    i = 0;
    while (jobext[i] != NULL) {
        zbx_snprintf(filename, sizeof(filename), "%s.%s", filepath, jobext[i]);
        if (strcmp(jobext[i], "stdout") == 0 || strcmp(jobext[i], "stderr") == 0) {
            ret = ja_file_create(filename, JA_STD_OUT_LEN);
        }
        else {
            ret = ja_file_create(filename, 1);
        }

        if (ret == FAIL) {
            return FAIL;
        }
        i++;
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
int ja_jobfile_remove(const char *filepath, char *jobext[])
{
    char filename[JA_MAX_STRING_LEN];
    int ret, i;
    const char *__function_name = "ja_jobfile_remove";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filepath: %s", __function_name, filepath);

    ret = SUCCEED;

    zbx_snprintf(filename, sizeof(filename), "%s.%s", filepath, JA_EXE);
    if (ja_file_remove(filename) == FAIL) {
        ret = FAIL;
    }

    i = 0;
    while (jobext[i] != NULL) {
        zbx_snprintf(filename, sizeof(filename), "%s.%s", filepath, jobext[i]);
        if (ja_file_remove(filename) == FAIL) {
            ret = FAIL;
        }
        i++;
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
int ja_jobfile_chkend(const char *filepath, JA_PID pid, const char *type)
{
    char filename[JA_MAX_STRING_LEN];
    int ret_fsize, start_fsize, end_fsize;
#ifdef _WINDOWS
    ZBX_THREAD_HANDLE hd;
    DWORD st;
#endif
    const char *__function_name = "ja_jobfile_chkend";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filepath: %s, pid: %d", __function_name, filepath, pid);

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.ret", filepath);
    ret_fsize = ja_file_getsize(filename);

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.start", filepath);
    start_fsize = ja_file_getsize(filename);

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.end", filepath);
    end_fsize = ja_file_getsize(filename);

    if (ret_fsize < 0 || start_fsize < 0 || end_fsize < 0) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() more than one of the result files are not existed. ret_fsize: %d, start_fsize: %d, end_fsize: %d ",
                   __function_name, ret_fsize, start_fsize, end_fsize);
        return -1;
    }

    if (ret_fsize > 3 && start_fsize > 3 && end_fsize > 3) {
        return 1;
    }

    if (pid == 0) {
        return 0;
    }

#ifdef _WINDOWS
    hd = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hd != NULL) {
        if (GetExitCodeProcess(hd, &st) != 0) {
            if (st == STILL_ACTIVE) {
                return 0;
            }
        }
    }
#else
    if (kill(pid, 0) == 0) {
        return 0;
    }
#endif

    zbx_sleep(3);

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.ret", filepath);
    ret_fsize = ja_file_getsize(filename);

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.start", filepath);
    start_fsize = ja_file_getsize(filename);

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.end", filepath);
    end_fsize = ja_file_getsize(filename);

    if (strcmp(type, JA_PROTO_VALUE_REBOOT) == 0) {
        if (start_fsize > 3) {
            zabbix_log(LOG_LEVEL_DEBUG, "In %s() reboot is complete (other than Linux)", __function_name);
            return 1;
        }
    } else {
        if (ret_fsize > 3 && start_fsize > 3 && end_fsize > 3) {
            zabbix_log(LOG_LEVEL_DEBUG, "In %s() job execution completion (with file write delay)", __function_name);
            return 1;
        }
    }

    zabbix_log(LOG_LEVEL_ERR, "In %s() process %d isn't existed", __function_name, pid);
    return -1;
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
int ja_jobfile_load(const char *filepath, ja_job_object * job)
{
    int res_ret;
    time_t t;
    char filename[JA_MAX_STRING_LEN];
    char *buf;
    const char *__function_name = "ja_jobfile_load";

    if (job == NULL || filepath == NULL) {
        return FAIL;
    }

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filepath: %s", __function_name, filepath);

    buf = NULL;

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.ret", filepath);
    if (ja_file_getsize(filename) == 0) {
        job->return_code = 0;
        job->signal = 0;
    } else {
        if (ja_file_load(filename, sizeof(res_ret), &res_ret) == FAIL) {
            return FAIL;
        }
#ifdef _WINDOWS
        job->return_code = res_ret;
        if (res_ret == SIGNALNO) {
            job->signal = 1;
        } else {
            job->signal = 0;
        }
#else
        if (WIFEXITED(res_ret)) {
            job->return_code = WEXITSTATUS(res_ret);
            job->signal = 0;
        } else {
            job->return_code = WTERMSIG(res_ret);
            job->signal = 1;
        }
#endif
    }

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.start", filepath);
    if (ja_file_load(filename, sizeof(t), &(t)) == FAIL) {
        return FAIL;
    }
    job->start_time = t;

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.end", filepath);
    if (ja_file_getsize(filename) == 0) {
        job->end_time = job->start_time;
    } else {
        if (ja_file_load(filename, sizeof(t), &(t)) == FAIL) {
                return FAIL;
        }
        job->end_time = t;
    }

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.stdout", filepath);
    if (ja_file_load(filename, 0, &(job->std_out)) == FAIL) {
        return FAIL;
    }

#ifdef _WINDOWS
    buf = (char *) ja_acp_to_utf8(job->std_out);
    zbx_snprintf(job->std_out, sizeof(job->std_out), "%s", buf);
    zbx_free(buf);
#endif

    zbx_snprintf(filename, JA_MAX_STRING_LEN, "%s.stderr", filepath);
    if (ja_file_load(filename, 0, &(job->std_err)) == FAIL) {
        return FAIL;
    }

#ifdef _WINDOWS
    buf = (char *) ja_acp_to_utf8(job->std_err);
    zbx_snprintf(job->std_err, sizeof(job->std_err), "%s", buf);
    zbx_free(buf);
#endif

    return SUCCEED;
}
