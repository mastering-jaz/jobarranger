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
using System.Windows.Media;


namespace jp.co.ftf.jobcontroller.JobController
{
    public class JobnetExecInfo
    {
        public decimal inner_jobnet_id { get; set; }
        public String jobnet_id { get; set; }
        public String jobnet_name { get; set; }
        public int status { get; set; }
        public String display_status { get; set; }
        public int run_type { get; set; }
        public String job_id { get; set; }
        public String job_name { get; set; }
        public SolidColorBrush status_color { get; set; }
        public String scheduled_time { get; set; }
        public String start_time { get; set; }
        public String end_time { get; set; }

        //added by YAMA 2014/04/25
        public int load_status { get; set; }

        //added by YAMA 2014/07/01
        public String Foreground_color { get; set; }    
    }
}
