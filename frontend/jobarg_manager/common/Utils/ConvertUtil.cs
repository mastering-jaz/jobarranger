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
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
**/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace jp.co.ftf.jobcontroller.Common
{
    /// <summary>
    /// 共通変換ユーティリティクラス
    /// </summary>
    public class ConvertUtil
    {
        public static decimal ConverDate2IntYYYYMMDDHHMISS(DateTime date)
        {

            String str = date.ToString("yyyyMMddHHmmss");

            return Convert.ToDecimal(str);
        }

        public static DateTime ConverIntYYYYMMDDHHMISS2Date(decimal iDate)
        {
            String str = iDate.ToString();

            return DateTime.ParseExact(str, "yyyyMMddHHmmss", null);
        }

        public static decimal ConverDate2IntYYYYMMDD(DateTime date)
        {
            String str = date.ToString("yyyyMMdd");
            return Convert.ToDecimal(str);
        }

        public static DateTime ConverIntYYYYMMDD2Date(decimal iDate)
        {
            String str = iDate.ToString();

            return DateTime.ParseExact(str, "yyyyMMdd", null);
        }
        public static decimal ConverDate2IntYYYYMMDDHHMI(DateTime date)
        {
            String str = date.ToString("yyyyMMddHHmm");
            return Convert.ToDecimal(str);
        }
        public static DateTime ConverIntYYYYMMDDHHMI2Date(decimal iDate)
        {
            String str = iDate.ToString();

            return DateTime.ParseExact(str, "yyyyMMddHHmm", null);
        }
        public static decimal getCalendarFromDate(int year, int up)
        {
            return Convert.ToDecimal((year + up).ToString() + "0101");
        }

        public static decimal getCalendarToDate(int year, int up)
        {
            return Convert.ToDecimal((year + up).ToString() + "1231");
        }
        public static String convertDisplayFlag(int flag)
        {
            String displayFlag = "";
            if (flag == 1)
            {
                displayFlag = "○";
            }
            return displayFlag;
        }
    }
}

