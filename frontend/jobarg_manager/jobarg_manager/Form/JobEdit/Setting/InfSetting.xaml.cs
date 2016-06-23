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
using System.Collections.Generic;
using System.Windows.Controls;
using jp.co.ftf.jobcontroller.DAO;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 旭 2012/11/05 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.JobEdit
{
    /// <summary>
    /// InfSetting.xaml の相互作用ロジック
    /// </summary>
    public partial class InfSetting : Window
    {
        #region フィールド

        /// <summary>DBアクセスインスタンス</summary>
        private DBConnect dbAccess = new DBConnect(LoginSetting.ConnectStr);

        #endregion

        #region コンストラクタ
        public InfSetting(IRoom room, string jobId)
        {
            InitializeComponent();

            _myJob = room;

            _oldJobId = jobId;

            SetValues(jobId);

            if (_myJob.ContentItem.InnerJobId != null)
            {
                ChangeButton4DetailRef();
            }
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
        private string _oldJobId;
        public string OldJobId
        {
            get
            {
                return _oldJobId;
            }
            set
            {
                _oldJobId = value;
            }
        }
        #endregion

        #region イベント

        /// <summary>登録処理</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        private void btnToroku_Click(object sender, RoutedEventArgs e)
        {
            // DB接続をオープン 
            dbAccess.CreateSqlConnect();

            // 入力チェック 
            if (!InputCheck())
            {
                // DB接続をクローズ 
                dbAccess.CloseSqlConnect();

                return;
            }

            // DB接続をクローズ 
            dbAccess.CloseSqlConnect();

            // 入力されたジョブID 
            string newJobId = txtJobId.Text;
            // 入力されたジョブ名 
            string newJobNm = txtJobName.Text;

            // ジョブ管理テーブルの更新 
            DataRow[] rowJobCon = _myJob.Container.JobControlTable.Select("job_id='" + _oldJobId + "'");
            if (rowJobCon != null && rowJobCon.Length > 0)
            {
                rowJobCon[0]["job_id"] = newJobId;
                rowJobCon[0]["job_name"] = newJobNm;
            }

            // 情報取得アイコン設定テーブルの更新 
            DataRow[] rowInfo = _myJob.Container.IconInfoTable.Select("job_id='" + _oldJobId + "'");
            if (rowInfo != null && rowInfo.Length > 0)
            {
                rowInfo[0]["job_id"] = newJobId;
                Int16 infoFlag = Convert.ToInt16(combInfo.SelectedValue);
                rowInfo[0]["info_flag"] = infoFlag;
                if (infoFlag == 0)
                {
                    rowInfo[0]["get_job_id"] = txtJobIdInfo.Text;
                }
                else
                {
                    rowInfo[0]["get_job_id"] = Convert.DBNull;
                }
            }

            // ジョブIDが変更された場合、フロー管理テーブルを更新 
            if (!_oldJobId.Equals(newJobId))
                CommonUtil.UpdateFlowForJobId(_myJob.Container.FlowControlTable, _oldJobId, newJobId);

            // 画面再表示 
            _myJob.Container.JobItems.Remove(_oldJobId);
            _myJob.Container.JobItems.Add(newJobId, _myJob);
            _myJob.JobId = newJobId;
            _myJob.JobName = newJobNm;
            _myJob.Container.SetedJobIds[_myJob.JobId] = "1";
            this.Close();
        }

        /// <summary>情報種別を選択</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        private void combMethod_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (Convert.ToInt16(combInfo.SelectedValue) == 0)
            {
                txtJobIdInfo.IsEnabled = true;
            }
            else
            {
                txtJobIdInfo.IsEnabled = false;
            }
        }

        /// <summary>キャンセルをクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        //*******************************************************************
        /// <summary>画面を閉める</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        //*******************************************************************
        private void Window_Closed(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (_myJob.ItemEditType == Consts.EditType.READ && _myJob.Container.ParantWindow is JobEdit)
                _myJob.ResetInitColor();
        }
        #endregion

        #region publicメッソド
        public void SetDisable()
        {
            txtJobId.IsEnabled = false;
            txtJobName.IsEnabled = false;
            btnToroku.IsEnabled = false;
            txtJobIdInfo.IsEnabled = false;
            combInfo.IsEnabled = false;

        }
        #endregion

        #region privateメッソド

        /// <summary> 値のセットと表示処理</summary>
        /// <param name="sender">源</param>
        private void SetValues(string jobId)
        {
            // ジョブ管理テーブルのデータを取得 
            DataRow[] rowJob = _myJob.Container.JobControlTable.Select("job_id='" + jobId + "'");
            if (rowJob != null && rowJob.Length > 0)
            {
                txtJobId.Text = jobId;
                txtJobName.Text = Convert.ToString(rowJob[0]["job_name"]);
            }

            // 情報種別 
            Dictionary<int, string> dic = new Dictionary<int, string>();
            dic.Add(0, Properties.Resources.job_status_text);
            combInfo.Items.Clear();
            combInfo.ItemsSource = dic;
            combInfo.DisplayMemberPath = "Value";
            combInfo.SelectedValuePath = "Key";

            // 情報取得アイコン設定テーブルのデータを取得 
            DataRow[] rowInfo;
            if (_myJob.ContentItem.InnerJobId == null)
            {
                rowInfo = _myJob.Container.IconInfoTable.Select("job_id='" + jobId + "'");
            }
            else
            {
                rowInfo = _myJob.Container.IconInfoTable.Select("inner_job_id=" + _myJob.ContentItem.InnerJobId);
            }
            if (rowInfo != null && rowInfo.Length > 0)
            {
                if (!Convert.IsDBNull(rowInfo[0]["info_flag"]))
                {
                    Int16 infoFlag = Convert.ToInt16(rowInfo[0]["info_flag"]);
                    combInfo.SelectedValue = infoFlag;
                    if (infoFlag == 0)
                    {
                        txtJobIdInfo.Text = Convert.ToString(rowInfo[0]["get_job_id"]);
                    }
                    else
                    {
                        txtJobIdInfo.IsEnabled = false;
                    }
                }
                else
                {
                    txtJobIdInfo.IsEnabled = false;
                }
            }
        }

        /// <summary> 各項目のチェック処理</summary>
        private bool InputCheck()
        {
            // ジョブID 
            string jobIdForChange = Properties.Resources.err_message_job_id;
            String jobId = txtJobId.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(jobId))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001,
                    new string[] { jobIdForChange });
                return false;
            }
            // 桁数チェック 
            if (CheckUtil.IsLenOver(jobId, 32))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_003,
                    new string[] { jobIdForChange, "32" });
                return false;
            }
            // 半角英数字とハイフン（-）チェック 
            if (!CheckUtil.IsHankakuStrAndHyphen(jobId))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_005,
                    new string[] { jobIdForChange });
                return false;
            }
            // 予約語（START）チェック 
            if (CheckUtil.IsHoldStrSTART(jobId))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_001);
                return false;
            }
            // すでに登録済みの場合 
            DataRow[] rowJob = _myJob.Container.JobControlTable.Select("job_id='" + jobId + "'");
            if (rowJob != null && rowJob.Length > 0)
            {
                foreach (DataRow row in rowJob)
                {
                    if (!jobId.Equals(_oldJobId) && jobId.Equals(row["job_id"]))
                    {
                        CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_004,
                            new string[] { jobIdForChange });
                        return false;
                    }
                }
            }

            // ジョブ名 
            string jobNameForChange = Properties.Resources.err_message_job_name;
            String jobName = txtJobName.Text;
            // バイト数チェック 
            if (CheckUtil.IsLenOver(jobName, 64))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_003,
                    new string[] { jobNameForChange, "64" });
                return false;
            }

            // ジョブ情報 
            if (Convert.ToInt16(combInfo.SelectedValue) == 0)
            {
                string jobInfoForChange = Properties.Resources.err_message_job_info;
                string jobInfo = txtJobIdInfo.Text;
                // 未入力の場合 
                if (CheckUtil.IsNullOrEmpty(jobInfo))
                {
                    CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001,
                        new string[] { jobInfoForChange });
                    return false;
                }
                // バイト数チェック 
                if (CheckUtil.IsLenOver(jobInfo, 1024))
                {
                    CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_003,
                        new string[] { jobInfoForChange, "1024" });
                    return false;
                }
                // 半角英数字とハイフン、スラッシュチェック 
                if (!CheckUtil.IsHankakuStrAndHyphenAndSlash(jobInfo))
                {
                    CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_010,
                        new string[] { jobInfoForChange });
                    return false;
                }
                // ジョブネット階層がない場合 
                if (!jobInfo.Contains("/"))
                {
                    DataRow[] rowJobInfo = _myJob.Container.JobControlTable.Select("job_id='" + jobInfo + "'");
                    if (rowJobInfo == null || rowJobInfo.Length == 0)
                    {
                        CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_003);
                        return false;
                    }
                }
                else
                {
                    // ジョブネット階層がある場合 
                    string[] jobInfojobId = jobInfo.Split(new char[] { '/' });
                    // 呼出元ジョブネットと呼出先ジョブネットの関係を確認 
                    DataRow[] rowIconJobnet = _myJob.Container.IconJobnetTable.Select("job_id='" +
                        jobInfojobId[0] + "'");
                    if (rowIconJobnet == null || rowIconJobnet.Length == 0)
                    {
                        CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_003);
                        return false;
                    }
                    // リンク先ジョブネットID 
                    string linkJobnetId = Convert.ToString(rowIconJobnet[0]["link_jobnet_id"]);

                    JobnetControlDAO _jobnetControlDAO = new JobnetControlDAO(dbAccess);
                    IconJobnetDAO _iconJobnetDAO = new IconJobnetDAO(dbAccess);
                    JobControlDAO _jobControlDAO = new JobControlDAO(dbAccess);
                    // ジョブネットID 
                    string jobnetId = "";
                    // 更新日 
                    string updateDate = "";
                    // 呼出元ジョブネットと呼出先ジョブネットの関係を確認 
                    for (int k = 1; k < jobInfojobId.Length ; k++)
                    {
                        DataTable tbJobnetCon = _jobnetControlDAO.GetInfoForJobInfo(linkJobnetId);

                        if (tbJobnetCon == null || tbJobnetCon.Rows.Count == 0)
                        {
                            CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_003);
                            return false;
                        }
                        else
                        {
                            jobnetId = Convert.ToString(tbJobnetCon.Rows[0]["jobnet_id"]);
                            updateDate = Convert.ToString(tbJobnetCon.Rows[0]["update_date"]);
                            DataTable tbJobControl = _jobControlDAO.GetEntityByPk(jobnetId, jobInfojobId[k], updateDate);
                            if (tbJobControl == null || tbJobControl.Rows.Count == 0)
                            {
                                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_003);
                                return false;
                            }
                            else
                            {
                                DataTable tbIconJobnet = _iconJobnetDAO.GetEntityByPk(jobnetId, jobInfojobId[k], updateDate);
                                if (tbIconJobnet != null && tbIconJobnet.Rows.Count > 0)
                                {
                                    linkJobnetId = Convert.ToString(tbIconJobnet.Rows[0]["link_jobnet_id"]);
                                }
                            }
                        }
                    }
                }
            }

            return true;
        }

        /// <summary> 詳細画面からの参照時のボタンの切り替え</summary>
        private void ChangeButton4DetailRef()
        {
            GridSetting.Children.Remove(btnToroku);
            System.Windows.Controls.Button button = new System.Windows.Controls.Button();
            btnCancel.Content = Properties.Resources.close_button_text;
            btnCancel.IsDefault = true;
        }
        #endregion
    }
}
