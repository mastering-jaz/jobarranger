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
** $Date:: 2012-11-26 13:25:18 +0900 #$
** $Revision: 2727 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_JAJOBFILE_H
#define JOBARG_JAJOBFILE_H

int ja_jobfile_create(const char *filepath, char *jobext[],
                      const char *script);
int ja_jobfile_remove(const char *filepath, char *jobext[]);
int ja_jobfile_chkend(const char *filepath, JA_PID pid);
int ja_jobfile_load(const char *filepath, ja_job_object * job);

#endif
