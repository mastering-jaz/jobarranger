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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using jp.co.ftf.jobcontroller.DAO;
using jp.co.ftf.jobcontroller.Common;
//using jp.co.ftf.jobcontroller.JobController.Form.JobEdit;
using System.Data;


namespace jp.co.ftf.jobcontroller.JobController.Form.SetParameter
{
    /// <summary>
    /// SetParameterPage.xaml の相互作用ロジック
    /// </summary>
    public partial class SetParameterPage : BaseUserControl
    {
        #region フィールド

        /// <summary>DBアクセスインスタンス</summary>
        //private DBConnect dbAccess = new DBConnect(LoginSetting.ConnectStr);
        private DBConnect dbAccess;


        /// <summary> パラメータ設定テーブル </summary>
        private ParameterTableDAO _parameterTableDAO;

        private String jobnetViewSpan = "";
        private String jobnetLoadSpan = "";
        private String jobnetKeepSpan = "";
        private String joblogKeepSpan = "";

        private String JobArrangerStandardTime = "";

        private String zabbixNotice = "";
        private String zabbixServerIPaddress = "";
        private String zabbixServerPortNumber = "";
        private String zabbixSenderCommand = "";
        private String messageDestinationServer = "";
        private String messageDestinationItemKey = "";
        private String retry = "";
        private String retryCount = "";
        private String retryInterval = "";

        private String sv_jobnetViewSpan = "";
        private String sv_jobnetLoadSpan = "";
        private String sv_jobnetKeepSpan = "";
        private String sv_joblogKeepSpan = "";

        private String sv_JobArrangerStandardTime = "";

        private String sv_zabbixNotice = "";
        private String sv_zabbixServerIPaddress = "";
        private String sv_zabbixServerPortNumber = "";
        private String sv_zabbixSenderCommand = "";
        private String sv_messageDestinationServer = "";
        private String sv_messageDestinationItemKey = "";
        private String sv_retry = "";
        private String sv_retryCount = "";
        private String sv_retryInterval = "";


        #endregion


        #region コンストラクタ
        public SetParameterPage(JobArrangerWindow parent)
        {

            bool viewer = false;
            #if VIEWER
                viewer = true;
            #endif


            //Parent = parent;

            InitializeComponent();

            // 設定値取得・表示
            SetValues();


            // Zabbixユーザー（一般ユーザー）、参照モードの場合は参照のみ
            if (LoginSetting.Authority == Consts.AuthorityEnum.GENERAL || viewer)
            {
                // 画面コントロールを非活性化
                SetItemIsEnabled(false, true);
            }
            else
            {
                SetItemIsEnabled(true, false);
            }

            _parent = parent;
            DataContext = this;
        }
        #endregion

        #region プロパティ
        /// <summary>ジョブネット一覧</summary>
        public ObservableCollection<JobnetExecInfo> JobnetExecList { get; set; }

        private JobArrangerWindow _parent;
        public JobArrangerWindow Parent
        {
            get
            {
                return _parent;
            }
        }

        /// <summary>クラス名</summary>
        public override string ClassName
        {
            get
            {
                return "SetParameterPage";
            }
        }

        /// <summary>画面ID</summary>
        public override string GamenId
        {
            get
            {
                return Consts.WINDOW_600;
            }
        }



        #endregion

        #region イベント

        #endregion

        #region privateメソッド

