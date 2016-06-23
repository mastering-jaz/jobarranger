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
** $Date:: 2013-08-05 13:45:29 +0900 #$
** $Rev: 5232 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"

#include "log.h"
#include "db.h"
#include "dbcache.h"
#include "daemon.h"
#include "zbxserver.h"
#include "zbxself.h"
#include "../events.h"

#include "jacommon.h"
#include "jaboot.h"

#define JOBNET_KEEP_SPAN		"JOBNET_KEEP_SPAN"
#define JOBLOG_KEEP_SPAN		"JOBLOG_KEEP_SPAN"

#define DEFAULT_JOBNET_KEEP_SPAN	60
#define DEFAULT_JOBLOG_KEEP_SPAN	1440

extern unsigned char	process_type;
extern int		process_num;

static char		msgwork[2048];

/******************************************************************************
 *                                                                            *
 * Function: get_purge_date                                                   *
 *                                                                            *
 * Purpose: gets the date of the information can be deleted jobnet            *
 *                                                                            *
 * Parameters: span (in) - retention period of net job information            *
 *             purge_date (out) - purge start time (YYYYMMDDHHMM)             *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
void		get_purge_date(int span, char **purge_date)
{
	struct tm	*tm;
	time_t		now;
	const char	*__function_name = "get_purge_date";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%d)", __function_name, span);

	time(&now);
	tm = localtime(&now);

	tm->tm_min = tm->tm_min - span;
	mktime(tm);

	*purge_date = zbx_dsprintf(NULL, "%04d%02d%02d%02d%02d",
				(tm->tm_year + 1900),
				(tm->tm_mon  + 1),
				 tm->tm_mday,
				 tm->tm_hour,
				 tm->tm_min);

	return;
}

/******************************************************************************
 *                                                                            *
 * Function: get_jobnet_keep_span                                             *
 *                                                                            *
 * Purpose: gets the retention period of a jobnet                             *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value: retention period                                             *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	get_jobnet_keep_span()
{
	DB_RESULT	result;
	DB_ROW		row;
	int		span = DEFAULT_JOBNET_KEEP_SPAN;
	const char	*__function_name = "get_jobnet_keep_span";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	result = DBselect("select value from ja_parameter_table where parameter_name = '%s'", JOBNET_KEEP_SPAN);

	if (NULL != (row = DBfetch(result)))
	{
		span = atoi(row[0]);
	}
	DBfree_result(result);

	return span;
}

/******************************************************************************
 *                                                                            *
 * Function: get_joblog_keep_span                                             *
 *                                                                            *
 * Purpose: gets the retention period of a joblog                             *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value: retention period                                             *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	get_joblog_keep_span()
{
	DB_RESULT	result;
	DB_ROW		row;
	int		span = DEFAULT_JOBLOG_KEEP_SPAN;
	const char	*__function_name = "get_joblog_keep_span";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	result = DBselect("select value from ja_parameter_table where parameter_name = '%s'", JOBLOG_KEEP_SPAN);

	if (NULL != (row = DBfetch(result)))
	{
		span = atoi(row[0]);
	}
	DBfree_result(result);

	return span;
}

/******************************************************************************
 *                                                                            *
 * Function: error_state_jobnet                                               *
 *                                                                            *
 * Purpose: change in the status of the jobnet error stop state               *
 *                                                                            *
 * Parameters: inner_jobnet_id (in) - inner jobnet id                         *
 *             jobnet_id (in) - jobnet id                                     *
 *             update_date (in) - update date                                 *
 *             scheduled_time (in) - scheduled time  (YYYYMMDDHHMM)           *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - an error occurred                                    *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	error_state_jobnet(char *inner_jobnet_id, char *jobnet_id, char *update_date,
				char *scheduled_time)
{
	DB_RESULT	result;
	DB_ROW		row;
	int		rc;
	zbx_uint64_t	i_inner_jobnet_id;
	const char	*__function_name = "error_state_jobnet";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s %s %s %s)",
		__function_name, inner_jobnet_id, jobnet_id, update_date, scheduled_time);

	ZBX_STR2UINT64(i_inner_jobnet_id, inner_jobnet_id);

	result = DBselect("select inner_job_id"
			" from ja_run_job_table"
			" where inner_jobnet_id = %s and job_type = %d",
			inner_jobnet_id, JA_JOB_TYPE_START);

	if (NULL == (row = DBfetch(result)))
	{
		ja_log("JABOOT200003", i_inner_jobnet_id, NULL, 0, inner_jobnet_id, jobnet_id, update_date);
		DBfree_result(result);
		return FAIL;
	}

	rc = DBexecute("update ja_run_job_table set status = %d where inner_job_id = %s",
			JA_JOB_STATUS_RUNERR, row[0]);
	if (rc <= ZBX_DB_OK)
	{
		ja_log("JABOOT200002", i_inner_jobnet_id, NULL, 0, "ja_run_job_table", row[0]);
		DBfree_result(result);
		return FAIL;
	}

	rc = DBexecute("update ja_run_jobnet_table set status = %d where inner_jobnet_id = %s",
			JA_JOBNET_STATUS_RUNERR, inner_jobnet_id);
	if (rc <= ZBX_DB_OK)
	{
		ja_log("JABOOT200002", i_inner_jobnet_id, NULL, 0, "ja_run_jobnet_table", inner_jobnet_id);
		DBfree_result(result);
		return FAIL;
	}

	rc = DBexecute("update ja_run_jobnet_summary_table set"
			" status = %d, job_status = %d"
			" where inner_jobnet_id = %s",
			JA_JOBNET_STATUS_RUN, JA_SUMMARY_JOB_STATUS_ERROR, inner_jobnet_id);
	if (rc <= ZBX_DB_OK)
	{
		ja_log("JABOOT200002", i_inner_jobnet_id, NULL, 0, "ja_run_jobnet_summary_table", inner_jobnet_id);
		DBfree_result(result);
		return FAIL;
	}

	rc = ja_joblog(JC_JOBNET_START_ERR, i_inner_jobnet_id, 0, scheduled_time);

	DBfree_result(result);

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: start_jobnet                                                     *
 *                                                                            *
 * Purpose: change in the status of the jobnet ready to run                   *
 *                                                                            *
 * Parameters: inner_jobnet_id (in) - inner jobnet id                         *
 *             jobnet_id (in) - jobnet id                                     *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - an error occurred                                    *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	start_jobnet(char *inner_jobnet_id, char *jobnet_id)
{
	int		rc;
	const char	*__function_name = "start_jobnet";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s %s)", __function_name, inner_jobnet_id, jobnet_id);

	rc = DBexecute("update ja_run_jobnet_summary_table set status = %d where inner_jobnet_id = %s",
			JA_JOBNET_STATUS_READY, inner_jobnet_id);
	if (rc <= ZBX_DB_OK)
	{
		ja_log("JABOOT200002", 0, jobnet_id, 0, "ja_run_jobnet_summary_table", inner_jobnet_id);
		return FAIL;
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: jobnet_boot                                                      *
 *                                                                            *
 * Purpose: expand the immediate start jobnet                                 *
 *                                                                            *
 * Parameters: now_date (in) - current time (YYYYMMDDHHMM)                    *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - an error occurred                                    *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	jobnet_boot(char *now_date)
{
	DB_RESULT	result;
	DB_ROW		row;
	int		run_type, rc;
	const char	*__function_name = "jobnet_boot";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, now_date);

	result = DBselect("select inner_jobnet_id, run_type, scheduled_time, jobnet_id, update_date"
			" from ja_run_jobnet_summary_table"
			" where status = %d",
			JA_JOBNET_STATUS_BEGIN);

	while (NULL != (row = DBfetch(result)))
	{
		DBbegin();

		zabbix_log(LOG_LEVEL_DEBUG, "-DEBUG- get ja_run_jobnet_summary_table data:"
			" run_type[%s] scheduled_time[%s] now_date[%s]",
			row[1], row[2], now_date);

		run_type = atoi(row[1]);
		if (run_type == JA_JOBNET_RUN_TYPE_NORMAL ||
		    run_type == JA_JOBNET_RUN_TYPE_SCHEDULED)
		{
			/* excluded from execution */
			if (strcmp(row[2], now_date) > 0)
			{
				DBcommit();
				continue;
			}

			/* reform leak */
			if (strcmp(row[2], now_date) < 0)
			{
				rc = error_state_jobnet(row[0], row[3], row[4], row[2]);
				if (rc != SUCCEED)
				{
					DBrollback();
					continue;
				}
				DBcommit();
				continue;
			}
		}

		rc = start_jobnet(row[0], row[3]);
		if (rc != SUCCEED)
		{
			DBrollback();
			continue;
		}

		DBcommit();
	}
	DBfree_result(result);

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: get_running_jobs                                                 *
 *                                                                            *
 * Purpose: get the number of running jobs                                    *
 *                                                                            *
 * Parameters: inner_jobnet_id (in) - inner jobnet id                         *
 *                                                                            *
 * Return value:  number of running jobs                                      *
 *                FAIL - an error occurred                                    *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	get_running_jobs(char *inner_jobnet_id)
{
	DB_RESULT	result;
	DB_ROW		row;
	int		count;
	zbx_uint64_t	i_inner_jobnet_id;
	const char	*__function_name = "get_running_jobs";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, inner_jobnet_id);

	result = DBselect("select count(*) from ja_run_job_table"
			" where inner_jobnet_main_id = %s and status in(%d, %d)",
			inner_jobnet_id, JA_JOBNET_STATUS_READY, JA_JOBNET_STATUS_RUN);

	if (NULL == (row = DBfetch(result)))
	{
		ZBX_STR2UINT64(i_inner_jobnet_id, inner_jobnet_id);
		zbx_snprintf(msgwork, sizeof(msgwork), "%s (%d %d)",
			inner_jobnet_id, JA_JOBNET_STATUS_READY, JA_JOBNET_STATUS_RUN);
		ja_log("JABOOT200001", i_inner_jobnet_id, NULL, 0, "ja_run_job_table", msgwork);
		DBfree_result(result);
		return FAIL;
	}

	count = atoi(row[0]);

	DBfree_result(result);

	return count;
}

