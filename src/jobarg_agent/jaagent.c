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
** $Date:: 2016-04-13 14:58:32 +0900 #$
** $Revision: 7051 $
** $Author: sypark@FITECHLABS.CO.JP $
**/

#include <json.h>
#include "common.h"
#include "comms.h"
#include "log.h"

#if defined(ZABBIX_SERVICE)
#include "service.h"
#elif defined(ZABBIX_DAEMON)
#include "daemon.h"
#endif

#include "jacommon.h"
#include "jafile.h"
#include "jajobfile.h"
#include "jajobobject.h"
#include "jakill.h"
#include "jastr.h"
#include "jasqlite.h"
#include "jajobdb.h"
#include "jatelegram.h"
#include "jatcp.h"
#include "jathreads.h"
#include "jareboot.h"
#include "jaextjob.h"
#include "jaagent.h"

#ifdef _WINDOWS
#include "Windows.h"
#include <stdio.h>
#include <stdlib.h>
#include "locale.h"
#include "Userenv.h"
#include <winsafer.h>

#include <wincrypt.h>
#include <string.h>
#include <WinCrypt.h>
#define  KEYLENGTH_256   256 * 0x10000        /* 256-bit */
#endif

char *jobext[] = { "start", "end", "ret", "stdout", "stderr", NULL };

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
int ja_agent_setenv(ja_job_object * job, char *env_string)
{
    int ret;
    json_object *json_env;
    char *key, *env;
    char tmp_string[JA_VALUE_NAME_LEN + JA_STD_OUT_LEN];
    struct json_object *val;
    struct lh_entry *entry;
    const char *__function_name = "ja_job_object_setenv";

    if (job == NULL) {
        return FAIL;
    }

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64 ", env: %s", __function_name, job->jobid, job->env);

    memset(tmp_string, 0, JA_MAX_DATA_LEN);

    if (env_string != NULL) {
        memset(env_string, 0, JA_MAX_DATA_LEN);
    }

    if (strlen(job->env) == 0) {
        return SUCCEED;
    }

    ret = FAIL;
    json_env = json_tokener_parse(job->env);
    if (is_error(json_env)) {
        zbx_snprintf(job->message, sizeof(job->message), "Can not parse json env data [%s]", job->env);
        json_env = NULL;
        goto clear;
    }

    for (entry = json_object_get_object(json_env)->head;
        (entry ? (key = (char *) entry->k, val = (struct json_object *)entry->v, entry) : 0);
         entry = entry->next) {
        zabbix_log(LOG_LEVEL_DEBUG, "putenv %s=%s", key, json_object_get_string(val));
#ifdef _WINDOWS
        zbx_snprintf(tmp_string, JA_MAX_DATA_LEN, "%s", env_string);
        zbx_snprintf(env_string, JA_MAX_DATA_LEN, "%sset %s=%s\r\n", tmp_string, key, json_object_get_string(val));
#else
        zbx_snprintf(tmp_string, sizeof(tmp_string), "%s=%s", key, json_object_get_string(val));
        env = zbx_strdup(NULL, tmp_string);
        if (putenv(env) != 0) {
            zbx_snprintf(job->message, sizeof(job->message), "Can not putenv %s=%s", key, json_object_get_string(val));
            goto clear;
        }
#endif
    }
    ret = SUCCEED;

  clear:
    if (json_env != NULL) {
        json_object_put(json_env);
    }

    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "%s", job->message);
    }

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
int ja_agent_kill(ja_job_object * job)
{
    int ret, status;
    JA_PID pid;
    const char *__function_name = "ja_agent_kill";

    if (job == NULL) {
        return FAIL;
    }

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64, __function_name, job->jobid);

    ret = FAIL;
    if (ja_db_begin() != SUCCEED) {
        zbx_snprintf(job->message, sizeof(job->message), "Can not kill jobid:", ZBX_FS_UI64, job->jobid);
        goto error;
    }

    status = ja_jobdb_get_status(job->jobid);
    if (status == JA_AGENT_STATUS_RUN) {
        pid = ja_jobdb_get_pid(job->jobid);
        if (ja_kill(pid) == FAIL) {
            zbx_snprintf(job->message, sizeof(job->message), "Can not kill pid: %d, jobid: " ZBX_FS_UI64, pid, job->jobid);
            goto error;
        }
    } else {
        zbx_snprintf(job->message, sizeof(job->message), "jobid: " ZBX_FS_UI64 " is not running", job->jobid);
        goto error;
    }

    ret = SUCCEED;

  error:
    ja_db_commit();
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() %s", __function_name, job->message);
    }
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
int ja_agent_begin(ja_job_object * job)
{
    ja_reboot_arg reboot_arg;
    char reboot_flag_file[JA_MAX_STRING_LEN];
    int status;
    const char *__function_name = "ja_agent_begin";

    if (job == NULL) {
        return FAIL;
    }

    zabbix_log(LOG_LEVEL_INFORMATION, "In %s() jobid: " ZBX_FS_UI64 ", method: %d", __function_name, job->jobid, job->method);

    zbx_snprintf(reboot_flag_file, sizeof(reboot_flag_file), "%s-" ZBX_FS_UI64, CONFIG_REBOOT_FLAG, job->jobid);

    switch (job->method) {
    case JA_AGENT_METHOD_NORMAL:
    case JA_AGENT_METHOD_ABORT:
    case JA_AGENT_METHOD_TEST:
        if (strcmp(job->type, JA_PROTO_VALUE_REBOOT) == 0) {
            if (ja_reboot_load_arg(job, &reboot_arg) == FAIL) {
                goto error;
            }

            if (ja_file_create(reboot_flag_file, 1) == FAIL) {
                zbx_snprintf(job->message, sizeof(job->message), "Can not create the file [%s]", reboot_flag_file);
                goto error;
            }
            zbx_snprintf(job->script, sizeof(job->script), "\"%s\" \"%s\"", CONFIG_REBOOT_FILE, reboot_flag_file);
        }

        if (strcmp(job->type, JA_PROTO_VALUE_COMMAND) == 0 ||
            strcmp(job->type, JA_PROTO_VALUE_REBOOT)  == 0 ||
            strcmp(job->type, JA_PROTO_VALUE_EXTJOB)  == 0) {
            if (strcmp(job->type, JA_PROTO_VALUE_EXTJOB) == 0) {
                if (ja_extjob_script(job) == FAIL) {
                    goto error;
                }
            }
            job->status = JA_AGENT_STATUS_BEGIN;
            if (ja_jobdb_insert(job) == FAIL) {
                goto error;
            }
            if (ja_file_getsize(CONFIG_REQUEST_FLAG) < 0 ) {
            	if (ja_file_create(CONFIG_REQUEST_FLAG, 1) == FAIL) {
                	zbx_snprintf(job->message, sizeof(job->message), "Can not create the file [%s]", CONFIG_REQUEST_FLAG);
                    status = ja_jobdb_get_status(job->jobid);
                    if(status == JA_AGENT_STATUS_BEGIN){
                    	job->status = JA_AGENT_STATUS_CLOSE;
                    	if(ja_jobdb_update(job) == SUCCEED)
                    		goto error;
                    	else{
                    		zabbix_log(LOG_LEVEL_WARNING, "In %s() jobid[" ZBX_FS_UI64 ",] Close failed  %s", __function_name, job->jobid, job->message);
                    		if (ja_db_execute("delete from jobs where jobid = " ZBX_FS_UI64,job->jobid) == SUCCEED )
                    			goto error;
                    		else
                    			zabbix_log(LOG_LEVEL_ERR, "In %s() jobid[" ZBX_FS_UI64 ",] Delete failed  %s", __function_name, job->jobid, job->message);
                    	}
                    }
            	}
            }
        } else {
            zbx_snprintf(job->message, sizeof(job->message), "Invalid job type [%s]", job->type);
            goto error;
        }
        break;

    case JA_AGENT_METHOD_KILL:
        if (ja_agent_kill(job) == FAIL) {
            goto error;
        }
        if (strcmp(job->type, JA_PROTO_VALUE_REBOOT) == 0) {
            ja_file_remove(reboot_flag_file);
        }
        break;

    default:
        zbx_snprintf(job->message, sizeof(job->message), "Invalid method: %d, jobid " ZBX_FS_UI64 ")", job->method, job->jobid);
        goto error;
        break;
    }

    zabbix_log(LOG_LEVEL_INFORMATION, "jobid: " ZBX_FS_UI64 ", method: %d is begin", job->jobid, job->method);

    return SUCCEED;

  error:
    zabbix_log(LOG_LEVEL_ERR, "In %s() jobid[" ZBX_FS_UI64 ",] %s", __function_name, job->jobid, job->message);
    return FAIL;
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
int ja_agent_run(ja_job_object * job)
{
    JA_PID pid;
    int        ret;
    int        isSeteuid = 1;                               /* 0:run user,  1:not run user,  2:command user */
    char       filepath[JA_MAX_STRING_LEN];
    char       full_command[JA_MAX_STRING_LEN];
    char       w_user[JA_MAX_STRING_LEN];
    char       w_passwd[JA_MAX_STRING_LEN];
    char       cmd_user[JA_MAX_STRING_LEN];
    char       cmd_passwd[JA_MAX_STRING_LEN];
    const char *__function_name = "ja_agent_run";

	//Park.iggy ADD START
	char    d_passwd[JA_MAX_STRING_LEN]; //16�i������char�ɕύX
	char    d_dec[256];
	char    d_flag[2]="1";
    char    d_x16[3];
    char    *d_cat="0x";
    char    d_catX16[5];

	int     k,kk,x16toX10;

	//Park.iggy END

#ifdef _WINDOWS
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    LPTSTR wcommand;
    char env[JA_MAX_DATA_LEN], full_script[JA_MAX_DATA_LEN];

    int     i;
    int     j;
    wchar_t user[256];
    wchar_t pwd[256];
    size_t    wLen;
    HANDLE  hToken;

    char    dec[256];
    char    *key = "199907";

    BYTE    pbData[256];
    DWORD   dwDataLen=(DWORD)(strlen((char*)pbData)+1);
    DWORD   strLen=1000;
#endif

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    if (job == NULL) {
        return FAIL;
    }

    zabbix_log(LOG_LEVEL_INFORMATION, "jobid: " ZBX_FS_UI64 ", status: %d", job->jobid, job->status);

    ret = FAIL;
    pid = 0;
    job->start_time = time(NULL);
    job->end_time = time(NULL);
    zbx_snprintf(filepath, JA_MAX_STRING_LEN, "%s%c%s-" ZBX_FS_UI64, CONFIG_TMPDIR, JA_DLM, progname, job->jobid);

#ifdef _WINDOWS
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    GetStartupInfo(&si);
    wcommand = NULL;

    if (ja_agent_setenv(job, env) == FAIL)
        goto error;
    if (job->method == JA_AGENT_METHOD_TEST) {
        zbx_snprintf(full_script, JA_MAX_DATA_LEN, "%s", env);
    } else {
        zbx_snprintf(full_script, JA_MAX_DATA_LEN, "%s%s", env, job->script);
    }
    if (ja_jobfile_create(filepath, jobext, full_script) == FAIL) {
        zbx_snprintf(job->message, sizeof(job->message), "Can not create the result files [%s]", filepath);
        goto error;
    }
    zbx_snprintf(full_command, JA_MAX_STRING_LEN, "\"%s\" \"%s\" \"%s.%s\"", CONFIG_CMD_FILE, filepath, filepath, JA_EXE);
    wcommand = zbx_acp_to_unicode(full_command);

    zbx_snprintf(w_user,     sizeof(w_user),     "%s", job->run_user);
    zbx_snprintf(w_passwd,   sizeof(w_passwd),   "%s", job->run_user_password);

	//Park.iggy ADD START
	zbx_snprintf(d_passwd,   sizeof(d_passwd),   "%s", job->run_user_password);
	//zabbix_log(LOG_LEVEL_DEBUG, "[jaagent]    d_passwd (Encryption) = %s  ", d_passwd );
	//Park.iggy END

    zbx_snprintf(cmd_user,   sizeof(cmd_user),   "%s", CONFIG_JA_COMMAND_USER);
    zbx_snprintf(cmd_passwd, sizeof(cmd_passwd), "%s", CONFIG_JA_COMMAND_PASSWORD);

    zabbix_log(LOG_LEVEL_DEBUG, "[jaagent]   run user = %s ,  passwd (Encryption) = %s  ", w_user, w_passwd );
    zabbix_log(LOG_LEVEL_DEBUG, "[jaagent]   config user = %s ,  passwd = %s  ", cmd_user, cmd_passwd );

    if ((strcmp(w_user, "") != 0) && (strcmp(w_user, "(null)") != 0)) {              /* run_user specified ? */
        isSeteuid = 0;
    } else if ((strcmp(cmd_user, "") != 0) && (strcmp(cmd_user, "(null)") != 0)) {   /* is command_user specified as jobarg_agentd.conf */
        zbx_snprintf(w_user,   sizeof(w_user),   "%s", CONFIG_JA_COMMAND_USER);
        zbx_snprintf(w_passwd, sizeof(w_passwd), "%s", CONFIG_JA_COMMAND_PASSWORD);
        isSeteuid = 2;
    } else {
        zbx_snprintf(w_user,   sizeof(w_user),   "%s", "");
        zbx_snprintf(w_passwd, sizeof(w_passwd), "%s", "");
    }

    /* if not command, not CreateProcessAsUser() */
    if (strcmp(job->type, JA_PROTO_VALUE_COMMAND) != 0) {
        isSeteuid = 1;
        zbx_snprintf(w_user, sizeof(user), "%s", "");
    }

    if (( isSeteuid == 0 ) || ( isSeteuid == 2 )) {
        /* decodes password */
        if ( isSeteuid == 0 ){

			//Park.iggy ADD START
			if(d_flag[0] == d_passwd[0]) {
				j=0;
				k=0;
				for(kk = 1; kk < strlen(d_passwd) ; kk++){
					if((kk%2) != 0){
						d_x16[0] = d_passwd[kk];
					}else{
						d_x16[1] = d_passwd[kk];
						d_x16[2] = '\0';
						zbx_snprintf(d_catX16,   sizeof(d_catX16),   "0x%s", d_x16);
						x16toX10 = (unsigned long)strtol(d_catX16,NULL,0);
						*d_x16 = NULL;
						*d_catX16 = NULL;
						d_dec[k] = (char)(x16toX10) ;
						dec[k] = (char)(d_dec[k]^key[j]);

						j++;
						k++;
						if (j == strlen(key)) j =0;
					}
				}
				*w_passwd = NULL;
			}
			//Park.iggy END

            j = 0;
            for (i = 0; i < strlen(w_passwd); i++)
            {
                dec[i] = (char)(w_passwd[i]^key[j]);
                j++;
                if (j == strlen(key)) j =0;
            }
            memset( w_passwd , '\0' , strlen( w_passwd ) );
            zbx_snprintf(w_passwd, sizeof(w_passwd), "%s", dec);
            zabbix_log(LOG_LEVEL_DEBUG, "[jaagent]   password (Decryption) = %s  ", w_passwd );
        }

        mbstowcs_s(&wLen, user, sizeof(user)/2, w_user,   _TRUNCATE);
        mbstowcs_s(&wLen, pwd,  sizeof(pwd)/2,  w_passwd, _TRUNCATE);

        /* attempt to log a user on to the local computer */
        if (!LogonUser(
            user,                      /* A pointer to a null-terminated string that specifies the name of the user                           */
            NULL,                      /* A pointer to a null-terminated string that specifies the name of the domain or server               */
            pwd,                       /* A pointer to a null-terminated string that specifies the plaintext password for the user            */
            LOGON32_LOGON_INTERACTIVE, /* The type of logon operation to perform                                                              */
            LOGON32_PROVIDER_DEFAULT,  /* Specifies the logon provider                                                                        */
            &hToken                    /* A pointer to a handle variable that receives a handle to a token that represents the specified user */
        )){
            job->result = JA_JOBRESULT_FAIL;
            job->status = JA_AGENT_STATUS_END;
            if (GetLastError() == ERROR_LOGON_FAILURE){
                zbx_snprintf(job->message, sizeof(job->message),
                                   "can not specify the user account for  '%ws' ", user);
            }else{
                zbx_snprintf(job->message, sizeof(job->message),
                                   "failed to LogonUser function");
            }
            ja_jobdb_update(job);
            ret = FAIL;
            CloseHandle(hToken);
            zbx_free(wcommand);
            goto error;
        }
        CloseHandle(hToken);

        zbx_snprintf(full_command, JA_MAX_STRING_LEN, "\"%s\" \"%s\" \"%s.%s\" \"%s\" \"%s\"",
                     CONFIG_CMD_FILE, filepath, filepath, JA_EXE, w_user, w_passwd);
        wcommand = zbx_acp_to_unicode(full_command);
    }else{
        zbx_snprintf(full_command, JA_MAX_STRING_LEN, "\"%s\" \"%s\" \"%s.%s\" \"%s\" \"%s\"",
                     CONFIG_CMD_FILE, filepath, filepath, JA_EXE, "", "");
        wcommand = zbx_acp_to_unicode(full_command);
    }

    if (0 == CreateProcess(NULL,        /* no module name (use command line) */
                           wcommand,    /* name of app to launch */
                           NULL,        /* default process security attributes */
                           NULL,        /* default thread security attributes */
                           FALSE,       /* do not inherit handles from the parent */
                           0,           /* normal priority */
                           NULL,        /* use the same environment as the parent */
                           NULL,        /* launch in the current directory */
                           &si,         /* startup information */
                           &pi          /* process information stored upon return */
        )) {
        zbx_snprintf(job->message, sizeof(job->message), "failed to create process for jobid " ZBX_FS_UI64 " [%s]: %s",
                     job->jobid, full_command, strerror_from_system(GetLastError()));
        ret = FAIL;
    } else {
        job->pid = pi.dwProcessId;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        ret = SUCCEED;
    }
    zbx_free(wcommand);

#else
    if (job->method == JA_AGENT_METHOD_TEST) {
        ret = ja_jobfile_create(filepath, jobext, "");
    } else {
        ret = ja_jobfile_create(filepath, jobext, job->script);
    }
    if (ret == FAIL) {
        zbx_snprintf(job->message, sizeof(job->message), "Can not create the result files [%s]", filepath);
        goto error;
    }
    zbx_snprintf(full_command, JA_MAX_STRING_LEN, "%s.%s", filepath, JA_EXE);

    pid = ja_fork();
    if (pid == -1) {
        ret = FAIL;
        zbx_snprintf(job->message, sizeof(job->message), "ja_fork() failed for jobid: " ZBX_FS_UI64 "[%s]",
                     job->jobid, zbx_strerror(errno));
        goto error;
    } else if (pid == 0) {
        zbx_snprintf(w_user,     sizeof(w_user),     "%s", job->run_user);
        zbx_snprintf(w_passwd,   sizeof(w_passwd),   "%s", job->run_user_password);
        zbx_snprintf(cmd_user,   sizeof(cmd_user),   "%s", CONFIG_JA_COMMAND_USER);
        zbx_snprintf(cmd_passwd, sizeof(cmd_passwd), "%s", CONFIG_JA_COMMAND_PASSWORD);

        zabbix_log(LOG_LEVEL_DEBUG, "[jaagent]   run_user = %s ,  run_passwd = %s  ", w_user, w_passwd);
        zabbix_log(LOG_LEVEL_DEBUG, "[jaagent]   config_user = %s ,  config_passwd = %s  ", cmd_user, cmd_passwd);

        isSeteuid = 1;

        /* run_user specified ? */
        if ((strcmp(w_user, "") != 0) && (strcmp(w_user, "(null)") != 0)) {
            isSeteuid = 0;
        } else if ((strcmp(cmd_user, "") != 0) && (strcmp(cmd_user, "(null)") != 0)) {   /* is command_user specified as jobarg_agentd.conf */
            zbx_snprintf(w_user, sizeof(w_user), "%s", CONFIG_JA_COMMAND_USER);
            isSeteuid = 2;
        }

        /* if not command, not seteuid() */
        if (strcmp(job->type, JA_PROTO_VALUE_COMMAND) != 0) {
            isSeteuid = 1;
            zbx_snprintf(w_user, sizeof(w_user), "%s", "");
        }

        if (isSeteuid == 0 || isSeteuid == 2) {
            /* the agent to run as 'root' ? */
            if (0 == getuid() || 0 == getgid()){
                /* set the agent UID to the realUID( run_user/command_user) */

                struct passwd *pwd;
                /* get the local password file that matches user name */
                pwd = getpwnam(w_user);

                if (NULL == pwd){
                    zbx_sleep(1);
                    job->result = JA_JOBRESULT_FAIL;
                    job->status = JA_AGENT_STATUS_END;
                    zbx_snprintf(job->message, sizeof(job->message), "User [ %s ] does not exist", w_user);
                    ja_jobdb_update(job);
                    exit(-1);
                }
            }else{
                /* the agent not to run as 'root' */
                zbx_sleep(1);
                job->result = JA_JOBRESULT_FAIL;
                job->status = JA_AGENT_STATUS_END;
                zbx_snprintf(job->message, sizeof(job->message), "Agent does not to run as 'root'");
                ja_jobdb_update(job);
                exit(-1);
            }
        }

        if (ja_agent_setenv(job, NULL) == SUCCEED) {
            /* execl(CONFIG_CMD_FILE, CONFIG_CMD_FILE, filepath, full_command, NULL); */
            execl(CONFIG_CMD_FILE, CONFIG_CMD_FILE, filepath, full_command, w_user, w_passwd, NULL);

            zbx_snprintf(job->message, sizeof(job->message),
                         "execl() failed for [%s]: %s", CONFIG_CMD_FILE,
                         zbx_strerror(errno));
        }
        zbx_sleep(1);
        job->result = JA_JOBRESULT_FAIL;
        job->status = JA_AGENT_STATUS_END;
        ja_jobdb_update(job);
        exit(1);
    } else {
        waitpid(pid, NULL, WNOHANG);
        job->pid = pid;
        ret = SUCCEED;
    }
#endif

  error:
    if (ret == FAIL) {
        job->result = JA_JOBRESULT_FAIL;
        job->status = JA_AGENT_STATUS_END;
        zabbix_log(LOG_LEVEL_ERR, "In %s() %s", __function_name, job->message);
    } else {
        job->status = JA_AGENT_STATUS_RUN;
    }
    ja_jobdb_update(job);
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
int ja_agent_end(ja_job_object * job)
{
    int ret, chk;
    char filepath[JA_MAX_STRING_LEN];

    const char *__function_name = "ja_agent_end";
    zabbix_log(LOG_LEVEL_DEBUG, "In %s() jobid: " ZBX_FS_UI64 ", status: %d", __function_name, job->jobid, job->status);

    if (strcmp(job->type, JA_PROTO_VALUE_REBOOT) == 0) {
        if (ja_reboot_chkend(job) == FAIL) {
            return FAIL;
        }
    }

    zbx_snprintf(filepath, JA_MAX_STRING_LEN, "%s%c%s-" ZBX_FS_UI64, CONFIG_TMPDIR, JA_DLM, progname, job->jobid);

    chk = ja_jobfile_chkend(filepath, job->pid, job->type);
    if (chk == 0) {
        return FAIL;
    }

    ret         = FAIL;
    job->status = JA_AGENT_STATUS_END;

    if (chk == -1) {
        if (ja_jobdb_get_status(job->jobid) == JA_AGENT_STATUS_END) {
            ja_jobdb_load_jobid(job->jobid, job);
            job->result = JA_JOBRESULT_FAIL;
        } else {
            job->result = JA_JOBRESULT_FAIL;
            zbx_snprintf(job->message, sizeof(job->message), "Check job status(end) failed. jobid: " ZBX_FS_UI64, job->jobid);
        }
    } else {
        if (ja_jobfile_load(filepath, job) == FAIL) {
            job->result = JA_JOBRESULT_FAIL;
            zbx_snprintf(job->message, sizeof(job->message), "Can not load the result files[%s]. jobid: " ZBX_FS_UI64, filepath, job->jobid);
        } else {
            ret = SUCCEED;
        }
    }

    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "In %s() %s", __function_name, job->message);
    }

    if (ja_jobdb_update(job) != SUCCEED) {
        job->status = JA_AGENT_STATUS_RUN;
    }

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
int ja_agent_close(ja_job_object * job)
{
    char filepath[JA_MAX_STRING_LEN];
    const char *__function_name = "ja_agent_close";

    zabbix_log(LOG_LEVEL_INFORMATION, "In %s() jobid: " ZBX_FS_UI64 ", status: %d", __function_name, job->jobid, job->status);

    if (ja_agent_send(job) == FAIL) {
        job->send_retry++;
        if (job->send_retry >= CONFIG_SEND_RETRY) {
            zbx_snprintf(job->message, sizeof(job->message), "Can not send the result to server. jobid: " ZBX_FS_UI64, job->jobid);
            zabbix_log(LOG_LEVEL_ERR, "In %s() message: %s",  __function_name, job->message);
            job->status = JA_AGENT_STATUS_CLOSE;
            ja_jobdb_update(job);
        }
        return FAIL;
    }

    job->status = JA_AGENT_STATUS_CLOSE;
    ja_jobdb_update(job);
    zbx_snprintf(filepath, JA_MAX_STRING_LEN, "%s%c%s-" ZBX_FS_UI64, CONFIG_TMPDIR, JA_DLM, progname, job->jobid);

    if (job->result == JA_JOBRESULT_SUCCEED) {
        ja_jobfile_remove(filepath, jobext);
    }

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
int ja_agent_send(ja_job_object * job)
{
    int ret;
    zbx_sock_t s;
    char *data;
    json_object *response;
    json_object *jp_data, *jp;
    int result;
    const char *__function_name = "ja_agent_send";

    response = NULL;
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    if (job == NULL) {
        return FAIL;
    }

    if (job->method == JA_AGENT_METHOD_ABORT) {
        return SUCCEED;
    }

    zbx_snprintf(job->hostname, sizeof(job->hostname), "%s", CONFIG_HOSTNAME);

    ret = zbx_tcp_connect(&s, CONFIG_SOURCE_IP, CONFIG_HOSTS_ALLOWED, CONFIG_SERVER_PORT, CONFIG_TIMEOUT);
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "Send job result error: [connect] %s", zbx_tcp_strerror());
        return FAIL;
    }

    zbx_snprintf(job->kind, sizeof(job->kind), "%s", JA_PROTO_VALUE_JOBRESULT);
    ret = ja_tcp_send_to(&s, job, CONFIG_TIMEOUT);
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_WARNING, "Send job result error: [send] %s", zbx_tcp_strerror());
        goto error;
    }

    ret = zbx_tcp_recv_to(&s, &data, CONFIG_TIMEOUT);
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_WARNING, "Send job result error: [recv] %s", zbx_tcp_strerror());
        goto error;
    }

    if (strlen(data) == 0) {
        goto error;
    }

    response = json_tokener_parse(data);
    if (is_error(response)) {
        zabbix_log(LOG_LEVEL_WARNING, "the recv data is not json data. %s", data);
        response = NULL;
        goto error;
    }

    jp_data = json_object_object_get(response, JA_PROTO_TAG_DATA);
    if (jp_data == NULL) {
        zabbix_log(LOG_LEVEL_WARNING, "can not get the tag [%s] from json data [%s]", JA_PROTO_TAG_DATA, data);
        goto error;
    }

    jp = json_object_object_get(jp_data, JA_PROTO_TAG_RESULT);
    result = json_object_get_int(jp);
    if (result == FAIL) {
        jp = json_object_object_get(jp_data, JA_PROTO_TAG_MESSAGE);
        zabbix_log(LOG_LEVEL_ERR, "job response message: %s", json_object_get_string(jp));
    }

    ret = SUCCEED;

  error:
    zbx_tcp_close(&s);
    if (response != NULL) {
        json_object_put(response);
    }

    return ret;
}