        /// <summary> 値のセットと表示処理</summary>
        /// <param name="sender">源</param>
        private void SetValues()
        {
            // 基準時刻コンボボックス作成
            DataTable DtCombStdTime = new DataTable();

            DtCombStdTime.Columns.Add("ID", typeof(string));
            DtCombStdTime.Columns.Add("NAME", typeof(string));

            DataRow RowCombStdTime = DtCombStdTime.NewRow();
            RowCombStdTime["ID"] = "0";
            RowCombStdTime["NAME"] = jp.co.ftf.jobcontroller.JobController.Properties.Resources.settings_Job_Arranger_std_time0;
            DtCombStdTime.Rows.Add(RowCombStdTime);
            RowCombStdTime = DtCombStdTime.NewRow();
            RowCombStdTime["ID"] = "1";
            RowCombStdTime["NAME"] = jp.co.ftf.jobcontroller.JobController.Properties.Resources.settings_Job_Arranger_std_time1;
            DtCombStdTime.Rows.Add(RowCombStdTime);

            combStandardTime.Items.Clear();
            combStandardTime.ItemsSource = DtCombStdTime.DefaultView;
            combStandardTime.DisplayMemberPath = "NAME";
            combStandardTime.SelectedValuePath = "ID";


            // Zabbix通知コンボボックス作成
            DataTable DtCombNotice = new DataTable();

            DtCombNotice.Columns.Add("ID", typeof(string));
            DtCombNotice.Columns.Add("NAME", typeof(string));

            DataRow RowCombNotice = DtCombNotice.NewRow();
            RowCombNotice["ID"] = "0";
            RowCombNotice["NAME"] = jp.co.ftf.jobcontroller.JobController.Properties.Resources.settings_zbxsnd_notice0;
            DtCombNotice.Rows.Add(RowCombNotice);
            RowCombNotice = DtCombNotice.NewRow();
            RowCombNotice["ID"] = "1";
            RowCombNotice["NAME"] = jp.co.ftf.jobcontroller.JobController.Properties.Resources.settings_zbxsnd_notice1;
            DtCombNotice.Rows.Add(RowCombNotice);

            combNotice.Items.Clear();
            combNotice.ItemsSource = DtCombNotice.DefaultView;
            combNotice.DisplayMemberPath = "NAME";
            combNotice.SelectedValuePath = "ID";


            // Zabbix通知再送コンボボックス作成
            DataTable DtCombRetry = new DataTable();

            DtCombRetry.Columns.Add("ID", typeof(string));
            DtCombRetry.Columns.Add("NAME", typeof(string));

            DataRow RowCombRetry = DtCombRetry.NewRow();
            RowCombRetry["ID"] = "0";
            RowCombRetry["NAME"] = jp.co.ftf.jobcontroller.JobController.Properties.Resources.settings_zbxsnd_retry0;
            DtCombRetry.Rows.Add(RowCombRetry);
            RowCombRetry = DtCombRetry.NewRow();
            RowCombRetry["ID"] = "1";
            RowCombRetry["NAME"] = jp.co.ftf.jobcontroller.JobController.Properties.Resources.settings_zbxsnd_retry1;
            DtCombRetry.Rows.Add(RowCombRetry);

            combRetry.Items.Clear();
            combRetry.ItemsSource = DtCombRetry.DefaultView;
            combRetry.DisplayMemberPath = "NAME";
            combRetry.SelectedValuePath = "ID";


            // DBデータ取得
            GetParamData();
            
            // 画面項目設定
            SetParamData();

            // 修正前データ保存
            svParamData();


        }

        private void GetParamData()
        {
            jobnetViewSpan = DBUtil.GetParameterVelue("JOBNET_VIEW_SPAN");
            jobnetLoadSpan = DBUtil.GetParameterVelue("JOBNET_LOAD_SPAN");
            jobnetKeepSpan = DBUtil.GetParameterVelue("JOBNET_KEEP_SPAN");
            joblogKeepSpan = DBUtil.GetParameterVelue("JOBLOG_KEEP_SPAN");

            JobArrangerStandardTime = DBUtil.GetParameterVelue("MANAGER_TIME_SYNC");

            zabbixNotice = DBUtil.GetParameterVelueForStrData("ZBXSND_ON");
            zabbixServerIPaddress = DBUtil.GetParameterVelueForStrData("ZBXSND_ZABBIX_IP");
            zabbixServerPortNumber = DBUtil.GetParameterVelue("ZBXSND_ZABBIX_PORT");
            zabbixSenderCommand = DBUtil.GetParameterVelueForStrData("ZBXSND_SENDER");
            messageDestinationServer = DBUtil.GetParameterVelueForStrData("ZBXSND_ZABBIX_HOST");
            messageDestinationItemKey = DBUtil.GetParameterVelueForStrData("ZBXSND_ITEM_KEY");
            retry = DBUtil.GetParameterVelue("ZBXSND_RETRY");
            retryCount = DBUtil.GetParameterVelue("ZBXSND_RETRY_COUNT");
            retryInterval = DBUtil.GetParameterVelue("ZBXSND_RETRY_INTERVAL");

        }



