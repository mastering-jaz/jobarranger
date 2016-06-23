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
** $Date:: 2012-11-26 13:22:31 +0900 #$
** $Revision: 2726 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_JADBMAIN_H
#define JOBARG_JADBMAIN_H

extern char *CONFIG_DATABASE_FILE;
extern int CONFIG_JOB_HISTORY;

int ja_dbmain_create(const char *filename);
int ja_dbmain_restore();
int ja_dbmain_recovery(sqlite3 * conn);
int ja_dbmain_mem_backup(sqlite3 * conn_src, sqlite3 * conn_dest);

int ja_dbmain_backup();
int ja_dbmain_remove_oldjob(int day);

#endif
