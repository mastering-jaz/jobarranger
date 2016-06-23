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
** $Date:: 2013-06-10 11:04:56 +0900 #$
** $Revision: 4879 $
** $Author: ossinfra@FITECHLABS.CO.JP $
**/

#include <json/json.h>

#include "common.h"
#include "comms.h"
#include "cfg.h"
#include "log.h"
#include "zbxgetopt.h"

#include "jacommon.h"
#include "jastr.h"
#include "jatcp.h"
#include "jatelegram.h"

/* long options */
static struct zbx_option longopts[] = {
    {"jobarg-server", 1, NULL, 'z'},
    {"port", 1, NULL, 'p'},
    {"user-name", 1, NULL, 'U'},
    {"password", 1, NULL, 'P'},
    {"job-id", 1, NULL, 'j'},
    {"time", 0, NULL, 't'},
    {"management-id", 0, NULL, 'i'},
    {"help", 0, NULL, 'h'},
    {"version", 0, NULL, 'V'},
    {NULL}
};

/* short options */
static char shortopts[] = "z:p:U:P:j:t:r:hV";

/* end of COMMAND LINE OPTIONS */

char *CONFIG_HOSTNAME = NULL;
int CONFIG_LOG_LEVEL = LOG_LEVEL_WARNING;
char *CONFIG_SERVER = NULL;
unsigned short CONFIG_SERVER_PORT = JA_DEFAULT_SERVER_PORT;
char *CONFIG_JOBID = NULL;
char *CONFIG_USERNAME = NULL;
char *CONFIG_PASSWORD = NULL;
char *CONFIG_STARTTIME = NULL;
char *CONFIG_MID = NULL;

const char *progname = NULL;
const char title_message[] = "Job Arranger release";
const char usage_message[] =
    "[-hV] -z <hostname or IP> [-p <port>] -U <username> -P <password> -j <jobid> [-t [<YYYYMMDD>|<YYYYMMDDHHMM>] ] [-r <management ID>]";

const char *help_message[] = {
    "Options:",
    "  -z --jobarranger-server <server>                 Hostname or IP address of Job Arranger server",
    "  -p --port <server port>                          Specify port number of server trapper running on the server. Default is 10061",
    "  -U --user-name <user-name>                       Specify user name",
    "  -P --password <password>                         Specify password",
    "  -j --job-id <jobid>                              Specify jobid",
    "  -t --start-time [<YYYYMMDD>|<YYYYMMDDHHMM>]      Specify jobnet start time",
    "  -r --management-id                               Specify jobnet managemnet id",
    "",
    "Other options:",
    "  -h --help                                        Give this help",
    "  -V --version                                     Display version number",
    NULL
};

