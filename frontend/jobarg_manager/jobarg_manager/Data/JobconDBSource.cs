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
using System.Data;
using jp.co.ftf.jobcontroller.Common;

namespace jp.co.ftf.jobcontroller.JobController
{
    public class JobconDBSource
    {
        private string _jobconName;
        private string _dbUser;
        private string _dbPass;
        private string _dbs;
        private string _connnectStr;
        private Consts.DBTYPE _dbType;
        private bool _healthCheckFlag;
        private int _healthCheckInterval;

        public string JobconName
        {
            get { return _jobconName; }
            set { _jobconName = value; }
        }
        public string DBUser
        {
            get { return _dbUser; }
            set { _dbUser = value; }
        }
        public string DBPass
        {
            get { return _dbPass; }
            set { _dbPass = value; }
        }
        public string DBS
        {
            get { return _dbs; }
            set { _dbs = value; }
        }
        public String ConnnectStr
        {
            get { return _connnectStr; }
            set { _connnectStr = value; }
        }

        public Consts.DBTYPE DBType
        {
            get { return _dbType; }
            set { _dbType = value; }
        }

        public bool HealthCheckFlag
        {
            get { return _healthCheckFlag; }
            set { _healthCheckFlag = value; }
        }

        public int HealthCheckInterval
        {
            get { return _healthCheckInterval; }
            set { _healthCheckInterval = value; }
        }

        public JobconDBSource(String jobconName, String dbUser, String dbPass, String dbs, Consts.DBTYPE dbType,
                                bool healthCheckFlag, int healthCheckInterval)
        {
            _jobconName = jobconName;
            _dbUser = dbUser;
            _dbPass = dbPass;
            _dbs = dbs;
            _dbType = dbType;
            _healthCheckFlag = healthCheckFlag;
            _healthCheckInterval = healthCheckInterval;
            _connnectStr = "DSN=" + dbs + ";UID=" + dbUser + ";PWD=" + dbPass + "; Connect Timeout=300"; ;
        }
        public override string ToString()
        {
            return _jobconName;
        }

        public static JobconDBSource Create(DataRow dr)
        {
            String jobconName = (String)dr["JobconName"];
            String dbUser = (String)dr["DBUser"];
            String dbPass = (String)dr["DBPassword"];
            String dbs = (String)dr["DBSource"];
            String strDbtype = (String)dr["DBType"];
            Consts.DBTYPE dbType = (Consts.DBTYPE)Convert.ToInt16(strDbtype);
            bool healthCheckFlag=true;
            if (dr["HealthCheckFlag"]!=null)
            {
                if (((String)dr["HealthCheckFlag"]).Equals("0"))
                    healthCheckFlag = false;
            }
            int healthCheckInterval = 5;
            if (dr["HealthCheckInterval"] != null)
            {
                healthCheckInterval = Convert.ToInt32(dr["HealthCheckInterval"]);
            }
            if (healthCheckFlag && healthCheckInterval == 0)
                healthCheckInterval = 5;

            return new JobconDBSource(jobconName, dbUser, dbPass, dbs, dbType, healthCheckFlag, healthCheckInterval);
        }

    }
}


