/*
** Job Arranger for ZABBIX
** Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.
** Copyright (C) 2013 Daiwa Institute of Research Business Innovation Ltd. All Rights Reserved.
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
** $Date:: 2013-05-17 17:04:52 +0900 #$
** $Revision: 4643 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_JATRAPAUTH_H
#define JOBARG_JATRAPAUTH_H

#include "comms.h"
#include "jatelegram.h"

int jatrap_auth_host(zbx_sock_t * sock, ja_telegram_object * obj);
zbx_uint64_t jatrap_auth_user(ja_telegram_object * obj);
int jatrap_auth_jobnet(zbx_uint64_t userid, zbx_uint64_t inner_jobnet_id);

#endif
