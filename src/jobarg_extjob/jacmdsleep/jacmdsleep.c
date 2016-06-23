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
** $Date:: 2014-04-25 14:25:43 +0900 #$
** $Revision: 5924 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define	JA_COMMAND_NAME		"jacmdsleep"
#define	JA_COMMAND_VERSION	"1.0.0"
#define	JA_COMMAND_REVDATE	"5 Oct 2012"

#define WAIT_TIME_MAX		999999

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
	int	wtime;

	if (argc != 2 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
		fprintf(stderr, "Job Arranger extended job command %s v%s (%s)\n\n",
			JA_COMMAND_NAME, JA_COMMAND_VERSION, JA_COMMAND_REVDATE); 
		fprintf(stderr, "usage: %s <waiting time (Second)>\n\n", JA_COMMAND_NAME);
		return FAIL;
	}

	if (check_number(argv[1]) != SUCCEED)
	{
		sprintf(msg, "wait time is invalid (%s)", argv[1]);
		write_message(msg);
		return FAIL;
	}

	wtime = atoi(argv[1]);
	if (wtime > WAIT_TIME_MAX)
	{
		sprintf(msg, "wait time is oversized (%s)", argv[1]);
		write_message(msg);
		return FAIL;
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: main (jacmdsleep)                                                *
 *                                                                            *
 * Purpose: the specified time(Second), the process wait                      *
 *                                                                            *
 * Call format: jacmdsleep P1                                                 *
 *                P1: waiting time (Second)                                   *
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
	int	rc, wtime;

	/* parameter check */
	rc = param_check(argc, argv);
	if (rc != SUCCEED)
	{
		return ABNORMAL;
	}

	wtime = atoi(argv[1]);

	sleep(wtime);

	return NORMAL;
}
