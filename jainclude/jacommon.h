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
** $Date:: 2013-07-08 11:13:41 +0900 #$
** $Revision: 5070 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#ifndef JOBARG_JACOMMON_H
#define JOBARG_JACOMMON_H

#ifndef _WINDOWS
#include "jajoblog.h"
#include "jalog.h"
#include "jaself.h"
#include "jaindex.h"
#endif

/* version or revsion */
#define JOBARG_VERSION          "1.3.0"
#define JOBARG_REVISION         "5164"
#define JOBARG_REVDATE		"2013-07-10"

/* treat unknown parameters as error */
#define JA_CFG_STRICT			0

/* config default value */
#define JA_DEFAULT_AGENT_PORT		10055
#define JA_DEFAULT_SERVER_PORT		10061

/* extjob pid */
#define JA_EXTJOB_PID			"JOBARRANGER_EXTJOB_PID"

/* data size */
#define JA_MAX_STRING_LEN		1024
#define JA_MAX_DATA_LEN			4096
#define JA_KIND_LEN			16
#define JA_SERVERID_LEN			18

/* extjob command */
#define JA_CMD_TIME			"jacmdtime"

/* ja_run_jobnet_table */
/* run type */
#define	JA_JOBNET_RUN_TYPE_NORMAL	0
#define	JA_JOBNET_RUN_TYPE_IMMEDIATE	1
#define	JA_JOBNET_RUN_TYPE_WAIT		2
#define	JA_JOBNET_RUN_TYPE_TEST		3
#define	JA_JOBNET_RUN_TYPE_SCHEDULED	4
#define	JA_JOBNET_RUN_TYPE_JOBALONE	5

/* main flag */
#define	JA_JOBNET_MAIN_FLAG_MAIN	0
#define	JA_JOBNET_MAIN_FLAG_SUB		1

/* jobnet status */
#define	JA_JOBNET_STATUS_BEGIN		0
#define	JA_JOBNET_STATUS_READY		1
#define	JA_JOBNET_STATUS_RUN		2
#define	JA_JOBNET_STATUS_END		3
#define	JA_JOBNET_STATUS_RUNERR		4
#define	JA_JOBNET_STATUS_ENDERR		5
#define	JA_JOBNET_STATUS_ABORT		6


/* ja_run_jobnet_summary_table */
/* jobnet job status */
#define	JA_SUMMARY_JOB_STATUS_NORMAL	0
#define	JA_SUMMARY_JOB_STATUS_TIMEOUT	1
#define	JA_SUMMARY_JOB_STATUS_ERROR	2

/* load status */
#define	JA_SUMMARY_LOAD_STATUS_NORMAL	0
#define	JA_SUMMARY_LOAD_STATUS_ERROR	1


/* ja_run_job_table */
/* job type */
#define	JA_JOB_TYPE_START		0
#define	JA_JOB_TYPE_END			1
#define	JA_JOB_TYPE_IF			2
#define	JA_JOB_TYPE_VALUE		3
#define	JA_JOB_TYPE_JOB			4
#define	JA_JOB_TYPE_JOBNET		5
#define	JA_JOB_TYPE_M			6
#define	JA_JOB_TYPE_W			7
#define	JA_JOB_TYPE_L			8
#define	JA_JOB_TYPE_EXTJOB		9
#define	JA_JOB_TYPE_CALC		10
#define	JA_JOB_TYPE_TASK		11
#define	JA_JOB_TYPE_INFO		12
#define	JA_JOB_TYPE_IFEND		13
#define	JA_JOB_TYPE_FCOPY		14
#define	JA_JOB_TYPE_FWAIT		15
#define	JA_JOB_TYPE_REBOOT		16
#define	JA_JOB_TYPE_REL			17
#define	JA_JOB_TYPE_ABORT		90

/* job status */
#define	JA_JOB_STATUS_BEGIN		0
#define	JA_JOB_STATUS_READY		1
#define	JA_JOB_STATUS_RUN		2
#define	JA_JOB_STATUS_END		3
#define	JA_JOB_STATUS_RUNERR		4
#define	JA_JOB_STATUS_ENDERR		5
#define	JA_JOB_STATUS_ABORT		6