/******************************************************************************
 *                                                                            *
 * Function: jobnet_purge                                                     *
 *                                                                            *
 * Purpose: remove the jobnet information beyond the expiration date          *
 *                                                                            *
 * Parameters: jobnet_purge_date (in) - jobnet purge time (YYYYMMDDHHMM)      *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - an error occurred                                    *
 *                                                                            *
 * Comments: by deleting job net information, the job information by which    *
 *           foreign key combination was carried out, and flow information    *
 *           are also deleted automatically                                   *
 *                                                                            *
 ******************************************************************************/
static int	jobnet_purge(char *jobnet_purge_date)
{
	DB_RESULT	result;
	DB_ROW		row;
	int		rc, i_status;
	zbx_uint64_t	i_inner_jobnet_id;
	const char	*__function_name = "jobnet_purge";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, jobnet_purge_date);

	result = DBselect("select inner_jobnet_id, jobnet_id, update_date, status"
			" from ja_run_jobnet_summary_table"
			" where end_time <= %s59 and status in(%d, %d)",
			jobnet_purge_date, JA_JOBNET_STATUS_END, JA_JOBNET_STATUS_ENDERR);

	while (NULL != (row = DBfetch(result)))
	{
		DBbegin();

		zabbix_log(LOG_LEVEL_DEBUG, "-DEBUG- get jobnet status: [%s]", row[3]);

		i_status = atoi(row[3]);
		if (i_status == JA_JOBNET_STATUS_ENDERR)
		{
			/* stopped checks all jobs */
			rc = get_running_jobs(row[0]);
			if (rc == FAIL)
			{
				DBrollback();
				DBfree_result(result);
				return FAIL;
			}

			if (rc > 0)
			{
				/* skip delete */
				zabbix_log(LOG_LEVEL_DEBUG, "-DEBUG- skip delete: count[%d]", rc);
				DBcommit();
				continue;
			}
		}

		/* jobnet information delete */
		rc = DBexecute("delete from ja_run_jobnet_table"
				" where inner_jobnet_main_id = %s", row[0]);
		if (rc <= ZBX_DB_OK)
		{
			ZBX_STR2UINT64(i_inner_jobnet_id, row[0]);
			ja_log("JABOOT200004", i_inner_jobnet_id, NULL, 0, row[0], row[1], row[2]);
			DBrollback();
			continue;
		}

		DBcommit();
	}
	DBfree_result(result);

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: joblog_purge                                                     *
 *                                                                            *
 * Purpose: remove the jobnet information beyond the expiration date          *
 *                                                                            *
 * Parameters: joblog_purge_date (in) - joblog purge time (YYYYMMDDHHMM)      *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - an error occurred                                    *
 *                                                                            *
 * Comments: by deleting job net information, the job information by which    *
 *           foreign key combination was carried out, and flow information    *
 *           are also deleted automatically                                   *
 *                                                                            *
 ******************************************************************************/
