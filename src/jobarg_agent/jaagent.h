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
** $Date:: 2013-04-22 16:38:33 +0900 #$
** $Revision: 4446 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_JAAGENT_H
#define JOBARG_JAAGENT_H

#include "comms.h"

extern char *CONFIG_REQUEST_FLAG;
extern char *CONFIG_CMD_FILE;
extern char *CONFIG_TMPDIR;
extern int CONFIG_SEND_RETRY;
extern char *CONFIG_SOURCE_IP;
extern char *CONFIG_HOSTS_ALLOWED;
extern int CONFIG_SERVER_PORT;
extern int CONFIG_TIMEOUT;
extern char *CONFIG_REBOOT_FLAG;
extern char *CONFIG_REBOOT_FILE;

int ja_agent_setenv(ja_job_object * job, char *env_string);
int ja_agent_kill(ja_job_object * job);
int ja_agent_begin(ja_job_object * job);
int ja_agent_run(ja_job_object * job);
int ja_agent_end(ja_job_object * job);
int ja_agent_close(ja_job_object * job);
int ja_agent_send(ja_job_object * job);

#endif
