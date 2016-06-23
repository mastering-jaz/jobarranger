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
** $Date:: 2012-11-26 13:40:59 +0900 #$
** $Revision: 2728 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define	JA_COMMAND_NAME		"jacmdtime"
#define	JA_COMMAND_VERSION	"1.0.0"
#define	JA_COMMAND_REVDATE	"5 Oct 2012"

#define DATE_LEN		20
#define MESSAGE_LEN		2048

#define	SUCCEED			0
#define	FAIL			-1

#define	NORMAL			0
#define	ABNORMAL		-1

char	msg[MESSAGE_LEN];

/******************************************************************************
 *                                                                            *
 * Function: check_number                                                     *
 *                                                                            *
 * Purpose: check that it is a number                                         *
 *                                                                            *
 * Parameters: data (in) - string to be checked                               *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - detect incorrect data                                *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int check_number(char *data)
{
	int	i, size;

	size = strlen(data);
	for (i = 0; i < size; i++)
	{
		if (data[i] < '0' || data[i] > '9') {
			return FAIL;
		}
	}
	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: check_scheduled_time                                             *
 *                                                                            *
 * Purpose: check that it is a scheduled time format                          *
 *                                                                            *
 * Parameters: data (in) - string to be checked (YYYYMMDDHHMMSS)              *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - detect incorrect data                                *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int check_scheduled_time(char *data)
{
	int	size, i_year, i_mon, i_day, i_hour, i_min, i_sec, val, leap_year;
	char	year[5], mon[3], day[3], hour[3], min[3], sec[3];

	size = strlen(data);

	if (size != 14)
	{
		return FAIL;
	}
	

	strncpy(year,  data,       4);	year[4] = '\0';
	strncpy(mon,  (data + 4),  2);	mon[2]  = '\0';
	strncpy(day,  (data + 6),  2);	day[2]  = '\0';
	strncpy(hour, (data + 8),  2);	hour[2] = '\0';
	strncpy(min,  (data + 10), 2);	min[2]  = '\0';
	strncpy(sec,  (data + 12), 2);	sec[2]  = '\0';

	i_year = atoi(year);
	i_mon  = atoi(mon);
	i_day  = atoi(day);
	i_hour = atoi(hour);
	i_min  = atoi(min);
	i_sec  = atoi(sec);

	if (i_year < 1970 || i_year > 2100)
	{
		return FAIL;
	}

	if (i_mon < 1 || i_mon > 12)
	{
		return FAIL;
	}

	if (i_day < 1 || i_day > 31)
	{
		return FAIL;
	}

	if (i_mon == 2)
	{
		if ((val = i_year % 4) == 0)
		{
			leap_year = 1;
			if ((val = i_year % 100) == 0)
			{
				leap_year = 0;
				if ((val = i_year % 400) == 0)
				{
					leap_year = 1;
				}
			}
		}
		else
		{
			leap_year = 0;
		}
		if (leap_year == 0 && i_day > 28) {
			return FAIL;
		}
		if (leap_year == 1 && i_day > 29) {
			return FAIL;
		}
	}

	if (i_mon == 4 || i_mon == 6 || i_mon == 9 || i_mon == 11)
	{
		if (i_day > 30)
		{
			return FAIL;
		}
	}

	if (i_hour < 0 || i_hour > 23)
	{
		return FAIL;
	}

	if (i_min < 0 || i_min > 59)
	{
		return FAIL;
	}

	if (i_sec < 0 || i_sec > 59)
	{
		return FAIL;
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: check_wait_time                                                  *
 *                                                                            *
 * Purpose: check that it is a wait time format                               *
 *                                                                            *
 * Parameters: data (in) - string to be checked (HHMM[SS])                    *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - detect incorrect data                                *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int check_wait_time(char *data)
{
	int	size, i_hour, i_min, i_sec;
	char	hour[3], min[3], sec[3];

	size = strlen(data);

	if (size == 4 || size == 6)
	{
		/* OK */
	}
	else
	{
		return FAIL;
	}
	

	strncpy(hour,  data,      2);	hour[2] = '\0';
	strncpy(min,  (data + 2), 2);	min[2]  = '\0';
	i_hour = atoi(hour);
	i_min  = atoi(min);
	i_sec  = 0;

	if (size > 4)
	{
		strncpy(sec, (data + 4), 2);	sec[2] = '\0';
		i_sec = atoi(sec);
	}

	if (i_hour < 0 || i_hour > 99)
	{
		return FAIL;
	}

	if (i_min < 0 || i_min > 59)
	{
		return FAIL;
	}

	if (i_sec < 0 || i_sec > 59)
	{
		return FAIL;
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: write_message                                                    *
 *                                                                            *
 * Purpose: prints a message to standard error                                *
 *                                                                            *
 * Parameters: comment (in) - message body                                    *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
void write_message(char *comment)
{
	struct tm	*tm;
	time_t		now;
	char		now_date[DATE_LEN];

	time(&now);
	tm = localtime(&now);

	sprintf(now_date, "%04d/%02d/%02d %02d:%02d:%02d",
			(tm->tm_year + 1900),
			(tm->tm_mon  + 1),
			 tm->tm_mday,
			 tm->tm_hour,
			 tm->tm_min,
			 tm->tm_sec);

	fprintf(stderr, "[%s] %s\n", now_date, comment);

	return;
}
/******************************************************************************
 *                                                                            *
 * Function: param_check                                                      *
 *                                                                            *
 * Purpose: checks the startup parameters                                     *
 *                                                                            *
 * Parameters: argc (in) - number of arguments                                *
 *             argv (in) - arguments                                          *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - detect incorrect data                                *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	param_check(int argc, char **argv)
{

	if (argc < 2 || argc > 3 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
		fprintf(stderr, "Job Arranger extended job command %s v%s (%s)\n\n",
			JA_COMMAND_NAME, JA_COMMAND_VERSION, JA_COMMAND_REVDATE); 
		fprintf(stderr, "usage: %s <waiting time (HHMM[SS])> [<jobnet start time (YYYYMMDDHHMMSS)>]\n\n",
			JA_COMMAND_NAME);
		return FAIL;
	}

	if (check_number(argv[1]) != SUCCEED)
	{
		sprintf(msg, "wait time is invalid (%s)", argv[1]);
		write_message(msg);
		return FAIL;
	}

	if (check_wait_time(argv[1]) != SUCCEED)
	{
		sprintf(msg, "wait time format is invalid (%s)", argv[1]);
		write_message(msg);
		return FAIL;
	}

	if (argc > 2)
	{
		if (check_number(argv[2]) != SUCCEED)
		{
			sprintf(msg, "scheduled time is invalid (%s)", argv[2]);
			write_message(msg);
			return FAIL;
		}

		if (check_scheduled_time(argv[2]) != SUCCEED)
		{
			sprintf(msg, "scheduled time format is invalid (%s)", argv[2]);
			write_message(msg);
			return FAIL;
		}
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: get_wait_time                                                    *
 *                                                                            *
 * Purpose: edit the waiting time                                             *
 *                                                                            *
 * Parameters: argc (in) - number of arguments                                *
 *             argv (in) - arguments                                          *
 *             wait_time (out) - waiting time (YYYYMMDDHHMMSS)                *
 *                                                                            *
 * Return value:  0: successful completion                                    *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	get_wait_time(int argc, char **argv, char *wait_time)
{
	struct tm	*tm, stm;
	time_t		now;
	int		i_wtime, i_year, i_mon, i_day, i_pday, len;
	char		year[5], mon[3], day[3];
	char		w_time[DATE_LEN], w_time_ss[DATE_LEN], edit_time[DATE_LEN];

	time(&now);
	tm = localtime(&now);

	/* HHMMSS -> HHMM */
	strcpy(w_time, argv[1]);
	w_time[4] = '\0';

	len = strlen(argv[1]);
	if (len == 4)
	{
		strcpy(w_time_ss, "00");
	}
	else
	{
		/* HHMMSS -> SS */
		strncpy(w_time_ss, (argv[1] + 5), 2);
		w_time_ss[2] = '\0';
	}

	i_pday  = 0;
	i_wtime = atoi(w_time);
	if (i_wtime >= 2400)
	{
		i_pday  = i_wtime / 2400;
		i_wtime = i_wtime - (2400 * i_pday);
	}

	if (argc >= 3)
	{
		strncpy(year,  argv[2],       4);	year[4] = '\0';
		strncpy(mon,  (argv[2] + 4),  2);	mon[2]  = '\0';
		strncpy(day,  (argv[2] + 6),  2);	day[2]  = '\0';

		i_year = atoi(year);
		i_mon  = atoi(mon);
		i_day  = atoi(day);

		stm.tm_year  = i_year - 1900;
		stm.tm_mon   = i_mon - 1;
		stm.tm_mday  = i_day + i_pday;
	}
	else
	{
		stm.tm_year  = tm->tm_year;
		stm.tm_mon   = tm->tm_mon;
		stm.tm_mday  = tm->tm_mday + i_pday;
	}

	stm.tm_hour  = 0;
	stm.tm_min   = 0;
	stm.tm_sec   = 0;
	stm.tm_isdst = -1;

	mktime(&stm);

	sprintf(edit_time, "%04d%02d%02d",
			(stm.tm_year + 1900),
			(stm.tm_mon  + 1),
			 stm.tm_mday);

	sprintf(wait_time, "%s%04d%s", edit_time, i_wtime, w_time_ss);

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: main (jacmdtime)                                                 *
 *                                                                            *
 * Purpose: wait for processing until the specified time                      *
 *                                                                            *
 * Call format: jacmdtime P1 [P2]                                             *
 *                P1:  waiting time (HHMM[SS] : 0000-9959)                    *
 *               [P2]: jobnet start time (YYYYMMDDHHMMSS)                     *
 *                                                                            *
 *                P1(-h) : displays the command version and usage             *
 *                                                                            *
 * Return value:  0: successful completion                                    *
 *               -1: an error occurred                                        *
 *                                                                            *
 * Comments: error will output an error message to standard error             *
 *                                                                            *
 ******************************************************************************/
int	main(int argc, char **argv)
{
	struct tm	*tm;
	time_t		now;
	int		rc;
	char		now_time[DATE_LEN], wait_time[DATE_LEN];

	/* parameter check */
	rc = param_check(argc, argv);
	if (rc != SUCCEED)
	{
		return ABNORMAL;
	}

	get_wait_time(argc, argv, wait_time);

	while (1)
	{
		time(&now);
		tm = localtime(&now);
		strftime(now_time, DATE_LEN, "%Y%m%d%H%M%S", tm);

/*		printf("DATE CHECK wait_time[%s] now_time[%s]\n", wait_time, now_time); */  /* FOR DEBUG */

		if (strcmp(wait_time, now_time) <= 0)
		{
			break;
		}

		sleep(1);
	}

	return NORMAL;
}
