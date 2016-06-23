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
** $Date:: 2013-10-11 10:26:38 +0900 #$
** $Revision: 5276 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "comms.h"
#include "log.h"
#include "cfg.h"
#include "zbxgetopt.h"
#include "sysinfo.h"

#if defined(ZABBIX_SERVICE)
#include "service.h"
#elif defined(ZABBIX_DAEMON)
#include "daemon.h"
#endif

#include "jacommon.h"
#include "jahostname.h"
#include "jajobdb.h"
#include "listener.h"
#include "executive.h"
#include "jobarg_agentd.h"

/******************************************************************************
 *                                                                            *
 *                                                                            *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
const char *progname = NULL;

/* Default config file location */
#ifdef _WINDOWS
static char DEFAULT_CONFIG_FILE[] = "C:\\jobarg_agentd.conf";
#else
static char DEFAULT_CONFIG_FILE[] = SYSCONFDIR "/jobarg_agentd.conf";
#endif

/* application TITLE */
const char title_message[] = "Job Arranger Agent"
#if defined(_WIN64)
    " Win64"
#elif defined(WIN32)
    " Win32"
#endif
#if defined(ZABBIX_SERVICE)
    " (service)"
#elif defined(ZABBIX_DAEMON)
    " (daemon)"
#endif
    ;
/* end of application TITLE */

/* application USAGE message */
const char usage_message[] = "[-Vh]"
#ifdef _WINDOWS
    " [-idsx]"
#endif
    " [-c <config-file>]";
/* end of application USAGE message */

/* application HELP message */
const char *help_message[] = {
    "Options:",
    "  -c --config <config-file>  Absolute path to the configuration file",
    "",
    "Other options:",
    "  -h --help                  Give this help",
    "  -V --version               Display version number",
#ifdef _WINDOWS
    "",
    "Functions:",
    "  -i --install          Install Job arranger agent as service",
    "  -d --uninstall        Uninstall Job arranger agent from service",
    "  -s --start            Start Job arranger agent service",
    "  -x --stop             Stop Job arranger agent service",
#endif
    NULL
};

/* end of application HELP message */

/* COMMAND LINE OPTIONS */
static struct zbx_option longopts[] = {
    {"config", 1, NULL, 'c'},
    {"help", 0, NULL, 'h'},
    {"version", 0, NULL, 'V'},
#ifdef _WINDOWS
    {"install", 0, NULL, 'i'},
    {"uninstall", 0, NULL, 'd'},
    {"start", 0, NULL, 's'},
    {"stop", 0, NULL, 'x'},
#endif
    {NULL}
};

static char shortopts[] = "c:hV"
#ifdef _WINDOWS
    "idsx"
#endif
    ;
/* end of COMMAND LINE OPTIONS */

int threads_num = 0;
ZBX_THREAD_HANDLE *threads = NULL;
unsigned char daemon_type = ZBX_DAEMON_TYPE_AGENT;