        private void SetParamData()
        {

            tbxJobnetViewSpan.Text = jobnetViewSpan;
            tbxJobnetLoadSpan.Text = jobnetLoadSpan;
            tbxJobnetKeepSpan.Text = jobnetKeepSpan;
            tbxJoblogKeepSpan.Text = joblogKeepSpan;

            tbxZabbixServerIPaddress.Text = zabbixServerIPaddress;
            tbxZabbixServerPortNumber.Text = zabbixServerPortNumber;
            tbxZabbixSenderCommand.Text = zabbixSenderCommand;
            tbxMessageDestinationServer.Text = messageDestinationServer;
            tbxMessageDestinationItemKey.Text = messageDestinationItemKey;

            tbxRetryCount.Text = retryCount;
            tbxRetryInterval.Text = retryInterval;


            // 基準時刻コンボボックスに項目を設定
            combStandardTime.SelectedValue = JobArrangerStandardTime;

            // Zabbix通知コンボボックスに項目を設定
            combNotice.SelectedValue = zabbixNotice;

            // Zabbix通知再送コンボボックスに項目を設定
            combRetry.SelectedValue = retry;

        }

        private void svParamData()
        {
            sv_jobnetViewSpan = tbxJobnetViewSpan.Text;
            sv_jobnetLoadSpan = tbxJobnetLoadSpan.Text;
            sv_jobnetKeepSpan = tbxJobnetKeepSpan.Text;
            sv_joblogKeepSpan = tbxJoblogKeepSpan.Text;
            sv_JobArrangerStandardTime = combStandardTime.SelectedValue.ToString();

            sv_zabbixNotice = combNotice.SelectedValue.ToString();
            sv_zabbixServerIPaddress = tbxZabbixServerIPaddress.Text;
            sv_zabbixServerPortNumber = tbxZabbixServerPortNumber.Text;
            sv_zabbixSenderCommand = tbxZabbixSenderCommand.Text;
            sv_messageDestinationServer = tbxMessageDestinationServer.Text;
            sv_messageDestinationItemKey = tbxMessageDestinationItemKey.Text;
            sv_retry = combRetry.SelectedValue.ToString();
            sv_retryCount = tbxRetryCount.Text;
            sv_retryInterval = tbxRetryInterval.Text;

        }

        private bool IsChengedParamData()
        {

            if (tbxJobnetViewSpan.Text != sv_jobnetViewSpan)
                return true;

            if (tbxJobnetLoadSpan.Text != sv_jobnetLoadSpan)
                return true;

            if (tbxJobnetKeepSpan.Text != sv_jobnetKeepSpan)
                return true;

            if (tbxJoblogKeepSpan.Text != sv_joblogKeepSpan)
                return true;

            if (tbxZabbixServerIPaddress.Text != sv_zabbixServerIPaddress)
                return true;

            if (tbxZabbixServerPortNumber.Text != sv_zabbixServerPortNumber)
                return true;

            if (tbxZabbixSenderCommand.Text != sv_zabbixSenderCommand)
                return true;

            if (tbxMessageDestinationServer.Text != sv_messageDestinationServer)
                return true;

            if (tbxMessageDestinationItemKey.Text != sv_messageDestinationItemKey)
                return true;

            if (tbxRetryCount.Text != sv_retryCount)
                return true;

            if (tbxRetryInterval.Text != sv_retryInterval)
                return true;

            if (combStandardTime.SelectedValue.ToString() != sv_JobArrangerStandardTime)
                return true;

            if (combNotice.SelectedValue.ToString() != sv_zabbixNotice)
                return true;

            if (combRetry.SelectedValue.ToString() != sv_retry)
                return true;

            return false;

        }

