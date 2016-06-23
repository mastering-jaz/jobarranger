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

#ifndef JOBARG_JALOG_H
#define JOBARG_JALOG_H

#define	JALOG_TYPE_INFO		0
#define	JALOG_TYPE_CRIT		1
#define	JALOG_TYPE_ERR		2
#define	JALOG_TYPE_WARN		3
#define	JALOG_TYPE_DEBUG	4

#define	JASENDER_OFF		0
#define	JASENDER_ON		1

#define	JOBARG_MESSAGE		"JOBARG_MESSAGE"

extern char	*CONFIG_ERROR_CMD_PATH;
extern char	*CONFIG_JA_LOG_MESSAGE_FILE;

int ja_log(char *message_id, zbx_uint64_t inner_jobnet_id, char *jobnet_id, zbx_uint64_t inner_job_id, ...);

#endif
