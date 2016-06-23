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
** $Date:: 2013-05-14 09:55:36 +0900 #$
** $Revision: 4619 $
** $Author: ossinfra@FITECHLABS.CO.JP $
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
int ja_file_create(const char *filename, const int size)
{
    FILE *fp;
    const char *__function_name = "ja_file_create";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filename: %s, size: %d",
               __function_name, filename, size);

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        zabbix_log(LOG_LEVEL_ERR, "Can not open the file: %s (%s)",
                   filename, strerror(errno));
        return FAIL;
    }
    if (fseek(fp, size, SEEK_SET) != 0) {
        zabbix_log(LOG_LEVEL_ERR, "Can not seek the file: %s (%s)",
                   filename, strerror(errno));
        fclose(fp);
        return FAIL;
    }
    if (fputc(0, fp) < 0) {
        zabbix_log(LOG_LEVEL_ERR, "Can not fputc the file: %s (%s)",
                   filename, strerror(errno));
        fclose(fp);
        return FAIL;
    }
    fclose(fp);
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
int ja_file_remove(const char *filename)
{
    int ret;
    const char *__function_name = "ja_file_remove";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filename: %s", __function_name,
               filename);
    ret = SUCCEED;
    if (remove(filename) != 0) {
        zabbix_log(LOG_LEVEL_ERR, "Can not remove the file: %s (%s)",
                   filename, strerror(errno));
        ret = FAIL;
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
long ja_file_getsize(const char *filename)
{
    long size;
    FILE *fp;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);

    return size;
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
int ja_file_load(const char *filename, const int size, void *data)
{
    int ret, err, fsize;
    FILE *fp;
    const char *__function_name = "ja_file_load";

    zabbix_log(LOG_LEVEL_DEBUG, "In %s() filename: %s, size: %d",
               __function_name, filename, size);
    ret = SUCCEED;
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        zabbix_log(LOG_LEVEL_ERR,
                   "Can not open the file: %s (%s)", filename,
                   strerror(errno));
        return FAIL;
    }

    err = 1;
    if (size > 0) {
        if (fread(data, size, 1, fp) != 1)
            err = 0;;
    } else {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        if (fsize >= JA_MAX_DATA_LEN)
            fsize = JA_MAX_DATA_LEN - 1;
        fseek(fp, 0, SEEK_SET);
        if (fread(data, sizeof(char), fsize, fp) != fsize)
            err = 0;
    }
    if (err != 1) {
        zabbix_log(LOG_LEVEL_ERR,
                   "Can not read result file: %s (%s)", filename,
                   strerror(errno));
        ret = FAIL;
    }
    fclose(fp);
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
#ifdef _WINDOWS
#else
int ja_file_is_regular(const char *path)
{
    struct stat st;

    if (0 == lstat(path, &st) && 0 != S_ISREG(st.st_mode))
        return SUCCEED;

    return FAIL;
}
#endif
