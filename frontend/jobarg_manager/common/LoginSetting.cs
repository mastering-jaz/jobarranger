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
    public static class LoginSetting
    {
        public static string _jobconName;
        public static string JobconName
        {
            get
            {
                return _jobconName;
            }
            set
            {
                _jobconName = value;
            }
        }
        public static string _connectStr;
        public static string ConnectStr
        {
            get
            {
                return _connectStr;
            }
            set
            {
                _connectStr = value;
            }
        }

        public static string _userName;
        public static string UserName
        {
            get
            {
                return _userName;
            }
            set
            {
                _userName = value;
            }
        }

        public static decimal _userID;
        public static decimal UserID
        {
            get
            {
                return _userID;
            }
            set
            {
                _userID = value;
            }
        }

        public static Consts.AuthorityEnum _authority;
        public static Consts.AuthorityEnum Authority
        {
            get
            {
                return _authority;
            }
            set
            {
                _authority = value;
            }
        }

        public static Consts.ActionMode _mode;
        public static Consts.ActionMode Mode
        {
            get
            {
                return _mode;
            }
            set
            {
                _mode = value;
            }
        }

        public static List<Decimal> _groupList;
        public static List<Decimal> GroupList
        {
            get
            {
                return _groupList;
            }
            set
            {
                _groupList = value;
            }
        }
        public static Consts.DBTYPE DBType {get; set;}

        public static String Lang { get; set; }

        public static bool HealthCheckFlag { get; set; }

        public static int HealthCheckInterval { get; set; }


    }

}