/* job method */
#define	JA_JOB_METHOD_NORMAL		0
#define	JA_JOB_METHOD_WAIT		1
#define	JA_JOB_METHOD_SKIP		2
#define	JA_JOB_METHOD_ABORT		3
#define	JA_JOB_METHOD_RERUN		4

/* test flag */
#define	JA_JOB_TEST_FLAG_OFF		0
#define	JA_JOB_TEST_FLAG_ON		1

/* invo flag */
#define	JA_JOB_INVO_FLAG_OFF		0
#define	JA_JOB_INVO_FLAG_ON		1

/*job command type*/
#define JA_JOB_TYPE_COMMAND		0
#define JA_JOB_TYPE_SCRIPT		1

/* ja_run_flow_table */
/* flow type */
#define	JA_FLOW_TYPE_NORMAL		0
#define	JA_FLOW_TYPE_TRUE		1
#define	JA_FLOW_TYPE_FALSE		2

/*job agent info*/
#define JA_AGENT_PORT			"{$JOBARRANGER_AGENT_PORT}"


/* telegram message */
#define JA_PROTO_TAG_JOBNETID		"jobnetid"
#define JA_PROTO_TAG_JOBNETNAME		"jobnetname"
#define JA_PROTO_TAG_JOBNETRUNTYPE	"jobnetruntype"
#define JA_PROTO_TAG_JOBNETSTATUS	"jobentstatus"
#define JA_PROTO_TAG_JOBSTATUS		"jobstatus"
#define JA_PROTO_TAG_USERNAME		"username"
#define JA_PROTO_TAG_PASSWORD		"password"
#define JA_PROTO_TAG_JOBNETID		"jobnetid"
#define JA_PROTO_TAG_SCHEDULEDTIME	"scheduled_time"
#define JA_PROTO_TAG_STARTTIME		"start_time"
#define JA_PROTO_TAG_ENDTIME		"end_time"
#define JA_PROTO_TAG_REGISTRYNUMBER	"registrynumber"

/* message item value */
#define JA_PROTO_VALUE_JOBNETRUN	"jobnetrun"
#define JA_PROTO_VALUE_JOBNETRUN_RES	"jobnetrun-res"
#define JA_PROTO_VALUE_JOBNETSTATUSRQ		"jobnetstatusrq"
#define JA_PROTO_VALUE_JOBNETSTATUSRQ_RES	"jobnetstatusrq-res"

/* telegram head */
#define JA_PROTO_TAG_KIND		"kind"
#define JA_PROTO_TAG_VERSION		"version"
#define JA_PROTO_TAG_SERVERID		"serverid"
#define JA_PROTO_TAG_DATA		"data"
#define JA_PROTO_TELE_VERSION		JA_PROTO_VALUE_VERSION_1
#define JA_PROTO_VALUE_VERSION_1	1

/* jobrun telegram */
#define JA_PROTO_VALUE_JOBRUN		"jobrun"
#define JA_PROTO_TAG_JOBID		"jobid"
#define JA_PROTO_TAG_METHOD		"method"
#define JA_PROTO_TAG_TYPE		"type"
#define JA_PROTO_TAG_ARGUMENT		"argument"
#define JA_PROTO_TAG_ENV		"env"
#define JA_PROTO_TAG_SCRIPT		"script"
#define JA_PROTO_VALUE_SCRIPT		"script"
#define JA_PROTO_VALUE_COMMAND		"command"

/* jobrun response */
#define JA_PROTO_VALUE_JOBRUN_RES	"jobrun-res"
#define JA_RESPONSE_SUCCEED		0
#define JA_RESPONSE_VERSION_FAIL	1
#define JA_RESPONSE_FAIL		2
#define JA_RESPONSE_ALREADY_RUN		3

