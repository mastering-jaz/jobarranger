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
** $Date:: 2014-05-12 10:24:25 +0900 #$
** $Revision: 5954 $
** $Author: nagata@FITECHLABS.CO.JP $
**/

#include "common.h"
#include "log.h"

#include "jacommon.h"
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
int ja_hostname(char *hostname)
{
#ifdef _WINDOWS
    DWORD dwSize = 256;
    TCHAR computerName[256];

    if (0 == GetComputerName(computerName, &dwSize)) {
        return FAIL;
    }
    zbx_snprintf(hostname, JA_MAX_STRING_LEN, "%s", zbx_unicode_to_utf8(computerName));

    return SUCCEED;
#else
    FILE *fp;
    char value[JA_MAX_STRING_LEN];

    memset(value, '\0', sizeof(value));
    if ((fp = popen("hostname", "r")) == NULL) {
        return FAIL;
    }

    if (fread(value, sizeof(char), (JA_MAX_STRING_LEN - 1), fp) == 0) {
        pclose(fp);
        return FAIL;
    }
    zbx_rtrim(value, " \n");
    pclose(fp);

    zbx_snprintf(hostname, JA_MAX_STRING_LEN, "%s", value);
    return SUCCEED;
#endif
}