/* COMMAND LINE OPTIONS */

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
#if !defined(_WINDOWS)
static void send_signal_handler(int sig)
{
    if (SIGALRM == sig)
        zabbix_log(LOG_LEVEL_WARNING, "Timeout while executing operation");

    exit(FAIL);
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
static void ja_help()
{
    const char **p = help_message;
    usage();
    printf("\n");
    while (NULL != *p)
        printf("%s\n", *p++);
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
static void parse_commandline(int argc, char **argv)
{
    char ch = '\0';

    /* parse the command-line */
    while ((char) EOF !=
           (ch =
            (char) zbx_getopt_long(argc, argv, shortopts, longopts,
                                   NULL))) {
        switch (ch) {
        case 'z':
            CONFIG_SERVER = zbx_strdup(CONFIG_SERVER, zbx_optarg);
            break;
        case 'p':
            CONFIG_SERVER_PORT = (unsigned short) atoi(zbx_optarg);
            break;
        case 'U':
            CONFIG_USERNAME = zbx_strdup(CONFIG_USERNAME, zbx_optarg);
            break;
        case 'P':
            CONFIG_PASSWORD = zbx_strdup(CONFIG_PASSWORD, zbx_optarg);
            break;
        case 'j':
            CONFIG_JOBID = zbx_strdup(CONFIG_JOBID, zbx_optarg);
            break;
        case 't':
            CONFIG_STARTTIME = zbx_strdup(CONFIG_STARTTIME, zbx_optarg);
            break;
        case 'r':
            CONFIG_MID = zbx_strdup(CONFIG_MID, zbx_optarg);
            break;
        case 'h':
            ja_help();
            exit(FAIL);
            break;
        case 'V':
            version();
            exit(FAIL);
            break;
        default:
            ja_help();
            exit(FAIL);
            break;
        }
    }

    if (CONFIG_SERVER == NULL)
        CONFIG_SERVER = zbx_strdup(CONFIG_SERVER, "localhost");

    if (CONFIG_USERNAME == NULL || CONFIG_PASSWORD == NULL
        || CONFIG_JOBID == NULL) {
        usage();
        exit(FAIL);
    }
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
static int ja_check_response(char *data)
{
    int ret;
    json_object *jp_res, *jp_data, *jp;
    int result;
    char *err;

    ret = FAIL;
    err = NULL;
    jp_res = json_tokener_parse(data);
    if (is_error(jp_res)) {
        err =
            zbx_dsprintf(NULL,
                         "The response data is not a json data. %s", data);
        jp_res = NULL;
        goto error;
    }

    jp_data = json_object_object_get(jp_res, JA_PROTO_TAG_DATA);
    if (jp_data == NULL) {
        err =
            zbx_dsprintf(NULL,
                         "can not find the tag '%s' from the response",
                         JA_PROTO_TAG_DATA);
        goto error;
    }

    jp = json_object_object_get(jp_data, JA_PROTO_TAG_RESULT);
    if (jp_data == NULL) {
        err =
            zbx_dsprintf(NULL,
                         "can not find the tag '%s' from the response",
                         JA_PROTO_TAG_DATA);
        goto error;
    }
    result = json_object_get_int(jp);
    if (result == FAIL) {
        jp = json_object_object_get(jp_data, JA_PROTO_TAG_MESSAGE);
        if (jp_data == NULL) {
            err =
                zbx_dsprintf(NULL,
                             "can not find the tag '%s' from the response",
                             JA_PROTO_TAG_MESSAGE);
            goto error;
        }
        zabbix_log(LOG_LEVEL_ERR, "response message: %s",
                   (char *) json_object_get_string(jp));
    } else {
        jp = json_object_object_get(jp_data, "inner_jobnet_id");
        zabbix_log(LOG_LEVEL_INFORMATION, "inner_jobnet_id: %s",
                   (char *) json_object_to_json_string(jp));
        ret = SUCCEED;
    }
  error:
    if (ret == FAIL) {
        if (err != NULL)
            zabbix_log(LOG_LEVEL_ERR, "response error: %s", err);
    }
    if (err != NULL)
        zbx_free(err);
    if (jp_res != NULL)
        json_object_put(jp_res);

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
int main(int argc, char **argv)
{
    int ret;
    ja_telegram_object obj;
    json_object *jp;
    zbx_sock_t sock;
    char *data;

    ret = FAIL;
    progname = get_program_name(argv[0]);
    parse_commandline(argc, argv);

    /*output to stderr */
    zabbix_open_log(LOG_TYPE_UNDEFINED, CONFIG_LOG_LEVEL, NULL);
    if (ja_telegram_new(&obj) == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "Can not create ja_telegram_object");
        goto exit;
    }

    if (MIN_ZABBIX_PORT > CONFIG_SERVER_PORT) {
        zabbix_log(LOG_LEVEL_ERR,
                   "Incorrect port number [%d]. Allowed [%d:%d]",
                   (int) CONFIG_SERVER_PORT, (int) MIN_ZABBIX_PORT,
                   (int) MAX_ZABBIX_PORT);
        goto exit;
    }
#if !defined(_WINDOWS)
    signal(SIGINT, send_signal_handler);
    signal(SIGTERM, send_signal_handler);
    signal(SIGQUIT, send_signal_handler);
    signal(SIGALRM, send_signal_handler);
#endif

    json_object_object_add(obj.request, JA_PROTO_TAG_KIND,
                           json_object_new_string("job.release"));
    json_object_object_add(obj.request, JA_PROTO_TAG_VERSION,
                           json_object_new_int(JA_PROTO_TELE_VERSION));
    jp = json_object_object_get(obj.request, JA_PROTO_TAG_DATA);
    json_object_object_add(jp, JA_PROTO_TAG_USERNAME,
                           json_object_new_string(CONFIG_USERNAME));
    json_object_object_add(jp, JA_PROTO_TAG_PASSWORD,
                           json_object_new_string(CONFIG_PASSWORD));
    json_object_object_add(jp, "jobid",
                           json_object_new_string(CONFIG_JOBID));
    if (CONFIG_STARTTIME != NULL)
        json_object_object_add(jp, "start_time",
                               json_object_new_string(CONFIG_STARTTIME));
    if (CONFIG_MID != NULL)
        json_object_object_add(jp, "inner_jobnet_id",
                               json_object_new_string(CONFIG_MID));

    zabbix_log(LOG_LEVEL_DEBUG, "send data: %s",
               json_object_to_json_string(obj.request));
    ret =
        zbx_tcp_connect(&sock, NULL, CONFIG_SERVER, CONFIG_SERVER_PORT,
                        GET_SENDER_TIMEOUT);
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "connect error: %s", zbx_tcp_strerror());
        goto exit;
    }

    ret = ja_tcp_send(&sock, 0, obj.request);
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "send data error: %s",
                   json_object_to_json_string(obj.request));
        zbx_tcp_close(&sock);
        goto exit;
    }

    ret = zbx_tcp_recv(&sock, &data);
    if (ret == FAIL) {
        zabbix_log(LOG_LEVEL_ERR, "recive data error: %s", data);
        zbx_tcp_close(&sock);
        goto exit;
    }
    ret = ja_check_response(data);
    zbx_tcp_close(&sock);

  exit:
    ja_telegram_clear(&obj);
    zabbix_close_log();
    return ret;
}