/* job result */
#define JA_PROTO_VALUE_JOBRESULT	"jobresult"
#define JA_PROTO_TAG_HOSTNAME		"hostname"
#define JA_PROTO_TAG_RESULT		"result"
#define JA_PROTO_TAG_MESSAGE		"message"
#define JA_PROTO_TAG_MYVERSION		"myversion"
#define JA_PROTO_TAG_JOBSTDOUT		"std_out"
#define JA_PROTO_TAG_JOBSTDERR		"std_err"
#define JA_PROTO_TAG_RET		"return_code"
#define JA_PROTO_TAG_SIGNAL		"signal"
#define JA_JOBRESULT_SUCCEED		0
#define JA_JOBRESULT_FAIL		1

/* job result response */
#define JA_PROTO_VALUE_JOBRESULT_RES	"jobresult-res"
#define JA_JOBRESULT_RES_SUCCEED	0
#define JA_JOBRESULT_RES_FAIL		1

/* job file wait */
#define JA_PROTO_VALUE_FWAIT		"fwait"

/* job file copy */
#define JA_PROTO_VALUE_FCOPY		"fcopy"
#define JA_PROTO_VALUE_FCOPY_RES	"fcopy-res"
#define JA_PROTO_VALUE_GETFILE		"getfile"
#define JA_PROTO_VALUE_PUTFILE		"putfile"
#define JA_PROTO_TAG_FROMDIR		"fromdir"
#define JA_PROTO_TAG_FILENAME		"filename"
#define JA_PROTO_TAG_TODIR		"todir"
#define JA_PROTO_TAG_OVERWRITE		"overwrite"
#define JA_FCOPY_FLAG_SUCCEED		'0'
#define JA_FCOPY_FLAG_FAIL		'1'
#define JA_FCOPY_FLAG_NOFILE		'2'
#define JA_FCOPY_FLAG_KEEP		'3'

/* job agent reboot */
#define JA_PROTO_VALUE_REBOOT		"reboot"
#define JA_PROTO_TAG_REBOOT_MODE	"reboot_mode_flag"
#define JA_PROTO_TAG_REBOOT_WAIT_TIME	"reboot_wait_time"

/* job agent extjob */
#define JA_PROTO_VALUE_EXTJOB           "extjob"

/* job arranger agent */
#ifdef _WINDOWS
    #include <TlHelp32.h>
    #define JA_DLM '\\'
    #define JA_EXE "bat"
    #define JA_PID DWORD
    #define JA_JOBARG_COMMAND "jobarg_command.exe"
    #define JA_JOBARG_REBOOT  "jareboot.bat"
    #define SIGNALNO 137
#else
    #define JA_DLM '/'
    #define JA_EXE "sh"
    #define JA_PID pid_t
    #define JA_JOBARG_COMMAND "jobarg_command"
    #define JA_JOBARG_REBOOT  "jareboot.sh"
    #define O_BINARY 0
#endif

/* job arranger agent method */
#define JA_AGENT_METHOD_NORMAL		0
#define JA_AGENT_METHOD_TEST		1
#define JA_AGENT_METHOD_ABORT		2
#define JA_AGENT_METHOD_KILL		3

/* job arranger agent status */
#define JA_AGENT_STATUS_INIT		-1
#define JA_AGENT_STATUS_BEGIN		0
#define JA_AGENT_STATUS_RUN		1
#define JA_AGENT_STATUS_END		2
#define JA_AGENT_STATUS_CLOSE		3

typedef struct {
    char		kind[JA_KIND_LEN];
    int			version;
    zbx_uint64_t	jobid;
    char		serverid[JA_SERVERID_LEN];
    char		hostname[128];
    int			method;
    char		type[JA_MAX_STRING_LEN];
    char		argument[JA_MAX_DATA_LEN];
    char		script[JA_MAX_DATA_LEN];
    char		env[JA_MAX_DATA_LEN];
    int			result;
    int			status;
    JA_PID		pid;
    zbx_uint64_t	start_time;
    zbx_uint64_t	end_time;
    char		message[JA_MAX_STRING_LEN];
    char		std_out[JA_MAX_DATA_LEN];
    char		std_err[JA_MAX_DATA_LEN];
    int			return_code;
    int			signal;
    int			send_retry;
} ja_job_object;

#endif  /* JOBARG_JACOMMON_H */