static int	joblog_purge(char *joblog_purge_date)
{
	DB_RESULT	result;
	DB_ROW		row;
	int		rc;
	zbx_uint64_t	i_inner_jobnet_id;
	const char	*__function_name = "joblog_purge";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, joblog_purge_date);

	result = DBselect("select inner_jobnet_main_id from ja_run_log_table"
			" where log_date <= %s59999 group by inner_jobnet_main_id",
			joblog_purge_date);

	while (NULL != (row = DBfetch(result)))
	{
		DBbegin();

		zabbix_log(LOG_LEVEL_DEBUG, "-DEBUG- get joblog inner_jobnet_main_id: [%s]", row[0]);

		/* joblog delete */
		rc = DBexecute("delete from ja_run_log_table"
				" where inner_jobnet_main_id = %s", row[0]);
		if (rc <= ZBX_DB_OK)
		{
			ZBX_STR2UINT64(i_inner_jobnet_id, row[0]);
			ja_log("JABOOT200005", i_inner_jobnet_id, NULL, 0, row[0]);
			DBrollback();
			continue;
		}

		DBcommit();
	}
	DBfree_result(result);

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: main_jaboot_loop (main process)                                  *
 *                                                                            *
 * Purpose: Jobnet execution of deployed                                      *
 *          also, remove the information jobnet jobnet that ended             *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
