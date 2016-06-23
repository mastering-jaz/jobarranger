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
#include "javalue.h"
#include "jastatus.h"
#include "jaruniconend.h"

/******************************************************************************
 *                                                                            *
 * Function: ja_stop_code_conv                                                *
 *                                                                            *
 * Purpose: convert to a number stop code                                     *
 *                                                                            *
 * Parameters: str (in) - stop code of string                                 *
 *                                                                            *
 * Return value: stop code of number (0 - 255)                                *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int ja_stop_code_conv(char *str)
{
    int        sw, cnt, num;
    char       *p_str;
    const char *__function_name = "ja_stop_code_conv";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s))", __function_name, str);

    if ('\0' == *str) {
        return 0;
    }

    sw    = 0;
    cnt   = 0;
    p_str = str;

    while ('\0' != *p_str) {
        if (0 == isdigit(*p_str)) {
            return 0;  /* not a digit */
        }
        if (0 == sw && '0' != *p_str) {
            sw = 1;
        }
        if (1 == sw) {
            cnt = cnt + 1;
        }
        if (3 < cnt) {
            return 255;  /* number of digits over */
        }
        p_str++;
    }

    num = atoi(str);

    if (num > 255) {
        return 255;
    }

    if (num < 0) {
        return 0;
    }

    return num;
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
int jarun_icon_end(const zbx_uint64_t inner_job_id)
{
    DB_RESULT    result;
    DB_ROW       row;
    zbx_uint64_t inner_jobnet_id;
    char         stop_code_value[JA_STD_OUT_LEN];
    const char   *__function_name = "jarun_icon_end";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64, __function_name, inner_job_id);

    /* get information of end icon */
    result = DBselect("select inner_jobnet_id, jobnet_stop_code from ja_run_icon_end_table"
                      " where inner_job_id = " ZBX_FS_UI64, inner_job_id);

    if (NULL == (row = DBfetch(result))) {
        ja_log("JARUNICONEND200001", 0, NULL, inner_job_id, __function_name, inner_job_id);
        DBfree_result(result);
        return ja_set_runerr(inner_job_id, 2);
    }

    ZBX_STR2UINT64(inner_jobnet_id, row[0]);

    /* job controller variable get */
    if (FAIL == ja_cpy_value(inner_job_id, row[1], stop_code_value)) {
        ja_log("JARUNICONEND200002", 0, NULL, inner_job_id, __function_name, row[1], inner_job_id);
        DBfree_result(result);
        return ja_set_runerr(inner_job_id, 2);
    }
    DBfree_result(result);

    zbx_snprintf(stop_code_value, sizeof(stop_code_value), "%d", ja_stop_code_conv(stop_code_value));

    /* job(net) exit code write */
    if (FAIL == ja_set_value_after(inner_job_id, inner_jobnet_id, "JOB_EXIT_CD", stop_code_value)) {
        return FAIL;
    }

    return ja_set_end(inner_job_id, 1);
}
