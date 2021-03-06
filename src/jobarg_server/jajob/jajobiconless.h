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
** $Date:: 2013-04-04 13:18:43 +0900 #$
** $Revision: 4269 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_JAJOBICONLESS_H
#define JOBARG_JAJOBICONLESS_H

int jajob_icon_less_abort(zbx_uint64_t inner_job_id, int force_stop);
int jajob_icon_less_timeout(zbx_uint64_t inner_job_id, char *start_time);

#endif
