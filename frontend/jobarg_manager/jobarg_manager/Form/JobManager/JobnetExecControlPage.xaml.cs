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
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Data;
using System.Collections.ObjectModel;
using jp.co.ftf.jobcontroller.DAO;
using jp.co.ftf.jobcontroller.Common;
using System.Windows.Threading;
using jp.co.ftf.jobcontroller.JobController.Form.JobEdit;

namespace jp.co.ftf.jobcontroller.JobController.Form.JobManager
{
    /// <summary>
    /// JobnetExecControlPage.xaml の相互作用ロジック
    /// </summary>
    public partial class JobnetExecControlPage : BaseUserControl
    {
        #region フィールド

        private List<decimal> hideJobnetInnerIdList;
        private RunJobnetSummaryDAO runJobnetSummaryDAO;
        public int viewCount = 3;
        public int oldviewCount = -1;
        public bool viewJobnet = true;
        public bool viewErrJobnet = true;
        public bool viewRunningJobnet = true;
        private JobnetExecControlAllPage allPage;
        private JobnetExecControlErrPage errPage;
        private JobnetExecControlRunningPage runningPage;
        private int jobnetLoadSpan = 0;
        public DispatcherTimer dispatcherTimer;


        #endregion

        #region コンストラクタ
        public JobnetExecControlPage(JobArrangerWindow parent)
        {
            InitializeComponent();
            allPage = new JobnetExecControlAllPage(this);
            allPage.JobnetExecList = new ObservableCollection<JobnetExecInfo>();
            errPage = new JobnetExecControlErrPage(this);
            errPage.JobnetExecList = new ObservableCollection<JobnetExecInfo>();
            runningPage = new JobnetExecControlRunningPage(this);
            runningPage.JobnetExecList = new ObservableCollection<JobnetExecInfo>();
            dispatcherTimer = new DispatcherTimer(DispatcherPriority.Normal);
            dispatcherTimer.Interval = new TimeSpan(0, 0, 1);
            dispatcherTimer.Tick += new EventHandler(refresh);
            dispatcherTimer.Start();
            _db.CreateSqlConnect();
            runJobnetSummaryDAO = new RunJobnetSummaryDAO(_db);
            hideJobnetInnerIdList = new List<decimal>();
            jobnetLoadSpan = Convert.ToInt32(DBUtil.GetParameterVelue("JOBNET_LOAD_SPAN"));
            _parent = parent;
            SetInit();

        }
        #endregion

        #region プロパティ
        private JobArrangerWindow _parent;
        public JobArrangerWindow Parent
        {
            get
            {
                return _parent;
            }
        }

        public override string ClassName
        {
            get
            {
                return "JobnetExecControlPage";
            }
        }

        public override string GamenId
        {
            get
            {
                return Consts.WINDOW_300;
            }
        }

        private DBConnect _db = new DBConnect(LoginSetting.ConnectStr);
        public DBConnect DB
        {
            get
            {
                return _db;
            }
        }

        #endregion

