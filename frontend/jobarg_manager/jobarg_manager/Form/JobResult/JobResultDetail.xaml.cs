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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Data;
using System.Collections;
using System.IO;
using jp.co.ftf.jobcontroller.Common;
using jp.co.ftf.jobcontroller.DAO;
using jp.co.ftf.jobcontroller;

namespace jp.co.ftf.jobcontroller.JobController.Form.JobResult
{
    /// <summary>
    /// JobResultDetail.xaml の相互作用ロジック
    /// </summary>
    public partial class JobResultDetail : BaseWindow
    {

        #region コンストラクタ
        public JobResultDetail(DataRow row)
        {
            InitializeComponent();
            this.DataContext = row;         
        }
        #endregion

        #region プロパティ

        /// <summary>クラス名</summary>
        public override string ClassName
        {
            get
            {
                return "JobResultDetail";
            }
        }

        /// <summary>画面ID</summary>
        public override string GamenId
        {
            get
            {
                return Consts.WINDOW_520;
            }
        }
        #endregion

        #region イベント
        /// <summary>閉じるをクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        private void btnClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        #endregion

    }
}
