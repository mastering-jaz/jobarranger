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
** $Date:: 2013-04-11 14:52:30 +0900 #$
** $Revision: 4385 $
** $Author: ossinfra@FITECHLABS.CO.JP $
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
    int db_ret;
    const char *__function_name = "jarun_icon_value";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() inner_job_id: " ZBX_FS_UI64,
               __function_name, inner_job_id);

    db_ret =
        DBexecute("insert into ja_run_value_after_table"
                  " (inner_job_id, inner_jobnet_id, value_name, after_value)"
                  " select inner_job_id, inner_jobnet_id, value_name, value"
                  " from ja_run_icon_value_table where inner_job_id = "
                  ZBX_FS_UI64, inner_job_id);
    if (db_ret < ZBX_DB_OK)
        return FAIL;

    return ja_flow(inner_job_id, JA_FLOW_TYPE_NORMAL);
}
