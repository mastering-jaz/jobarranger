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
** $Date:: 2013-05-28 16:28:01 +0900 #$
** $Revision: 4670 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_JASCHEDULE_H
#define JOBARG_JASCHEDULE_H

int ja_schedule_load_span();
int ja_schedule_get_calendar_id(const char *schedule_id,
                                char *calendar_id);
int ja_schedule_check_time(const char *schedule_id,
                           const char *update_date,
                           const char *operating_date,
                           const char *boot_time);

#endif
