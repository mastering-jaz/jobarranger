#!/bin/bash

# Job Arranger Zabbix error transmission Shell script  - 2013/07/01 -

# Parameters
# $1 : User name
# $2 : Jobnet ID
# $3 : Current time (YYYY/MM/DD HH:MM:SS)
# $4 : Message ID
# $5 : Message type (0:Information 1:Critical error 2:Error 3:Warning)
# $6 : Message body
# $7 : Host name
# $8 : Job ID

# Please specify the parameter information of Zabbix Sender.
# Specify the host name of the default HOST variable.

ZABBIX_SERVER=""
ZABBIX_PORT="10051"
ZABBIX_SENDER="zabbix_sender"
KEY=""
HOST=""


if [ $# -lt 8 ]; then
    exit 1
fi

if [ -n "$7" ]; then
    HOST_NAME=$7
else
    HOST_NAME=$HOST
fi

case "$5" in
    "0" ) MSG_TYPE="INFO"  ;;
    "1" ) MSG_TYPE="CRIT"  ;;
    "2" ) MSG_TYPE="ERROR" ;;
    "3" ) MSG_TYPE="WARN"  ;;
     *  ) exit 1 ;;
esac

$ZABBIX_SENDER -z $ZABBIX_SERVER -p $ZABBIX_PORT -s "$HOST_NAME" -k $KEY -o "[$3] [$MSG_TYPE] [$4] $6 (USER NAME=$1 JOBNET=$2 JOB=$8)" >/dev/null 2>&1
