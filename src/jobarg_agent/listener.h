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
** $Date:: 2013-04-17 13:03:22 +0900 #$
** $Revision: 4412 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_LISTENER_H
#define JOBARG_LISTENER_H

#include "threads.h"

extern char *CONFIG_HOSTS_ALLOWED;
extern int CONFIG_TIMEOUT;
extern char *CONFIG_HOSTNAME;

ZBX_THREAD_ENTRY(listener_thread, args);

#endif