/******************************************************************************
 *                                                                            *
 * Function: help_jobarg                                                      *
 *                                                                            *
 * Purpose: show the help message                                             *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
void help_jobarg()
{
    const char **p = help_message;

    usage();
    printf("\n");

    while (NULL != *p)
        printf("%s\n", *p++);
}

/******************************************************************************
 *                                                                            *
 * Function: version_jobarg                                                   *
 *                                                                            *
 * Purpose: show the version of the application                               *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
void version_jobarg()
{
    printf("%s v%s (revision %s) (%s)\n", title_message, JOBARG_VERSION, JOBARG_REVISION, JOBARG_REVDATE);
    printf("Compilation time: %s %s\n", __DATE__, __TIME__);
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
static void parse_commandline(int argc, char **argv, ZBX_TASK_EX * t)
{
    char ch;
    t->task = ZBX_TASK_START;

    ch = '\0';
    while ((char) EOF != (ch = (char) zbx_getopt_long(argc, argv, shortopts, longopts, NULL))) {
        switch (ch) {
        case 'c':
            CONFIG_FILE = strdup(zbx_optarg);
            break;
        case 'h':
            help_jobarg();
            exit(FAIL);
            break;
        case 'V':
            version_jobarg();
            exit(FAIL);
            break;
#ifdef _WINDOWS
        case 'i':
            t->task = ZBX_TASK_INSTALL_SERVICE;
            break;
        case 'd':
            t->task = ZBX_TASK_UNINSTALL_SERVICE;
            break;
        case 's':
            t->task = ZBX_TASK_START_SERVICE;
            break;
        case 'x':
            t->task = ZBX_TASK_STOP_SERVICE;
            break;
#endif
        default:
            t->task = ZBX_TASK_SHOW_USAGE;
            break;
        }
    }

    if (NULL == CONFIG_FILE)
        CONFIG_FILE = DEFAULT_CONFIG_FILE;
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
static void set_defaults()
{
    char hostname[JA_MAX_STRING_LEN];
    if (NULL == CONFIG_HOSTNAME) {
        if (ja_hostname(hostname) == SUCCEED)
            CONFIG_HOSTNAME = zbx_strdup(CONFIG_HOSTNAME, hostname);
        else
            zabbix_log(LOG_LEVEL_WARNING, "failed to get system hostname");
    }
#ifdef USE_PID_FILE
    if (NULL == CONFIG_PID_FILE)
        CONFIG_PID_FILE = "/tmp/jobarg_agentd.pid";
#endif

    if (NULL == CONFIG_TMPDIR)
        CONFIG_TMPDIR = zbx_strdup(CONFIG_TMPDIR, "/tmp");
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
static void jobarg_load_config(int requirement)
{
    struct cfg_line cfg[] = {
        /* PARAMETER, VAR, TYPE, MANDATORY, MIN, MAX */
#ifdef USE_PID_FILE
        {"JaPidFile", &CONFIG_PID_FILE, TYPE_STRING,
         PARM_OPT, 0, 0},