        #region イベント
        protected override void OnPreviewKeyDown(KeyEventArgs e)
        {
            if (e.Key == Key.Tab)
            {
                TraversalRequest tRequest = new TraversalRequest(FocusNavigationDirection.Next);
                UIElement keyboardFocus = Keyboard.FocusedElement as UIElement;

                if (keyboardFocus != null)
                {
                    if (keyboardFocus.Equals(allPage.listView1))
                    {
                        if (allPage.listView1.Items.Count > 0)
                        {
                            allPage.listView1.SelectedIndex = 0;
                        }
                        if (viewErrJobnet)
                        {
                            Keyboard.Focus(errPage.listView1);
                            if (errPage.listView1.Items.Count > 0)
                            {
                                errPage.listView1.SelectedIndex = 0;
                            }
                        }
                        else
                        {
                            if (viewRunningJobnet)
                            {
                                Keyboard.Focus(runningPage.listView1);
                                if (runningPage.listView1.Items.Count > 0)
                                {
                                    runningPage.listView1.SelectedIndex = 0;
                                }
                            }
                            else
                            {
                                Keyboard.Focus(Parent.MenuItemFile);
                            }
                        }
                    }
                    else if (keyboardFocus.Equals(errPage.listView1))
                    {
                        if (viewRunningJobnet)
                        {
                            Keyboard.Focus(runningPage.listView1);
                            if (runningPage.listView1.Items.Count > 0)
                            {
                                runningPage.listView1.SelectedIndex = 0;
                            }
                        }
                        else
                        {
                            Keyboard.Focus(Parent.MenuItemFile);
                        }
                    }
                    else if (keyboardFocus.Equals(runningPage.listView1))
                    {
                        Keyboard.Focus(Parent.MenuItemFile);
                    }
                    else
                    {
                        keyboardFocus.MoveFocus(tRequest);
                    }
                }

                e.Handled = true;
                return;
            }
            if (e.Key == Key.Enter)
            {
                JobnetExecInfo jobnetExecInfo;
                if (allPage.listView1.IsKeyboardFocusWithin && allPage.listView1.SelectedItems.Count > 0)
                {
                    jobnetExecInfo = (JobnetExecInfo)allPage.listView1.SelectedItem;
                    if (jobnetExecInfo != null) ViewDetail(jobnetExecInfo.inner_jobnet_id);
                    return;
                }
                if (errPage.listView1.IsKeyboardFocusWithin && errPage.listView1.SelectedItems.Count > 0)
                {
                    jobnetExecInfo = (JobnetExecInfo)errPage.listView1.SelectedItem;
                    if (jobnetExecInfo != null) ViewDetail(jobnetExecInfo.inner_jobnet_id);
                    return;
                }
                if (runningPage.listView1.IsKeyboardFocusWithin && runningPage.listView1.SelectedItems.Count > 0)
                {
                    jobnetExecInfo = (JobnetExecInfo)runningPage.listView1.SelectedItem;
                    if (jobnetExecInfo != null) ViewDetail(jobnetExecInfo.inner_jobnet_id);
                    return;
                }

        }

        }
        private void refresh(object sender, EventArgs e)
        {
            SetInit();
            resetDefinitions();
            resetData();
        }

        /// <summary>コマンド実行可否</summary>
        /// <param name="sender">源</param>
        /// <param name="e"></param>
        private void CommandBinding_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        /// <summary>強制停止</summary>
        /// <param name="sender">源</param>
        /// <param name="e"></param>
        private void AllStopCommandBinding_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            JobnetExecInfo jobnetExecInfo = (JobnetExecInfo)allPage.stopContextMenu.Tag;
            if ((RunJobStatusType)jobnetExecInfo.status == RunJobStatusType.During)
            {
                DBUtil.StopRunningJobnet(jobnetExecInfo.inner_jobnet_id);
            }
            else
            {
                DBUtil.StopUnexecutedJobnet(jobnetExecInfo.inner_jobnet_id);
            }

        }

