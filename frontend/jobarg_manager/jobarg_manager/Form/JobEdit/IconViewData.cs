/*
** Job Arranger for ZABBIX
** Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.
** Copyright (C) 2013 Daiwa Institute of Research Business Innovation Ltd. All Rights Reserved.
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
using System.Windows;
using System.Windows.Controls;
using System.ComponentModel;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2013 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 偉 2013/09/27 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.JobEdit
{
    public class IconViewData : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;


        private void NotifyPropertyChanged(string info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }

        /// <summary>ジョブID</summary>
        private string _jobId="";
        public string JobId
        {
            get { return this._jobId; }
            set
            {
                if (value != this._jobId)
                {
                    this._jobId = value;
                    NotifyPropertyChanged("JobId");
                }
            }
        }

        /// <summary>ジョブ名</summary>
        private string _jobName;
        public string JobName
        {
            get { return this._jobName; }
            set
            {
                if (value != this._jobName)
                {
                    this._jobName = value;
                    NotifyPropertyChanged("JobName");
                }
            }
        }
    }
}