        /// <summary> 各項目のチェック処理(登録)</summary>
        private bool InputCheck()
        {
            Int64 chkData = 0;

            // システム設定項目
            // ジョブネット運行情報表示期間（分）
            string jobnetViewSpanForChange = Properties.Resources.err_message_settings_jobnet_view_span;
            String jobnetViewSpan = tbxJobnetViewSpan.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(jobnetViewSpan))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { jobnetViewSpanForChange });
                return false;
            }
            // 半角数字のみ可
            if (!CheckUtil.IsHankakuNum(jobnetViewSpan))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, new string[] { jobnetViewSpanForChange });
                return false;
            }
            // 1～1059127200（）
            chkData = Convert.ToInt64(jobnetViewSpan);
            if (chkData < 1 || chkData > 1059127200)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_008,
                    new string[] { jobnetViewSpanForChange, "「1～1059127200」" });
                return false;
            }

            // 予定ジョブネット事前展開開始時間（分）
            string jobnetLoadSpanForChange = Properties.Resources.err_message_settings_jobnet_load_span;
            String jobnetLoadSpan = tbxJobnetLoadSpan.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(jobnetLoadSpan))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { jobnetLoadSpanForChange });
                return false;
            }
            // 半角数字のみ可
            if (!CheckUtil.IsHankakuNum(jobnetLoadSpan))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, new string[] { jobnetLoadSpanForChange });
                return false;
            }
            // 1～2147483647（32ビット最大値）
            chkData = Convert.ToInt64(jobnetLoadSpan);
            if (chkData < 1 || chkData > 2147483647)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_008,
                    new string[] { jobnetLoadSpanForChange, "「1～2147483647」" });
                return false;
            }

            // 終了済みジョブネット情報保持期間（分）
            string jobnetKeepSpanForChange = Properties.Resources.err_message_settings_jobnet_keep_span;
            String jobnetKeepSpan = tbxJobnetKeepSpan.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(jobnetKeepSpan))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { jobnetKeepSpanForChange });
                return false;
            }
            // 半角数字のみ可
            if (!CheckUtil.IsHankakuNum(jobnetKeepSpan))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, new string[] { jobnetKeepSpanForChange });
                return false;
            }
            // 1～2147483647（32ビット最大値）
            chkData = Convert.ToInt64(jobnetKeepSpan);
            if (chkData < 1 || chkData > 2147483647)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_008,
                    new string[] { jobnetKeepSpanForChange, "「1～2147483647」" });
                return false;
            }

            // ジョブ実行結果ログ保持期間（分）
            string joblogKeepSpanForChange = Properties.Resources.err_message_settings_joblog_keep_span;
            String joblogKeepSpan = tbxJoblogKeepSpan.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(joblogKeepSpan))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { joblogKeepSpanForChange });
                return false;
            }
            // 半角数字のみ可
            if (!CheckUtil.IsHankakuNum(joblogKeepSpan))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, new string[] { joblogKeepSpanForChange });
                return false;
            }
            // 1～2147483647（32ビット最大値）
            chkData = Convert.ToInt64(joblogKeepSpan);
            if (chkData < 1 || chkData > 2147483647)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_008,
                    new string[] { joblogKeepSpanForChange, "「1～2147483647」" });
                return false;
            }

            // Zabbix通知設定項目
            // Zabbix IPアドレス 
            string zabbixServerIPaddressForChange = Properties.Resources.err_message_settings_zbxsnd_zabbix_ip;
            String zabbixServerIPaddress = tbxZabbixServerIPaddress.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(zabbixServerIPaddress))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { zabbixServerIPaddressForChange });
                return false;
            }
            // 最大2048バイト 
            if (CheckUtil.IsLenOver(zabbixServerIPaddress, 2048))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_003, new string[] { zabbixServerIPaddressForChange, "2048" });
                return false;
            }
            //  半角英数字とアンダーバー、ハイフン、ピリオドのみ可
            if (!CheckUtil.IsHankakuStrAndSpaceAndUnderbarAndHyphenAndPeriod(zabbixServerIPaddress))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_SETTING_002, new string[] { zabbixServerIPaddressForChange });
                return false;
            }
            // ハイフン、ピリオドは、先頭、最後の文字としては使用不可
            String firstChr = zabbixServerIPaddress.Substring(0, 1);
            String lastChr = zabbixServerIPaddress.Substring(zabbixServerIPaddress.Length - 1, 1);
            if ((firstChr == "-" || lastChr == "-") || (firstChr == "." || lastChr == "."))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_SETTING_002, new string[] { zabbixServerIPaddressForChange });
                return false;
            }

            // 数字のみの入力は不可
            if (CheckUtil.IsHankakuNum(zabbixServerIPaddress))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_SETTING_002, new string[] { zabbixServerIPaddressForChange });
                return false;
            }



            // Zabbix ポート番号
            string zabbixServerPortNumberForChange = Properties.Resources.err_message_settings_zbxsnd_zabbix_port;
            String zabbixServerPortNumber = tbxZabbixServerPortNumber.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(zabbixServerPortNumber))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { zabbixServerPortNumberForChange });
                return false;
            }
            // 半角数字のみ可
            if (!CheckUtil.IsHankakuNum(zabbixServerPortNumber))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, new string[] { zabbixServerPortNumberForChange });
                return false;
            }
            // 0～65535
            Int32 PortNumber = Convert.ToInt32(zabbixServerPortNumber);
            if (PortNumber < 0 || PortNumber > 65535)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_017, new string[] { zabbixServerPortNumberForChange, "0", "65535" });
                return false;
            }

            // Zabbix Sender コマンド
            string zabbixSenderCommandForChange = Properties.Resources.err_message_settings_zbxsnd_sender;
            String zabbixSenderCommand = tbxZabbixSenderCommand.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(zabbixSenderCommand))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { zabbixSenderCommandForChange });
                return false;
            }
            //  ASCII文字のみ可
            if (!CheckUtil.IsASCIIStr(zabbixSenderCommand))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, new string[] { zabbixSenderCommandForChange });
                return false;
            }
            // 最大2048バイト 
            if (CheckUtil.IsLenOver(zabbixSenderCommand, 2048))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_003, new string[] { zabbixSenderCommandForChange, "2048" });
                return false;
            }

            // メッセージ通知先Zabbixホスト
            string messageDestinationServerForChange = Properties.Resources.err_message_settings_zbxsnd_zabbix_host;
            String messageDestinationServer = tbxMessageDestinationServer.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(messageDestinationServer))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { messageDestinationServerForChange });
                return false;
            }
            //  半角英数字と半角空白、アンダーバー、ハイフン、ピリオドのみ可
            if (!CheckUtil.IsHankakuStrAndSpaceAndUnderbarAndHyphenAndPeriod(messageDestinationServer))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_028, new string[] { messageDestinationServerForChange });
                return false;
            }
            // 最大64バイト 
            if (CheckUtil.IsLenOver(messageDestinationServer, 64))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_003, new string[] { messageDestinationServerForChange, "64" });
                return false;
            }

            // アイテムキー
            string messageDestinationItemKeyForChange = Properties.Resources.err_message_settings_zbxsnd_zabbix_item;
            String messageDestinationItemKey = tbxMessageDestinationItemKey.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(messageDestinationItemKey))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { messageDestinationItemKeyForChange });
                return false;
            }
            //  半角英数字、アンダーバー、ハイフンのみ可
            if (!CheckUtil.IsHankakuStrAndHyphenAndUnderbar(messageDestinationItemKey))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_013, new string[] { messageDestinationItemKeyForChange });
                return false;
            }
            // 最大255バイト 
            if (CheckUtil.IsLenOver(messageDestinationItemKey, 255))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_003, new string[] { messageDestinationItemKeyForChange, "255" });
                return false;
            }

            // 再送回数
            string retryCountForChange = Properties.Resources.err_message_settings_zbxsnd_retry_count;
            String retryCount = tbxRetryCount.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(retryCount))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { retryCountForChange });
                return false;
            }
            // 半角数字のみ可
            if (!CheckUtil.IsHankakuNum(retryCount))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, new string[] { retryCountForChange });
                return false;
            }
            // 0～2147483647
            chkData = Convert.ToInt64(retryCount);
            if (chkData < 0 || chkData > 2147483647)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_008,
                    new string[] { retryCountForChange, "「0～2147483647」" });
                return false;
            }

            // 再送インターバル（秒）
            string retryIntervalForChange = Properties.Resources.err_message_settings_zbxsnd_retry_interval;
            String retryInterval = tbxRetryInterval.Text;
            // 未入力の場合 
            if (CheckUtil.IsNullOrEmpty(retryInterval))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, new string[] { retryIntervalForChange });
                return false;
            }
            // 半角数字のみ可
            if (!CheckUtil.IsHankakuNum(retryInterval))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, new string[] { retryIntervalForChange });
                return false;
            }
            // 1～2147483647
            chkData = Convert.ToInt64(retryInterval);
            if (chkData < 1 || chkData > 2147483647)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_008,
                    new string[] { retryIntervalForChange, "「1～2147483647」" });
                return false;
            }

            return true;

        }


        /// <summary>変更処理</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        private void btnUpdate_Click(object sender, RoutedEventArgs e)
        {
            // 入力チェック 
            if (!InputCheck())
            {
                return;
            }

            // 編集登録確認ダイアログの表示 
            if (MessageBoxResult.Yes == CommonDialog.ShowEditRegistDialog())
            {
                dbAccess = new DBConnect(LoginSetting.ConnectStr);
                _parameterTableDAO = new ParameterTableDAO(dbAccess);

                // パラメータテーブルをロック
                bool getLockFlag = GetLockForUpd(Consts.WINDOW_600);

                //200秒実行を停止します。
                //System.Threading.Thread.Sleep(200000); 
               
                //bool getLockFlag = true;

                 if (getLockFlag == true)
                {
                     RegistProcess();

                     // 修正前データ保存
                     svParamData();

                }
                dbAccess.CloseSqlConnect();
            }
        }

        //*******************************************************************
        /// <summary>ＤＢデータ登録</summary>
        //*******************************************************************
        private void RegistProcess()
        {
            //dbAccess.CreateSqlConnect();
            dbAccess.BeginTransaction();

            // 登録 
            RegistDataTable();

            dbAccess.TransactionCommit();
            //dbAccess.CloseSqlConnect();

        }


        private void RegistDataTable()
        {

            _parameterTableDAO.SetJaParameterTable("JOBNET_VIEW_SPAN", tbxJobnetViewSpan.Text);
            _parameterTableDAO.SetJaParameterTable("JOBNET_LOAD_SPAN", tbxJobnetLoadSpan.Text);
            _parameterTableDAO.SetJaParameterTable("JOBNET_KEEP_SPAN", tbxJobnetKeepSpan.Text);
            _parameterTableDAO.SetJaParameterTable("JOBLOG_KEEP_SPAN", tbxJoblogKeepSpan.Text);

            _parameterTableDAO.SetJaParameterTable("MANAGER_TIME_SYNC", combStandardTime.SelectedValue.ToString());

            _parameterTableDAO.SetJaParameterTable("ZBXSND_ON", combNotice.SelectedValue.ToString());
            _parameterTableDAO.SetJaParameterTable("ZBXSND_ZABBIX_IP", tbxZabbixServerIPaddress.Text);
            _parameterTableDAO.SetJaParameterTable("ZBXSND_ZABBIX_PORT", tbxZabbixServerPortNumber.Text);
            _parameterTableDAO.SetJaParameterTable("ZBXSND_SENDER", tbxZabbixSenderCommand.Text);
            _parameterTableDAO.SetJaParameterTable("ZBXSND_ZABBIX_HOST", tbxMessageDestinationServer.Text);
            _parameterTableDAO.SetJaParameterTable("ZBXSND_ITEM_KEY", tbxMessageDestinationItemKey.Text);
            _parameterTableDAO.SetJaParameterTable("ZBXSND_RETRY", combRetry.SelectedValue.ToString());

            _parameterTableDAO.SetJaParameterTable("ZBXSND_RETRY_COUNT", tbxRetryCount.Text);
            _parameterTableDAO.SetJaParameterTable("ZBXSND_RETRY_INTERVAL", tbxRetryInterval.Text);


        }



        /// <summary>再読込処理</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        private void btnReread_Click(object sender, RoutedEventArgs e)
        {
            // DBデータ取得
            GetParamData();

            // 項目編集時
            if (IsChengedParamData() == true)
            {
                if (MessageBoxResult.Yes == CommonDialog.ShowCancelDialog())
                {
                    SetParamData();
                    // 修正前データ保存
                    svParamData();
                }
            }
            else
            {
                SetParamData();
                // 修正前データ保存
                svParamData();
            }
        }

        //*******************************************************************
        /// <summary> DBのロック取得、存在チェック</summary>
        //*******************************************************************
        private bool GetLockForUpd(string windowId)
        {
            dbAccess.CreateSqlConnect();
            dbAccess.BeginTransaction();
            try
            {
                GetLock(windowId);
            }
            catch (DBException ex)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_SETTING_001);
                return false;
            }
            return true;

        }


        //*******************************************************************
        /// <summary> パラメータテーブルロック</summary>
        //*******************************************************************
        private void GetLock(string windowId)
        {
            _parameterTableDAO.GetLock(windowId, LoginSetting.DBType);

        }

        //*******************************************************************
        /// <summary> 画面項目活性制御</summary>
        //*******************************************************************
        private void SetItemIsEnabled(bool setStat, bool generalMode)
        {
            tbxJobnetViewSpan.IsEnabled = setStat;
            tbxJobnetLoadSpan.IsEnabled = setStat;
            tbxJobnetKeepSpan.IsEnabled = setStat;
            tbxJoblogKeepSpan.IsEnabled = setStat;

            combStandardTime.IsEnabled = setStat;

            combNotice.IsEnabled = setStat;
            tbxZabbixServerIPaddress.IsEnabled = setStat;
            tbxZabbixServerPortNumber.IsEnabled = setStat;
            tbxZabbixSenderCommand.IsEnabled = setStat;
            tbxMessageDestinationServer.IsEnabled = setStat;
            tbxMessageDestinationItemKey.IsEnabled = setStat;
            combRetry.IsEnabled = setStat;
            tbxRetryCount.IsEnabled = setStat;
            tbxRetryInterval.IsEnabled = setStat;

            btnReread.IsEnabled = setStat;
            btnUpdate.IsEnabled = setStat;

            if (generalMode)
            {
                btnReread.IsEnabled = true;
            }

        }


        #endregion
    }
}
