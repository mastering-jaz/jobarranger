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
** $Date:: 2013-05-27 13:50:15 +0900 #$
** $Revision: 4664 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include <json/json.h>
#include "common.h"
#include "log.h"

#include "jacommon.h"
#include "jajobobject.h"
#include "jaextjob.h"

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
int ja_extjob_script(ja_job_object * job)
{
    int ret;
    json_object *jp_arg, *jp;
    int i;
    char *cmd;
    const char *__function_name = "ja_extjob_script";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    ret = FAIL;
    jp_arg = json_tokener_parse(job->argument);
    if (is_error(jp_arg)) {
        jp_arg = NULL;
        zbx_snprintf(job->message, sizeof(job->message),
                     "Can not parse job argument [%s]", job->argument);
        goto error;
    }

    if (json_object_get_type(jp_arg) != json_type_array) {
        zbx_snprintf(job->message, sizeof(job->message),
                     "[%s] is not an array", job->argument);
        goto error;
    }

    cmd =
        zbx_dsprintf(NULL, "\"%s%c%s.%s\"", CONFIG_EXTJOB_PATH, JA_DLM,
                     job->script, JA_EXE);
    zbx_snprintf(job->script, sizeof(job->script), "%s", cmd);
    zbx_free(cmd);
    for (i = 0; i < json_object_array_length(jp_arg); i++) {
        jp = json_object_array_get_idx(jp_arg, i);
        cmd =
            zbx_dsprintf(NULL, "%s \"%s\"", job->script,
                         json_object_get_string(jp));
        zbx_snprintf(job->script, sizeof(job->script), "%s", cmd);
        zbx_free(cmd);
    }

    ret = SUCCEED;

  error:
    json_object_put(jp_arg);
    return ret;


}
