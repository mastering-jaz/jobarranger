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
** $Date:: 2014-02-20 16:52:27 +0900 #$
** $Revision: 5809 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"
#include "db.h"
#include "dbcache.h"

#include "jacommon.h"
#include "jazbxmsg.h"

/******************************************************************************
 *                                                                            *
 * Function: ja_zabbix_message                                                *
 *                                                                            *
 * Purpose: get Zabbix status change message body                             *
 *                                                                            *
 * Parameters: message_id (in) - message id                                   *
 *             lang (in) - message language                                   *
 *             inner_job_id (in) - inner job id                               *
 *                                                                            *
 * Return value:  pointer of the message body                                 *
 *                NULL - an error occurred                                    *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
char *ja_zabbix_message(char *message_id, char *lang, zbx_uint64_t inner_job_id)
{
	FILE		*fp;
	int		hit, m, n, cnt;
	char		*name = NULL, *mlang = NULL, *msg = NULL, *p = NULL;
	char		line[4096];


	/* zabbix message file open */
	fp = fopen(CONFIG_JA_ZBX_MESSAGE_FILE, "r");
	if (fp == NULL)
	{
		ja_log("JATRGMSG200001", 0, NULL, inner_job_id, CONFIG_JA_ZBX_MESSAGE_FILE);
		return NULL;
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
		name  = line;
		mlang = NULL;
		msg   = NULL;
		while (line[++n])
		{
			if (line[n] == ',')
			{
				if (m == 0)
				{
					line[n] = '\0';
					mlang = &line[n + 1];
					m++;
				}
				else if (m == 1)
				{
					line[n] = '\0';
					msg = &line[n + 1];
					break;
				}
			}
		}

		lrtrim_spaces(name);
		lrtrim_spaces(mlang);

		/* convert all characters to lower case */
		for (p = mlang; *p != '\0'; p++) {
			*p = tolower(*p);
		}

		for (p = lang; *p != '\0'; p++) {
			*p = tolower(*p);
		}

		if (strcmp(name, message_id) == 0 && strcmp(mlang, lang) == 0)
		{
			hit = 1;
			break;
		}
	}
	fclose(fp);

	/* message hit check */
	if (hit == 0)
	{
		ja_log("JATRGMSG200002", 0, NULL, inner_job_id, message_id, lang);
		return NULL;
	}

	/* message get check */
	if (name == NULL || mlang == NULL || msg == NULL)
	{
		ja_log("JATRGMSG200003", 0, NULL, inner_job_id, cnt, message_id, CONFIG_JA_ZBX_MESSAGE_FILE);
		return NULL;
	}

	return DBdyn_escape_string(msg);

}
