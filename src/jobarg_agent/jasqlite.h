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
** $Date:: 2013-03-21 10:55:37 +0900 #$
** $Revision: 4030 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_JADB_H
#define JOBARG_JADB_H

#include <sqlite3.h>
#include "common.h"

#define DB_ROW char **
#define DB_RESULT SQ_DB_RESULT *
#define DBfree_result SQ_DBfree_result

typedef struct {
    int curow;
    char **data;
    int nrow;
    int ncolumn;
    DB_ROW values;
} SQ_DB_RESULT;

#ifdef HAVE___VA_ARGS__
#define ja_db_execute(fmt, ...) __ja_jobarg_db_execute(ZBX_CONST_STRING(fmt), ##__VA_ARGS__)
#else
#define ja_db_execute __ja_jobarg_db_execute
#endif
int __ja_jobarg_db_execute(const char *fmt, ...);
int ja_db_vexecute(const char *fmt, va_list args);

#ifdef HAVE___VA_ARGS__
#define ja_db_select(fmt, ...) __ja_jobarg_db_select(ZBX_CONST_STRING(fmt), ##__VA_ARGS__)
#else
#define ja_db_select __ja_jobarg_db_select
#endif
DB_RESULT __ja_jobarg_db_select(const char *fmt, ...);
DB_RESULT ja_db_vselect(const char *fmt, va_list args);

sqlite3 *ja_db_getconn();
int ja_db_connect(const char *filename);
void ja_db_close();
int ja_db_begin();
int ja_db_commit();
int ja_db_rollback();
DB_ROW ja_db_fetch(DB_RESULT result);
void ja_db_free_result(DB_RESULT result);

#endif