#endif
        {"TmpDir", &CONFIG_TMPDIR, TYPE_STRING, PARM_MAND, 0, 0},
        {"Server", &CONFIG_HOSTS_ALLOWED, TYPE_STRING, PARM_OPT, 0, 0},
        {"Hostname", &CONFIG_HOSTNAME, TYPE_STRING, PARM_OPT, 0, 0},
        {"AllowRoot", &CONFIG_ALLOW_ROOT, TYPE_INT, PARM_OPT, 0, 1},
        {"ListenIP", &CONFIG_LISTEN_IP, TYPE_STRING, PARM_OPT, 0, 0},
        {"ListenIP", &CONFIG_SOURCE_IP, TYPE_STRING, PARM_OPT, 0, 0},
        {"Timeout", &CONFIG_TIMEOUT, TYPE_INT, PARM_OPT, 1, 30},
        {"DebugLevel", &CONFIG_LOG_LEVEL, TYPE_INT, PARM_OPT, 0, 4},
        {"LogFileSize", &CONFIG_LOG_FILE_SIZE, TYPE_INT, PARM_OPT, 0,
         1024},
        {"JaLogFile", &CONFIG_LOG_FILE, TYPE_STRING, PARM_OPT, 0, 0},
        {"JaServerPort", &CONFIG_SERVER_PORT, TYPE_INT, PARM_OPT, 1024,
         32767},
        {"JaListenPort", &CONFIG_LISTEN_PORT, TYPE_INT, PARM_OPT, 1024,
         32767},
        {"JaSendRetry", &CONFIG_SEND_RETRY, TYPE_INT, PARM_OPT, 0, 3600},
        {"JaDatabaseFile", &CONFIG_DATABASE_FILE, TYPE_STRING, PARM_MAND,
         0, 0},
        {"JaJobHistory", &CONFIG_JOB_HISTORY, TYPE_INT, PARM_OPT, 1, 365},
        {"JaBackupTime", &CONFIG_BACKUP_TIME, TYPE_INT, PARM_OPT, 1, 24},
        {"JaExtjobPath", &CONFIG_EXTJOB_PATH, TYPE_STRING, PARM_MAND, 0,
         0},
        {"JaFcopyTimeout", &CONFIG_FCOPY_TIMEOUT, TYPE_INT, PARM_OPT, 1,
         3600},
        {NULL}
    };

    parse_cfg_file(CONFIG_FILE, cfg, requirement, JA_CFG_STRICT);
    set_defaults();

    if (CONFIG_HOSTNAME == NULL) {
        zbx_error("Hostname must be defined");
        exit(EXIT_FAILURE);
    }
    if (ZBX_CFG_FILE_REQUIRED == requirement
        && NULL == CONFIG_HOSTS_ALLOWED) {
        zbx_error("Server must be defined");
        exit(EXIT_FAILURE);
    }
    if (ZBX_CFG_FILE_REQUIRED == requirement
        && NULL == CONFIG_DATABASE_FILE) {
        zbx_error("Database file must be defined");
        exit(EXIT_FAILURE);
    }
    if (ZBX_CFG_FILE_REQUIRED == requirement && NULL == CONFIG_EXTJOB_PATH) {
        zbx_error("extjob path must be defined");
        exit(EXIT_FAILURE);
    }

    CONFIG_REQUEST_FLAG =
        zbx_dsprintf(NULL, "%s%cjobarg_agentd_request_flag", CONFIG_TMPDIR,
                     JA_DLM);
    CONFIG_CMD_FILE =
        zbx_dsprintf(NULL, "%s%c%s", CONFIG_EXTJOB_PATH, JA_DLM,
                     JA_JOBARG_COMMAND);
    CONFIG_REBOOT_FLAG =
        zbx_dsprintf(NULL, "%s%cjobarg_agentd_reboot_flag", CONFIG_TMPDIR,
                     JA_DLM);
    CONFIG_REBOOT_FILE =
        zbx_dsprintf(NULL, "%s%c%s", CONFIG_EXTJOB_PATH, JA_DLM,
                     JA_JOBARG_REBOOT);
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
#ifdef _WINDOWS
static int jobarg_exec_service_task(const char *name,
                                    const ZBX_TASK_EX * t)
{
    int ret;

    ret = FAIL;
    switch (t->task) {
    case ZBX_TASK_INSTALL_SERVICE:
        ret = ZabbixCreateService(name, 0);
        break;
    case ZBX_TASK_UNINSTALL_SERVICE:
        ret = ZabbixRemoveService();
        break;
    case ZBX_TASK_START_SERVICE:
        ret = ZabbixStartService();
        break;
    case ZBX_TASK_STOP_SERVICE:
        ret = ZabbixStopService();
        break;
    default:
        assert(0);
    }

    return ret;
}
#endif

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
int MAIN_ZABBIX_ENTRY()
{
    zbx_thread_args_t *thread_args;
    zbx_sock_t listen_sock;
    int thread_num = 0;

#ifdef _WINDOWS
    DWORD res;
#else
    int status;
#endif
    if (NULL == CONFIG_LOG_FILE || '\0' == *CONFIG_LOG_FILE) {
        zabbix_open_log(LOG_TYPE_SYSLOG, CONFIG_LOG_LEVEL, NULL);
    } else {
        zabbix_open_log(LOG_TYPE_FILE, CONFIG_LOG_LEVEL, CONFIG_LOG_FILE);
    }

    zabbix_log(LOG_LEVEL_INFORMATION,
               "Starting Job Arranger Agent. Job Arranger %s (revision %s).",
               JOBARG_VERSION, JOBARG_REVISION);

    if (FAIL == ja_jobdb_init()) {
        zabbix_log(LOG_LEVEL_CRIT, "Init database failed: %s",
                   CONFIG_DATABASE_FILE);
        exit(EXIT_FAILURE);
    }

    if (FAIL ==
        zbx_tcp_listen(&listen_sock, CONFIG_LISTEN_IP,
                       (unsigned short) CONFIG_LISTEN_PORT)) {
        zabbix_log(LOG_LEVEL_CRIT, "listener failed: %s",
                   zbx_tcp_strerror());
        exit(EXIT_FAILURE);
    }

    if (listen_sock.num_socks != 1
        || listen_sock.sockets[0] == ZBX_SOCK_ERROR) {
        zabbix_log(LOG_LEVEL_CRIT,
                   "listener failed(job arranger agent): %s",
                   zbx_tcp_strerror());
        exit(EXIT_FAILURE);
    }

    /* --- START THREADS --- */
    threads_num = 2;
    threads =
        (ZBX_THREAD_HANDLE *) zbx_calloc(threads, threads_num,
                                         sizeof(ZBX_THREAD_HANDLE));

    /* start the executive thread */
    thread_args =
        (zbx_thread_args_t *) zbx_malloc(NULL, sizeof(zbx_thread_args_t));
    thread_args->thread_num = thread_num;
    thread_args->args = NULL;
    threads[thread_num++] =
        zbx_thread_start(executive_thread, thread_args);

    /* start the listener thread */
    thread_args =
        (zbx_thread_args_t *) zbx_malloc(NULL, sizeof(zbx_thread_args_t));
    thread_args->thread_num = thread_num;
    thread_args->args = &listen_sock;
    threads[thread_num++] = zbx_thread_start(listener_thread, thread_args);

#ifdef _WINDOWS
    set_parent_signal_handler();
    res =
        WaitForMultipleObjectsEx(threads_num, threads, FALSE, INFINITE,
                                 FALSE);
    if (ZBX_IS_RUNNING()) {
        zabbix_log(LOG_LEVEL_CRIT,
                   "One thread has terminated unexpectedly (code:%lu). Exiting ...",
                   res);
        THIS_SHOULD_NEVER_HAPPEN;
        ZBX_DO_EXIT();
        zbx_sleep(1);
    } else {
        zbx_sleep(2);
        THIS_SHOULD_NEVER_HAPPEN;
    }
#else
    while (-1 == wait(&status)) {
        if (EINTR != errno) {
            zabbix_log(LOG_LEVEL_ERR,
                       "failed to wait on child processes: %s",
                       zbx_strerror(errno));
            break;
        }
    }
    THIS_SHOULD_NEVER_HAPPEN;
#endif
    ja_db_close();
    zbx_on_exit();

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
void zbx_on_exit()
{
    int i;
    zabbix_log(LOG_LEVEL_DEBUG, "zbx_on_exit() called");

    if (NULL != threads) {
#ifndef _WINDOWS
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGCHLD);
        sigprocmask(SIG_BLOCK, &set, NULL);
#endif
        for (i = 0; i < threads_num; i++) {
            if (threads[i]) {
                zbx_thread_kill(threads[i]);
                threads[i] = ZBX_THREAD_HANDLE_NULL;
            }
        }
        zbx_free(threads);
    }
#ifndef _WINDOWS
    zbx_sleep(2);
#endif

    zabbix_log(LOG_LEVEL_INFORMATION,
               "Job Arranger Agent stopped. Job Arranger %s (revision %s).",
               JOBARG_VERSION, JOBARG_REVISION);
    zabbix_close_log();
    exit(SUCCEED);
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
#if defined(HAVE_SIGQUEUE) && defined(ZABBIX_DAEMON)
void zbx_sigusr_handler(zbx_task_t task)
{
    /* nothing to do */
}
#endif

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
int main(int argc, char **argv)
{
    ZBX_TASK_EX t;
#ifdef _WINDOWS
    int ret;
    SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
    memset(&t, 0, sizeof(t));
    t.task = ZBX_TASK_START;

    progname = get_program_name(argv[0]);
    parse_commandline(argc, argv, &t);

    switch (t.task) {
    case ZBX_TASK_SHOW_USAGE:
        usage();
        exit(FAIL);
        break;
#ifdef _WINDOWS
    case ZBX_TASK_INSTALL_SERVICE:
    case ZBX_TASK_UNINSTALL_SERVICE:
    case ZBX_TASK_START_SERVICE:
    case ZBX_TASK_STOP_SERVICE:
        jobarg_load_config(ZBX_CFG_FILE_REQUIRED);
        ret = jobarg_exec_service_task(argv[0], &t);
        exit(ret);
        break;
#endif
    default:
        jobarg_load_config(ZBX_CFG_FILE_REQUIRED);
        break;
    }

    START_MAIN_ZABBIX_ENTRY(CONFIG_ALLOW_ROOT);
    exit(SUCCEED);
}
