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
** $Date:: 2013-05-16 16:03:58 +0900 #$
** $Revision: 4633 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"

#include "jacommon.h"
#include "jakill.h"

#ifdef _WINDOWS
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
int ja_kill(JA_PID job_pid)
{
    int i, cnt, size, pos;
    job_pid_t *jpt;
    HANDLE hSnapshot;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    DWORD pid, ppid, spid;
    const char *__function_name = "ja_kill";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() pid: %d", __function_name,
               job_pid);

    if (job_pid == 0)
        return SUCCEED;

    hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, job_pid);
    if (hProcess == NULL) {
        zabbix_log(LOG_LEVEL_ERR,
                   "In %s() job_pid: %d isn't existed. [%s]",
                   __function_name, job_pid,
                   strerror_from_system(GetLastError()));
        return FAIL;
    }
    CloseHandle(hProcess);

    pe32.dwSize = sizeof(PROCESSENTRY32);
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == (HANDLE) - 1) {
        zabbix_log(LOG_LEVEL_ERR,
                   "CreateToolhelp32Snapshot[%s]",
                   strerror_from_system(GetLastError()));
        return FAIL;
    }

    if (!Process32First(hSnapshot, &pe32)) {
        zabbix_log(LOG_LEVEL_ERR,
                   "Process32First[%s]",
                   strerror_from_system(GetLastError()));
        CloseHandle(hSnapshot);
        return FAIL;
    }

    cnt = 0;
    size = 0;
    jpt = NULL;
    do {
        pid = pe32.th32ProcessID;
        ppid = pe32.th32ParentProcessID;
        if (ppid <= 0 || pid == job_pid)
            continue;
        if (size <= cnt) {
            size += 1000;
            jpt = (job_pid_t *) zbx_realloc(jpt, sizeof(job_pid_t) * size);
        }
        jpt[cnt].pid = pid;
        jpt[cnt].ppid = ppid;
        cnt++;
    } while (Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);

    if (cnt == 0)
        return SUCCEED;

    spid = job_pid;
    pos = cnt;
    while (1) {
        for (i = 0; i < cnt; i++) {
            if (jpt[i].ppid == spid) {
                pos = i;
                spid = jpt[i].pid;
                break;
            }
        }
        if (pos == cnt)
            break;
        if (i == cnt) {
            zabbix_log(LOG_LEVEL_INFORMATION, "kill %d", jpt[pos].pid);
            hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, jpt[pos].pid);
            if (hProcess) {
                TerminateProcess(hProcess, SIGNALNO);
                CloseHandle(hProcess);
            } else {
                zabbix_log(LOG_LEVEL_INFORMATION, "OpenProcess [%s]",
                           strerror_from_system(GetLastError()));
            }
            jpt[pos].pid = 0;
            jpt[pos].ppid = 0;
            spid = job_pid;
            pos = cnt;
        }
    }

    zbx_free(jpt);
    return SUCCEED;
}

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
int ja_kill_ppid(JA_PID job_pid)
{
    int ret;
    HANDLE hSnapshot;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    DWORD pid, ppid;
    const char *__function_name = "ja_kill_ppid";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() pid: %d", __function_name,
               job_pid);

    ret = FAIL;
    hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, job_pid);
    if (hProcess == NULL)
        return FAIL;
    CloseHandle(hProcess);

    pe32.dwSize = sizeof(PROCESSENTRY32);
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == (HANDLE) - 1)
        return FAIL;
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return FAIL;
    }

    do {
        pid = pe32.th32ProcessID;
        ppid = pe32.th32ParentProcessID;
        if (ppid == job_pid) {
            zabbix_log(LOG_LEVEL_INFORMATION, "kill %d", pid);
            hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            if (hProcess) {
                TerminateProcess(hProcess, SIGNALNO);
                CloseHandle(hProcess);
                ret = SUCCEED;
                break;
            }
        }

    } while (Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);

    return ret;
}

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
int ja_process_check(JA_PID job_pid)
{
    HANDLE hProcess;

    hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, job_pid);
    if (hProcess == NULL)
        return FAIL;
    CloseHandle(hProcess);

    return SUCCEED;
}
#else
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
int ja_kill(JA_PID job_pid)
{
    int i, cnt, size, pos;
    job_pid_t *jpt;
    DIR *procdir;
    struct dirent *de;
    pid_t pid, ppid, spid;
    char statpath[JA_MAX_STRING_LEN];
    FILE *fp;
    const char *__function_name = "ja_kill";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() pid: %d", __function_name,
               job_pid);

    if (job_pid == 0)
        return SUCCEED;

    // check job_pid
    if (kill(job_pid, 0) != 0) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() job_pid: %d isn't existed",
                   __function_name, job_pid);
        return FAIL;
    }
    // open /proc 
    procdir = opendir("/proc");
    if (procdir == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() Can not opendir /proc [%s]",
                   __function_name, strerror(errno));
        return FAIL;
    }
    // load alll pid and ppid to job_pid_t list
    cnt = 0;
    size = 0;
    jpt = NULL;
    fp = NULL;
    while ((de = readdir(procdir)) != NULL) {
        pid = (pid_t) atoi(de->d_name);
        if (pid == 0)
            continue;
        zbx_snprintf(statpath, sizeof(statpath), "/proc/%d/stat", pid);
        if ((fp = fopen(statpath, "r")) == NULL)
            continue;
        if (fscanf(fp, "%*s %*s %*s %d", &ppid) == -1)
            ppid = 0;
        fclose(fp);
        if (ppid <= 1 || pid == job_pid)
            continue;
        if (size <= cnt) {
            size += 1000;
            jpt = (job_pid_t *) zbx_realloc(jpt, sizeof(job_pid_t) * size);
        }
        jpt[cnt].pid = pid;
        jpt[cnt].ppid = ppid;
        cnt++;
    }
    closedir(procdir);

    // can not match job_pid
    if (cnt == 0)
        return SUCCEED;

    spid = job_pid;
    pos = cnt;
    while (1) {
        for (i = 0; i < cnt; i++) {
            if (jpt[i].ppid == spid) {
                pos = i;
                spid = jpt[i].pid;
                break;
            }
        }
        if (pos == cnt)
            break;
        if (i == cnt) {
            zabbix_log(LOG_LEVEL_INFORMATION, "kill %d", jpt[pos].pid);
            if (kill(jpt[pos].pid, 9) != 0) {
                if (errno != 3) {
                    zabbix_log(LOG_LEVEL_ERR, "Can not kill pid: %s",
                               jpt[pos].pid);
                    zbx_free(jpt);
                    return FAIL;
                }
            }
            jpt[pos].pid = 0;
            jpt[pos].ppid = 0;
            spid = job_pid;
            pos = cnt;
        }
    }

    zbx_free(jpt);
    return SUCCEED;
}

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
int ja_kill_ppid(JA_PID job_pid)
{
    int ret;
    DIR *procdir;
    struct dirent *de;
    pid_t pid, ppid;
    FILE *fp;
    char statpath[JA_MAX_STRING_LEN];
    const char *__function_name = "ja_kill_ppid";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() pid: %d", __function_name,
               job_pid);

    ret = FAIL;
    if (kill(job_pid, 0) != 0)
        return FAIL;
    procdir = opendir("/proc");
    if (procdir == NULL)
        return FAIL;
    while ((de = readdir(procdir)) != NULL) {
        pid = (pid_t) atoi(de->d_name);
        if (pid == 0)
            continue;
        zbx_snprintf(statpath, sizeof(statpath), "/proc/%d/stat", pid);
        if ((fp = fopen(statpath, "r")) == NULL)
            continue;
        if (fscanf(fp, "%*s %*s %*s %d", &ppid) == -1)
            ppid = 0;
        fclose(fp);
        if (ppid == job_pid) {
            zabbix_log(LOG_LEVEL_INFORMATION, "kill %d", pid);
            kill(pid, 9);
            ret = SUCCEED;
            break;
        }
    }
    closedir(procdir);
    return ret;
}

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
int ja_process_check(JA_PID job_pid)
{
    if (kill(job_pid, 0) != 0)
        return FAIL;

    return SUCCEED;
}
#endif
