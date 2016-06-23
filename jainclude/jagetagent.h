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
** $Date:: 2013-05-08 10:51:46 +0900 #$
** $Revision: 4578 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_JA_GET_AGENT_H
#define JOBARG_JA_GET_AGENT_H

#define USE_IPADDRESS		1
#define USE_DNS			0
#define USE_HOSTNAME		0
#define USE_HOSTVARIABLE	1
typedef struct agent_info {
    const char *host;
    unsigned short port;
} JA_AGENT_INFO;

int get_agent_info(zbx_uint64_t inner_job_id, zbx_uint64_t inner_jobnet_id,
                   JA_AGENT_INFO * agent_info);
char *get_agent_hostname(zbx_uint64_t inner_job_id,
                         zbx_uint64_t inner_jobnet_id);

#endif