        /// <summary>強制停止</summary>
        /// <param name="sender">源</param>
        /// <param name="e"></param>
        private void ErrStopCommandBinding_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            JobnetExecInfo jobnetExecInfo = (JobnetExecInfo)errPage.stopContextMenu.Tag;
            DBUtil.StopRunningJobnet(jobnetExecInfo.inner_jobnet_id);

        }

        /// <summary>強制停止</summary>
        /// <param name="sender">源</param>
        /// <param name="e"></param>
        private void RunningStopCommandBinding_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            JobnetExecInfo jobnetExecInfo = (JobnetExecInfo)runningPage.stopContextMenu.Tag;
            DBUtil.StopRunningJobnet(jobnetExecInfo.inner_jobnet_id);

        }

        /// <summary>非表示</summary>
        /// <param name="sender">源</param>
        /// <param name="e"></param>
        private void HideCommandBinding_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            JobnetExecInfo jobnetExecInfo = (JobnetExecInfo)errPage.hideContextMenu.Tag;
            hideJobnetInnerIdList.Add(jobnetExecInfo.inner_jobnet_id);
        }
        #endregion

        #region publicメソッド

        /// <summary>実行ジョブネット詳細画面表示</summary>
        /// <param name="inner_jobnet_id">実行ジョブネット内部管理ID</param>
        public void ViewDetail(decimal inner_jobnet_id)
        {
            JobnetExecDetail detail = new JobnetExecDetail(inner_jobnet_id.ToString(), true);
            detail.Topmost = true;
            detail.Focusable = true;
            Keyboard.Focus(detail);
            detail.Show();
            detail.Focusable = true;
            Keyboard.Focus(detail);
        }
        #endregion

        #region privateメソッド

        /// <summary>表示設定を初期化</summary>
        private void SetInit()
        {
            viewCount = 0;
            viewJobnet = false;
            viewErrJobnet = false;
            viewRunningJobnet = false;
            if (_parent.menuitemViewAll.IsChecked)
            {
                viewCount++;
                viewJobnet = true;
            }
            if (_parent.menuitemViewErr.IsChecked)
            {
                viewCount++;
                viewErrJobnet = true;
            }
            if (_parent.menuitemViewRunning.IsChecked)
            {
                viewCount++;
                viewRunningJobnet = true;
            }

        }

        /// <summary>画面配置を再設定する</summary>
        private void resetDefinitions()
        {
            if (viewCount != oldviewCount)
            {
                jobnetExecControlGrid.Children.Clear();
                jobnetExecControlGrid.RowDefinitions.Clear();
                for (int i = 0; i < viewCount; i++)
                {
                    jobnetExecControlGrid.RowDefinitions.Add(new RowDefinition() { Height = new GridLength(1, GridUnitType.Star) });
                }
                if (viewJobnet)
                {
                    jobnetExecControlGrid.Children.Add(allPage);
                    Grid.SetColumn(allPage, 0);
                    Grid.SetRow(allPage, getRowNums()[0]);

                }
                if (viewErrJobnet)
                {
                    jobnetExecControlGrid.Children.Add(errPage);
                    Grid.SetColumn(errPage, 0);
                    Grid.SetRow(errPage, getRowNums()[1]);

                }
                if (viewRunningJobnet)
                {
                    jobnetExecControlGrid.Children.Add(runningPage);
                    Grid.SetColumn(runningPage, 0);
                    Grid.SetRow(runningPage, getRowNums()[2]);

                }
                oldviewCount = viewCount;
            }
        }

        /// <summary>画面データを再設定する</summary>
        private void resetData()
        {
            if (viewJobnet) viewJobnetList();
            if (viewErrJobnet) viewErrJobnetList();
            if (viewRunningJobnet) viewRunningJobnetList();

        }

        /// <summary>ジョブネットリスト表示する</summary>
        private void viewJobnetList()
        {
            allPage.JobnetExecList.Clear();
            DataTable dt;
            DateTime now = DateTime.Now;
            DateTime before = now.AddMinutes(-1 * jobnetLoadSpan);
            DateTime after = now.AddMinutes(jobnetLoadSpan);
            decimal fromTime = ConvertUtil.ConverDate2IntYYYYMMDDHHMI(before);
            decimal toTime = ConvertUtil.ConverDate2IntYYYYMMDDHHMI(after);

            decimal startFromTime = ConvertUtil.ConverDate2IntYYYYMMDDHHMISS(before);
            decimal startToTime = ConvertUtil.ConverDate2IntYYYYMMDDHHMISS(after);

            if (LoginSetting.Authority.Equals(Consts.AuthorityEnum.SUPER))
            {
                dt = runJobnetSummaryDAO.GetEntitySuperAll(fromTime, toTime, startFromTime, startToTime);
            }
            else
            {
                dt = runJobnetSummaryDAO.GetEntityAll(fromTime, toTime, startFromTime, startToTime, LoginSetting.UserID);
            }
            int i = 0;
            foreach (DataRow row in dt.Rows)
            {
                JobnetExecInfo jobnetExecInfo = createJobnetExecInfo(row);
                allPage.JobnetExecList.Add(jobnetExecInfo);
                if (allPage.AllSelectedInnerJobnetId.Equals(Convert.ToString(jobnetExecInfo.inner_jobnet_id)))
                {
                    allPage.listView1.SelectedIndex = i;
                }
                i++;
            }
            allPage.listView1.GetBindingExpression(System.Windows.Controls.ListView.ItemsSourceProperty).UpdateTarget();
            //var gridView = allPage.listView1.View as GridView;
            //if (gridView != null)
            //{
            //    foreach (var c in gridView.Columns)
            //    {
            //        // double.Nanにすることで自動調整される。
            //        c.Width = 0;
            //        c.Width = double.NaN;
            //    }
            //}

        }

        /// <summary>エラージョブネットリスト表示する</summary>
        private void viewErrJobnetList()
        {
            errPage.JobnetExecList.Clear();
            DataTable dt;
            if (LoginSetting.Authority.Equals(Consts.AuthorityEnum.SUPER))
            {
                dt = runJobnetSummaryDAO.GetEntitySuperErr();
            }
            else
            {
                dt = runJobnetSummaryDAO.GetEntityErr(LoginSetting.UserID);
            }
            int i = 0;

            foreach (DataRow row in dt.Rows)
            {
                if (!hideJobnetInnerIdList.Contains(Convert.ToDecimal(row["inner_jobnet_id"])))
                {
                    JobnetExecInfo jobnetExecInfo = createJobnetExecInfo(row);
                    errPage.JobnetExecList.Add(jobnetExecInfo);
                    if (errPage.ErrSelectedInnerJobnetId.Equals(Convert.ToString(jobnetExecInfo.inner_jobnet_id)))
                    {
                        errPage.listView1.SelectedIndex = i;

                    }
                    i++;
                }
            }

            errPage.listView1.GetBindingExpression(System.Windows.Controls.ListView.ItemsSourceProperty).UpdateTarget();

            //var gridView = errPage.listView1.View as GridView;
            //if (gridView != null)
            //{
            //    foreach (var c in gridView.Columns)
            //    {
            //        // double.Nanにすることで自動調整される。
            //        c.Width = 0;
            //        c.Width = double.NaN;
            //    }
            //}
        }

        /// <summary>実行中ジョブネットリスト表示する</summary>
        private void viewRunningJobnetList()
        {
            runningPage.JobnetExecList.Clear();
            DataTable dt;
            if (LoginSetting.Authority.Equals(Consts.AuthorityEnum.SUPER))
            {
                dt = runJobnetSummaryDAO.GetEntitySuperRunning();
            }
            else
            {
                dt = runJobnetSummaryDAO.GetEntityRunning(LoginSetting.UserID);
            }
            int i = 0;
            foreach (DataRow row in dt.Rows)
            {
                JobnetExecInfo jobnetExecInfo = createJobnetExecInfo(row);
                runningPage.JobnetExecList.Add(jobnetExecInfo);
                if (runningPage.RunningSelectedInnerJobnetId.Equals(Convert.ToString(jobnetExecInfo.inner_jobnet_id)))
                {
                    runningPage.listView1.SelectedItem = jobnetExecInfo;
                }
                i++;
            }
            runningPage.listView1.GetBindingExpression(System.Windows.Controls.ListView.ItemsSourceProperty).UpdateTarget();
            //var gridView = runningPage.listView1.View as GridView;
            //if (gridView != null)
            //{
            //    foreach (var c in gridView.Columns)
            //    {
            //        // double.Nanにすることで自動調整される。
            //        c.Width = 0;
            //        c.Width = double.NaN;
            //    }
            //}
        }

        /// <summary>実行ジョブネットデータを作成する</summary>
        /// <param name="row">実行ジョブネットデータ</param>
        private JobnetExecInfo createJobnetExecInfo(DataRow row)
        {
            JobnetExecInfo jobnetExecInfo = new JobnetExecInfo();
            jobnetExecInfo.jobnet_id = row["jobnet_id"].ToString();
            jobnetExecInfo.status = (int)row["status"];
            jobnetExecInfo.display_status = getRunJobStatusStr((int)row["status"], (int)row["load_status"]);
            jobnetExecInfo.status_color = getRunJobStatusColor((int)row["status"], (int)row["job_status"]);
            jobnetExecInfo.jobnet_name = row["jobnet_name"].ToString();

            if (Convert.ToDecimal(row["scheduled_time"]) > 0)
            {
                jobnetExecInfo.scheduled_time = ConvertUtil.ConverIntYYYYMMDDHHMI2Date(Convert.ToDecimal(row["scheduled_time"])).ToString();
            }
            else
            {
                jobnetExecInfo.scheduled_time = "";
            }
            if (Convert.ToDecimal(row["start_time"]) > 0)
            {
                jobnetExecInfo.start_time = ConvertUtil.ConverIntYYYYMMDDHHMISS2Date(Convert.ToDecimal(row["start_time"])).ToString();
            }
            else
            {
                jobnetExecInfo.start_time = "";
            }
            if (Convert.ToDecimal(row["end_time"]) > 0)
            {
                jobnetExecInfo.end_time = ConvertUtil.ConverIntYYYYMMDDHHMISS2Date(Convert.ToDecimal(row["end_time"])).ToString();
            }
            else
            {
                jobnetExecInfo.end_time = "";
            }
            jobnetExecInfo.inner_jobnet_id = Convert.ToDecimal(row["inner_jobnet_id"]);

            return jobnetExecInfo;

        }
        /// <summary>各リストの画面Gridの行番号を取得</summary>
        private int[] getRowNums()
        {
            int[] rowNumbers = new int[] { 0, 1, 2 };
            if (viewCount == 3)
            {
                rowNumbers[0] = 0;
                rowNumbers[1] = 1;
                rowNumbers[2] = 2;
            }
            if (viewCount == 2)
            {
                if (!viewJobnet)
                {
                    rowNumbers[0] = -1;
                    rowNumbers[1] = 0;
                    rowNumbers[2] = 1;
                }
                if (!viewErrJobnet)
                {
                    rowNumbers[0] = 0;
                    rowNumbers[1] = -1;
                    rowNumbers[2] = 1;
                }
                if (!viewRunningJobnet)
                {
                    rowNumbers[0] = 0;
                    rowNumbers[1] = 1;
                    rowNumbers[2] = -1;
                }
            }
            if (viewCount == 1)
            {
                if (viewJobnet)
                {
                    rowNumbers[0] = 0;
                    rowNumbers[1] = -1;
                    rowNumbers[2] = -1;
                }
                if (viewErrJobnet)
                {
                    rowNumbers[0] = -1;
                    rowNumbers[1] = 0;
                    rowNumbers[2] = -1;
                }
                if (viewRunningJobnet)
                {
                    rowNumbers[0] = -1;
                    rowNumbers[1] = -1;
                    rowNumbers[2] = 0;
                }
            }
            if (viewCount == 0)
            {
                rowNumbers[0] = -1;
                rowNumbers[1] = -1;
                rowNumbers[2] = -1;
            }
            return rowNumbers;
        }

        /// <summary>各リストの画面Gridの行番号を取得</summary>
        private String getRunJobStatusStr(int status, int load_status)
        {
            String str;
            switch (status)
            {
                case 2:
                case 4:
                case 6:
                    str = Properties.Resources.job_run_status_running;
                    break;
                case 3:
                    str = Properties.Resources.job_run_status_done;
                    break;
                case 5:
                    str = Properties.Resources.job_run_status_done;
                    if (load_status==1)
                        str = Properties.Resources.load_err;
                    break;
                default:
                    str = Properties.Resources.job_run_status_schedule;
                    break;
            }
            return str;
        }

        /// <summary>各リストの画面Gridの行番号を取得</summary>
        /// <param name="status">実行ジョブネットステータス</param>
        /// <param name="run_type">実行ジョブネット実行種別</param>
        private SolidColorBrush getRunJobStatusColor(int status, int run_type)
        {
            SolidColorBrush color = new SolidColorBrush(Colors.Aquamarine);
            switch (status)
            {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                case 6:
                    switch (run_type)
                    {
                        case 0:
                            color = new SolidColorBrush(Colors.Yellow);
                            break;
                        case 1:
                            color = new SolidColorBrush(Colors.Orange);
                            break;
                        case 2:
                            color = new SolidColorBrush(Colors.Red);
                            break;
                    }
                    break;
                case 3:
                    switch (run_type)
                    {
                        case 0:
                            color = new SolidColorBrush(Colors.Lime);
                            break;
                        case 1:
                            color = new SolidColorBrush(Colors.Orange);
                            break;
                        case 2:
                            color = new SolidColorBrush(Colors.Red);
                            break;
                    }
                    break;
                case 4:
                case 5:
                    color = new SolidColorBrush(Colors.Red);
                    break;
            }
            return color;
        }

        #endregion


    }
}
