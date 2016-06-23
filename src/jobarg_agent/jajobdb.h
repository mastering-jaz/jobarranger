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

#ifndef JOBARG_JAJOBDB_H
#define JOBARG_JAJOBDB_H

#include "jacommon.h"
#include "jasqlite.h"

extern char *CONFIG_DATABASE_FILE;

int ja_jobdb_init();
int ja_jobdb_load(DB_ROW row, ja_job_object * job);
int ja_jobdb_load_jobid(const zbx_uint64_t jobid, ja_job_object * job);
int ja_jobdb_get_status(const zbx_uint64_t jobid);
JA_PID ja_jobdb_get_pid(const zbx_uint64_t jobid);
int ja_jobdb_replace(ja_job_object * job);
int ja_jobdb_insert(ja_job_object * job);
int ja_jobdb_update(ja_job_object * job);
int ja_jobdb_get_jobs();

#endif
