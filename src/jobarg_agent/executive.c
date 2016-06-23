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

#include "common.h"
#include "log.h"

#if defined(ZABBIX_SERVICE)
#include "service.h"
#elif defined(ZABBIX_DAEMON)
#include "daemon.h"
#endif

#include "jasqlite.h"
#include "jadbmain.h"
#include "jajobobjectlist.h"
#include "executive.h"

/******************************************************************************
 *                                                                            *
 * Function:                                                                  *
 *                                                                            *
 * Purpose:                                                                   *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
ZBX_THREAD_ENTRY(executive_thread, args)
{
    int local_request_failed = 0;
    ja_job_object_list *jobs;

    assert(args);

    zabbix_log(LOG_LEVEL_INFORMATION,
               "jobarg_agentd #%d started [executive]",
               ((zbx_thread_args_t *) args)->thread_num);
    zbx_free(args);

    jobs = ja_job_object_list_new();
    if (jobs == NULL) {
        zabbix_log(LOG_LEVEL_CRIT, "Create job object list failed");
        goto error;
    }

    while (ZBX_IS_RUNNING()) {
        zbx_setproctitle("process executive");
        if (ja_job_object_list_executive(jobs) == FAIL)
            local_request_failed++;

        if (local_request_failed > 1000) {
            zabbix_log(LOG_LEVEL_WARNING,
                       "Too many consecutive errors on executive() call.");
            local_request_failed = 0;
        }

        if (time(NULL) % (CONFIG_BACKUP_TIME * 3600) == 0)
            ja_dbmain_backup();
        zbx_sleep(1);
    }

  error:
    if (jobs != NULL)
        ja_job_object_list_delete(jobs);
    zabbix_log(LOG_LEVEL_INFORMATION, "jobarg_agentd executive stopped");
    ZBX_DO_EXIT();
    zbx_thread_exit(0);
}
