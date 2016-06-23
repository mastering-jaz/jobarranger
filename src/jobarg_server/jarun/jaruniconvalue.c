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
** $Date:: 2013-12-16 16:39:52 +0900 #$
** $Revision: 5627 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"

#include "jacommon.h"
#include "jastatus.h"
#include "jaflow.h"
#include "javalue.h"

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
int jarun_icon_value(const zbx_uint64_t inner_job_id)
{
    DB_RESULT result;
    DB_ROW row;
    int db_ret;
    char *after_value_esc;
    char after_value[4000+1];
    const char *__function_name = "jarun_icon_value";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    result = DBselect("select inner_job_id, inner_jobnet_id, value_name, value"
                      " from ja_run_icon_value_table"
                      " where inner_job_id = " ZBX_FS_UI64, inner_job_id);

    while (NULL != (row = DBfetch(result)))
    {
        if (ja_cpy_value(inner_job_id, row[3], after_value) == FAIL) {
            ja_log("JARUNICONVALUE200002", 0, NULL, inner_job_id,
                   __function_name, row[3], inner_job_id);
            DBfree_result(result);
            return ja_set_runerr(inner_job_id);
        }

        after_value_esc = DBdyn_escape_string(after_value);
        db_ret = DBexecute("insert into ja_run_value_after_table"
                           " (inner_job_id, inner_jobnet_id, value_name, after_value)"
                           " values ( %s, %s, '%s', '%s')",
                           row[0], row[1], row[2], after_value_esc);
        if (db_ret <= ZBX_DB_OK) {
            ja_log("JARUNICONVALUE200001", 0, NULL, inner_job_id,
                   __function_name, "ja_run_value_after_table", row[2], inner_job_id);
            zbx_free(after_value_esc);
            DBfree_result(result);
            return ja_set_runerr(inner_job_id);
        }
        zbx_free(after_value_esc);
    }
    DBfree_result(result);

    return ja_flow(inner_job_id, JA_FLOW_TYPE_NORMAL);
}
