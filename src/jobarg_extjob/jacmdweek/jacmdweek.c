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

#define	JA_COMMAND_NAME		"jacmdweek"
#define	JA_COMMAND_VERSION	"1.0.0"
#define	JA_COMMAND_REVDATE	"5 Oct 2012"

#define DATE_LEN		20
#define MESSAGE_LEN		2048

#define	SUCCEED			0
#define	FAIL			-1

#define	MISMATCH		0
#define	MATCH_SUN		1
#define	MATCH_MON		2
#define	MATCH_TUE		3
#define	MATCH_WED		4
#define	MATCH_THU		5
#define	MATCH_FRI		6
#define	MATCH_SAT		7
#define	ABNORMAL		-1

char	msg[MESSAGE_LEN];
int	i_sun, i_mon, i_tue, i_wed, i_thu, i_fri, i_sat;


/******************************************************************************
 *                                                                            *
 * Function: check_week                                                       *
 *                                                                            *
 * Purpose: check the day of the week format                                  *
 *                                                                            *
 * Parameters: data (in) - string to be checked                               *
 *                         (Sun, Mon, Tue, Wed, Thu, Fri, Sat)                *
 *                                                                            *
 * Return value:  SUCCEED - processed successfully                            *
 *                FAIL - detect incorrect data                                *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int check_week(int cnt, char **data)
{
	int	size, i;
	char	*p, wday[4];

	i_sun = 0;
	i_mon = 0;
	i_tue = 0;
	i_wed = 0;
	i_thu = 0;
	i_fri = 0;
	i_sat = 0;

	if (cnt == 1)
	{
		i_sun = 1;
		i_mon = 2;
		i_tue = 3;
		i_wed = 4;
		i_thu = 5;
		i_fri = 6;
		i_sat = 7;
		return SUCCEED;
	}

	for (i = 1; i < cnt; i++)
	{
		size = strlen(data[i]);
		if (size != 3)
		{
			return FAIL;
		}

		strcpy(wday, data[i]);
		for (p = wday; *p != '\0'; p++)
		{
			*p = toupper(*p);
		}

		if (strcmp(wday, "SUN") == 0)
		{
			if (i_sun != 0)
			{
				return FAIL;
			}
			i_sun = 1;
			continue;
		}
		if (strcmp(wday, "MON") == 0)
		{
			if (i_mon != 0)
			{
				return FAIL;
			}
			i_mon = 2;
			continue;
		}
		if (strcmp(wday, "TUE") == 0)
		{
			if (i_tue != 0)
			{
				return FAIL;
			}
			i_tue = 3;
			continue;
		}
		if (strcmp(wday, "WED") == 0)
		{
			if (i_wed != 0)
			{
				return FAIL;
			}
			i_wed = 4;
			continue;
		}
		if (strcmp(wday, "THU") == 0)
		{
			if (i_thu != 0)
			{
				return FAIL;
			}
			i_thu = 5;
			continue;
		}
		if (strcmp(wday, "FRI") == 0)
		{
			if (i_fri != 0)
			{
				return FAIL;
			}
			i_fri = 6;
			continue;
		}
		if (strcmp(wday, "SAT") == 0)
		{
			if (i_sat != 0)
			{
				return FAIL;
			}
			i_sat = 7;
			continue;
		}
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

	if (argc > 8 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
		fprintf(stderr, "Job Arranger extended job command %s v%s (%s)\n\n",
			JA_COMMAND_NAME, JA_COMMAND_VERSION, JA_COMMAND_REVDATE); 
		fprintf(stderr, "usage: %s [<day of the week (Sun Mon Tue Wed Thu Fri Sat)>]\n\n",
			JA_COMMAND_NAME);
		return FAIL;
	}

	if (check_week(argc, argv) != SUCCEED)
	{
		sprintf(msg, "day of the week is invalid (%s)", argv[1]);
		write_message(msg);
		return FAIL;
	}

	return SUCCEED;
}

/******************************************************************************
 *                                                                            *
 * Function: main (jacmdweek)                                                 *
 *                                                                            *
 * Purpose: the specified day of the week is compared with the present day    *
 *          of the week. Inform the current day of the week If the parameter  *
 *          is omitted.                                                       *
 *                                                                            *
 * Call format: jacmdweek [P1-P7]                                             *
 *               [P1-P7] : day of the week (Sun,Mon,Tue,Wed,Thu,Fri,Sat)      *
 *                                                                            *
 *                P1(-h) : displays the command version and usage             *
 *                                                                            *
 * Return value:  0: day of the week is a mismatch                            *
 *                1: match sunday                                             *
 *                2: match monday                                             *
 *                3: match tuesday                                            *
 *                4: match wednesday                                          *
 *                5: match thursday                                           *
 *                6: match friday                                             *
 *                7: match saturday                                           *
 *               -1: an error occurred                                        *
 *                                                                            *
 * Comments: error will output an error message to standard error             *
 *                                                                            *
 ******************************************************************************/
int	main(int argc, char **argv)
{
	struct tm	*tm;
	time_t		now;
	int		rc, i_wday;

	/* parameter check */
	rc = param_check(argc, argv);
	if (rc != SUCCEED)
	{
		return ABNORMAL;
	}

	rc = MISMATCH;

	time(&now);
	tm = localtime(&now);

	i_wday = tm->tm_wday + 1;

	if (i_wday == i_sun || i_wday == i_mon || i_wday == i_tue ||
	    i_wday == i_wed || i_wday == i_thu || i_wday == i_fri ||
	    i_wday == i_sat)
	{
		rc = i_wday;
	}

	return rc;
}
