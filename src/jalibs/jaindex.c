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
** $Date:: 2013-04-04 11:51:35 +0900 #$
** $Revision: 4265 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"

#include "log.h"
#include "db.h"

#include "jaindex.h"
#include "jalog.h"

static char	msgwork[2048];

/******************************************************************************
 *                                                                            *
 * Function: get_next_id                                                      *
 *                                                                            *
 * Purpose: gets the sequence number of the ID for the execution              *
 *                                                                            *
 * Parameters: count_id (in) - ID serial number identification                *
 *             inner_jobnet_id (in) - inner jobnet id                         *
 *             jobnet_id (in) - jobnet id                                     *
 *                                                                            *
 * Return value: next id value                                                *
 *               0 - an error occurred                                        *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
zbx_uint64_t	get_next_id(int count_id, zbx_uint64_t inner_jobnet_id, char *jobnet_id)
{
	DB_RESULT	result;
	DB_ROW		row;
	int		rc, cycle_flag;
	zbx_uint64_t	nextid;
	const char	*__function_name = "get_next_id";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%d)", __function_name, count_id);

	result = DBselect("select nextid from ja_index_table where count_id = %d for update", count_id);

	if (NULL == (row = DBfetch(result)))
	{
		zbx_snprintf(msgwork, sizeof(msgwork), "%d", count_id);
		ja_log("JAINDEX200001", inner_jobnet_id, jobnet_id, 0, "ja_index_table", msgwork);
		DBfree_result(result);
		return 0;
	}

	cycle_flag = 0;

	ZBX_STR2UINT64(nextid, row[0]);
	DBfree_result(result);

	switch (count_id)
	{
		case JA_RUN_ID_JOBNET_LD:
			if (nextid > (zbx_uint64_t)__UINT64_C(1499999999999999999))
			{
				nextid = 1;
				cycle_flag = 1;
			}
			break;

		case JA_RUN_ID_JOBNET_EX:
			if (nextid > (zbx_uint64_t)__UINT64_C(1699999999999999999))
			{
				nextid = (zbx_uint64_t)__UINT64_C(1600000000000000000);
				cycle_flag = 1;
			}
			break;

		default:
			if (nextid > (zbx_uint64_t)__UINT64_C(9200000000000000000))
			{
				nextid = 1;
				cycle_flag = 1;
			}
			break;
	}

	if (cycle_flag == 1)
	{
		rc = DBexecute("update ja_index_table set nextid = " ZBX_FS_UI64 " where count_id = %d", nextid, count_id);
	}
	else
	{
		rc = DBexecute("update ja_index_table set nextid = nextid + 1 where count_id = %d", count_id);
	}

	if (rc <= ZBX_DB_OK)
	{
		zbx_snprintf(msgwork, sizeof(msgwork), "%d", count_id);
		ja_log("JAINDEX200002", inner_jobnet_id, jobnet_id, 0, "ja_index_table", msgwork);
		return 0;
	}

	return nextid;
}
