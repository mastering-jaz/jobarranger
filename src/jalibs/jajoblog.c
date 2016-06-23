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
** $Date:: 2013-06-10 14:12:42 +0900 #$
** $Revision: 4882 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"
#include "dbcache.h"

#include "jacommon.h"

#define AP_MESSAGE_BUF_SIZE	1024

static char	msgwork[2048];

/******************************************************************************
 *                                                                            *
 * Function: ja_joblog                                                        *
 *                                                                            *
 * Purpose: add data to a job run log table                                   *
 *                                                                            *
 * Parameters: message_id (in) - message id                                   *
 *             inner_jobnet_id (in) - inner jobnet id                         *
 *             inner_job_id (in) - inner job id                               *
 *             ... - additional information (variable parameters)             *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - Processing error occurs                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int	ja_joblog(char *message_id, zbx_uint64_t inner_jobnet_id, zbx_uint64_t inner_job_id, ...)
{
	FILE		*fp;
	struct tm	*tm;
	time_t		now;
	DB_RESULT	result;
	DB_ROW		row;
	int		job_type, log_type, sql_flag, rc, hit, m, n, cnt;
	zbx_uint64_t	i_inner_jobnet_id;
	char		*now_date, *message = NULL, *name = NULL, *type = NULL, *msg = NULL;
	char		line[AP_MESSAGE_BUF_SIZE+1];
	va_list		ap;


	/* parameters check */
	if (inner_jobnet_id == 0 && inner_job_id == 0)
	{
		ja_log("JAJOBLOG200006", 0, NULL, 0);
		return FAIL;
	}

	i_inner_jobnet_id = inner_jobnet_id;

	/* inner jobnet id get */
	if (inner_jobnet_id == 0)
	{
		result = DBselect("select inner_jobnet_id from ja_run_job_table"
				" where inner_job_id = " ZBX_FS_UI64,
				inner_job_id);

		if (NULL == (row = DBfetch(result)))
		{
			zbx_snprintf(msgwork, sizeof(msgwork), ZBX_FS_UI64, inner_job_id);
			ja_log("JAJOBLOG200007", inner_jobnet_id, NULL, inner_job_id, "ja_run_job_table", msgwork);
			DBfree_result(result);
			return FAIL;
		}

		ZBX_STR2UINT64(i_inner_jobnet_id, row[0]);
		DBfree_result(result);
	}

	/* message file open */
	fp = fopen(CONFIG_JA_MESSAGE_FILE, "r");
	if (fp == NULL)
	{
		ja_log("JAJOBLOG200001", i_inner_jobnet_id, NULL, inner_job_id, CONFIG_JA_MESSAGE_FILE);
		return FAIL;
	}

	/* message get */
	cnt = 0;
	hit = 0;
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		cnt = cnt + 1;

		if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' )
		{
			continue;
		}

		if (strlen(line) > 0)
		{
			if (line[strlen(line)-1] == '\n')
			{
				line[strlen(line)-1] = '\0';
			}
		}

		if (strlen(line) > 0)
		{
			if (line[strlen(line)-1] == '\r')
			{
				line[strlen(line)-1] = '\0';
			}
		}

		n = 0;
		m = 0;
		name = line;
		type = NULL;
		msg  = NULL;
		while (line[++n])
		{
			if (line[n] == ',')
			{
				if (m == 0)
				{
					line[n] = '\0';
					type = &line[n + 1];
					m++;
				}
				else
				{
					line[n] = '\0';
					msg = &line[n + 1];
					break;
				}
			}
		}

		lrtrim_spaces(name);

		if (strcmp(name, message_id) == 0)
		{
			hit = 1;
			break;
		}
	}
	fclose(fp);

	/* message hit check */
	if (hit == 0)
	{
		ja_log("JAJOBLOG200002", i_inner_jobnet_id, NULL, inner_job_id, message_id);
		return FAIL;
	}

	/* message get check */
	if (name == NULL || type == NULL || msg == NULL)
	{
		ja_log("JAJOBLOG200003", i_inner_jobnet_id, NULL, inner_job_id, cnt, message_id, CONFIG_JA_MESSAGE_FILE);
		return FAIL;
	}

	lrtrim_spaces(type);

	/* check the empty data */
	if (strlen(type) <= 0 || strlen(msg) <= 0)
	{
		ja_log("JAJOBLOG200003", i_inner_jobnet_id, NULL, inner_job_id, cnt, message_id, CONFIG_JA_MESSAGE_FILE);
		return FAIL;
	}

	/* log type check */
	log_type = atoi(type);
	if (log_type == LOG_TYPE_INFO || log_type == LOG_TYPE_WARN || log_type == LOG_TYPE_ERR)
	{
		/* OK */
	}
	else
	{
		ja_log("JAJOBLOG200004", i_inner_jobnet_id, NULL, inner_job_id, cnt, type, message_id, CONFIG_JA_MESSAGE_FILE);
		return FAIL;
	}

	/* message body edit */
	va_start(ap, inner_job_id);
	message = zbx_dvsprintf(message, msg, ap);
	va_end(ap);

	if (strlen(message) > AP_MESSAGE_BUF_SIZE)
	{
		*(message + AP_MESSAGE_BUF_SIZE) = '\0';
	}

	time(&now);
	tm = localtime(&now);
	now_date = zbx_dsprintf(NULL, "%04d%02d%02d%02d%02d%02d",
				(tm->tm_year + 1900),
				(tm->tm_mon  + 1),
				 tm->tm_mday,
				 tm->tm_hour,
				 tm->tm_min,
				 tm->tm_sec);

	/* determine the type of sql */
	sql_flag = 0;
	if (inner_job_id != 0)
	{
		sql_flag = 1;
		if (strcmp(message_id, JC_JOB_END) == 0)
		{
			result = DBselect("select job_type from ja_run_job_table"
					" where inner_job_id = " ZBX_FS_UI64,
					inner_job_id);

			if (NULL == (row = DBfetch(result)))
			{
				zbx_snprintf(msgwork, sizeof(msgwork), ZBX_FS_UI64, inner_job_id);
				ja_log("JAJOBLOG200005", i_inner_jobnet_id, NULL, inner_job_id, msgwork);
				zbx_free(message);
				zbx_free(now_date);
				DBfree_result(result);
				return FAIL;
			}

			job_type = atoi(row[0]);
			if (job_type == JA_JOB_TYPE_JOB)
			{
				sql_flag = 2;
			}

			DBfree_result(result);
		}
	}

	/* job run log write */
	switch (sql_flag)
	{
		case 1:
			/* job message */
			rc = DBexecute("insert into ja_run_log_table ("
					" log_date, inner_jobnet_id, inner_jobnet_main_id, inner_job_id, update_date, log_type,"
					" method_flag, jobnet_status, job_status, run_type, public_flag, jobnet_id, jobnet_name,"
					" job_id, job_name, user_name, message_id, message)"
					" select %s, a.inner_jobnet_id, a.inner_jobnet_main_id, b.inner_job_id, a.update_date, %d,"
					" b.method_flag, a.status, b.status, a.run_type, a.public_flag, a.jobnet_id, a.jobnet_name,"
					" b.job_id, b.job_name, a.user_name, '%s', '%s'"
					" from ja_run_jobnet_table a, ja_run_job_table b"
					" where a.inner_jobnet_id = b.inner_jobnet_id and b.inner_job_id = " ZBX_FS_UI64 " and b.inner_jobnet_id = " ZBX_FS_UI64,
					now_date, log_type, message_id, message,
					inner_job_id, i_inner_jobnet_id);
			break;

		case 2:
			/* end of Job icon */
			rc = DBexecute("insert into ja_run_log_table ("
					" log_date, inner_jobnet_id, inner_jobnet_main_id, inner_job_id, update_date, log_type,"
					" method_flag, jobnet_status, job_status, run_type, public_flag, jobnet_id, jobnet_name,"
					" job_id, job_name, user_name, return_code, std_out, std_err, message_id, message)"
					" select %s, a.inner_jobnet_id, a.inner_jobnet_main_id, b.inner_job_id, a.update_date, %d,"
					" b.method_flag, a.status, b.status, a.run_type, a.public_flag, a.jobnet_id, a.jobnet_name,"
					" b.job_id, b.job_name, a.user_name, c.after_value, d.after_value, e.after_value, '%s', '%s'"
					" from ja_run_jobnet_table a, ja_run_job_table b,"
					" (select after_value from ja_run_value_after_table where inner_job_id = " ZBX_FS_UI64 " and inner_jobnet_id = " ZBX_FS_UI64 " and value_name = 'JOB_EXIT_CD') c,"
					" (select after_value from ja_run_value_after_table where inner_job_id = " ZBX_FS_UI64 " and inner_jobnet_id = " ZBX_FS_UI64 " and value_name = 'STD_OUT') d,"
					" (select after_value from ja_run_value_after_table where inner_job_id = " ZBX_FS_UI64 " and inner_jobnet_id = " ZBX_FS_UI64 " and value_name = 'STD_ERR') e"
					" where a.inner_jobnet_id = b.inner_jobnet_id and b.inner_job_id = " ZBX_FS_UI64 " and b.inner_jobnet_id = " ZBX_FS_UI64,
					now_date, log_type, message_id, message,
					inner_job_id, i_inner_jobnet_id, inner_job_id, i_inner_jobnet_id,
					inner_job_id, i_inner_jobnet_id, inner_job_id, i_inner_jobnet_id);
			break;

		default:
			/* jobnet message */
			rc = DBexecute("insert into ja_run_log_table ("
					" log_date, inner_jobnet_id, inner_jobnet_main_id, update_date, log_type,"
					" method_flag, jobnet_status, run_type, public_flag, jobnet_id, jobnet_name,"
					" user_name, message_id, message)"
					" select %s, inner_jobnet_id, inner_jobnet_main_id, update_date, %d,"
					" 0, status, run_type, public_flag, jobnet_id, jobnet_name,"
					" user_name, '%s', '%s'"
					" from ja_run_jobnet_table"
					" where inner_jobnet_id = " ZBX_FS_UI64,
					now_date, log_type, message_id, message, i_inner_jobnet_id);
			break;
	}

	if (rc <= ZBX_DB_OK)
	{
		zbx_snprintf(msgwork, sizeof(msgwork),
			" message id[%s] inner jobnet id[" ZBX_FS_UI64 "] inner job id[" ZBX_FS_UI64 "]",
			message_id, i_inner_jobnet_id, inner_job_id);
		ja_log("JAJOBLOG200008", i_inner_jobnet_id, NULL, inner_job_id, "ja_run_log_table", msgwork);
		zbx_free(message);
		zbx_free(now_date);
		return FAIL;
	}

	zbx_free(message);
	zbx_free(now_date);

	return SUCCEED;
}