void	main_jaboot_loop()
{
	struct tm	*tm;
	time_t		now;
	int		jobnet_keep_span, joblog_keep_span;
	char		*jobnet_purge_date, *joblog_purge_date;
	char		now_date[20];
	double		sec;

	zabbix_log(LOG_LEVEL_DEBUG, "In main_jaboot_loop() process_type:'%s' process_num:%d",
			ja_get_process_type_string(process_type), process_num);

	zbx_setproctitle("%s [connecting to the database]", ja_get_process_type_string(process_type));

	DBconnect(ZBX_DB_CONNECT_NORMAL);

	jobnet_keep_span = get_jobnet_keep_span();
	joblog_keep_span = get_joblog_keep_span();

	for (;;)
	{
		zbx_setproctitle("jobnet boot [start the jobnet and purge]");

		time(&now);
		tm = localtime(&now);
		strftime(now_date, sizeof(now_date), "%Y%m%d%H%M", tm);

		get_purge_date(jobnet_keep_span, &jobnet_purge_date);
		get_purge_date(joblog_keep_span, &joblog_purge_date);

		/* jobnet purge */
		sec = zbx_time();
		jobnet_purge(jobnet_purge_date);
		sec = zbx_time() - sec;

		zabbix_log(LOG_LEVEL_DEBUG, "%s #%d (jobnet purge) spent " ZBX_FS_DBL " seconds while processing rules",
				ja_get_process_type_string(process_type), process_num, sec);

		/* joblog purge */
		sec = zbx_time();
		joblog_purge(joblog_purge_date);
		sec = zbx_time() - sec;

		zabbix_log(LOG_LEVEL_DEBUG, "%s #%d (joblog purge) spent " ZBX_FS_DBL " seconds while processing rules",
				ja_get_process_type_string(process_type), process_num, sec);

		/* jobnet start */
		sec = zbx_time();
		jobnet_boot(now_date);
		sec = zbx_time() - sec;

		zabbix_log(LOG_LEVEL_DEBUG, "%s #%d (jobnet start) spent " ZBX_FS_DBL " seconds while processing rules",
				ja_get_process_type_string(process_type), process_num, sec);

		zbx_free(jobnet_purge_date);
		zbx_free(joblog_purge_date);

		ja_sleep_loop(CONFIG_JABOOT_INTERVAL);
	}
}
