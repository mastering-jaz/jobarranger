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
using System.Windows;
using System.Data;
using System;
using jp.co.ftf.jobcontroller.Common;
using System.Windows.Input;
using System.Windows.Controls;
using jp.co.ftf.jobcontroller.DAO;
using jp.co.ftf.jobcontroller.JobController.Form.JobEdit;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author KIM 2012/12/14 新規作成<BR>                            *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.JobManager
{
    /// <summary>
    /// EnvSetting.xaml の相互作用ロジック
    /// </summary>
    public partial class ParameterView : Window
    {
        #region フィールド
        /// <summary>DBアクセスインスタンス</summary>
        private DBConnect _dbAccess = new DBConnect(LoginSetting.ConnectStr);

        #endregion

        #region コンストラクタ
        public ParameterView(IRoom room)
        {
            InitializeComponent();
            dgVariableBefore.RowHeight = double.NaN;
            dgVariableAfter.RowHeight = double.NaN;
            _dbAccess.CreateSqlConnect();

            _myJob = room;
            _jobId = room.ContentItem.InnerJobId;
            FillTables();

            SetValues();

        }
        #endregion

        #region プロパティ
        /// <summary>ジョブ</summary>
        private IRoom _myJob;
        public IRoom MyJob
        {
            get
            {
                return _myJob;
            }
            set
            {
                _myJob = value;
            }
        }

        /// <summary>ジョブID</summary>
        private string _jobId;
        public string JobId
        {
            get
            {
                return _jobId;
            }
            set
            {
                _jobId = value;
            }
        }


        /// <summary>変更前変数テーブル</summary>
        private DataTable _beforeValueTable;
        public DataTable BeforeValueTable
        {
            get
            {
                return _beforeValueTable;
            }
            set
            {
                _beforeValueTable = value;
            }
        }

        /// <summary>変更後変数テーブル</summary>
        private DataTable _afterValueTable;
        public DataTable AfterValueTable
        {
            get
            {
                return _afterValueTable;
            }
            set
            {
                _afterValueTable = value;
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

        #region protected　override メッソド
        protected override void OnPreviewKeyDown(KeyEventArgs e)
        {
            if (((e.Key.Equals(Key.Enter)) || (e.Key.Equals(Key.Return))))
            {
                this.Close();
                e.Handled = true;
                return;
            }

            base.OnPreviewKeyDown(e);
        }
        #endregion

        #region privateメッソド
        //*******************************************************************
        /// <summary> ジョブネットデータの検索（編集、コピー新規用）</summary>
        //*******************************************************************
        private void FillTables()
        {
            string strSqlJobBefore = "(select 'ja_run_value_before_table' as table_name, BV.inner_jobnet_id, BV.value_name, BV.before_value as value from ja_run_value_before_table BV " +
                                "where BV.inner_job_id = " + _jobId  + " " +
                                "and BV.value_name not in " +
                                "(select JV1.value_name from ja_run_value_job_table JV1 " +
                                "where JV1.inner_job_id = " + _jobId + ")) " +
                                "union " +
                                "(select 'ja_run_value_job_table' as table_name, JV2.inner_jobnet_id, JV2.value_name, JV2.value from ja_run_value_job_table JV2 " +
                                "where JV2.inner_job_id = " + _jobId  +")";
            string strSqlNotJobBefore = "select inner_jobnet_id,value_name,before_value as value from ja_run_value_before_table where inner_job_id = " + _jobId;
            string strSqlBefore = strSqlNotJobBefore;
            if(_myJob.ElementType == ElementType.JOB)
                strSqlBefore = strSqlJobBefore;

            string strSqlAfter = "select inner_jobnet_id,value_name,after_value as value from ja_run_value_after_table where inner_job_id = " + _jobId;

            _beforeValueTable = _dbAccess.ExecuteQuery(strSqlBefore);
            _afterValueTable = _dbAccess.ExecuteQuery(strSqlAfter);

        }

        /// <summary> 値のセットと表示処理</summary>
        /// <param name="sender">源</param>
        private void SetValues()
        {
            lblJobId.Content = _myJob.ContentItem.JobId;
            lblJobName.Content = _myJob.ContentItem.JobName;
            dgVariableBefore.ItemsSource = _beforeValueTable.DefaultView;
            dgVariableAfter.ItemsSource = _afterValueTable.DefaultView;

        }

        #endregion

        private void dgVariableBefore_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }
    }
}
