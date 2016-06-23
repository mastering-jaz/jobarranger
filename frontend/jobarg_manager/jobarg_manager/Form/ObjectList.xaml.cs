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
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using jp.co.ftf.jobcontroller.DAO;
using System.Data;
using System.Windows.Controls;
using jp.co.ftf.jobcontroller.JobController.Form.JobEdit;
using jp.co.ftf.jobcontroller.Common;
using jp.co.ftf.jobcontroller.JobController.Form.CalendarEdit;
using jp.co.ftf.jobcontroller.JobController.Form.ScheduleEdit;
using jp.co.ftf.jobcontroller.JobController.Form.JobEdit;

namespace jp.co.ftf.jobcontroller.JobController
{
    /// <summary>
    /// ObjectList.xaml の相互作用ロジック(ジョブ編集画面テスト用)
    /// </summary>
    public partial class ObjectList : BaseUserControl
    {
        #region フィールド

        /// <summary>ＤＢコネクター</summary>
        private DBConnect _db = new DBConnect(LoginSetting.ConnectStr);

        /// <summary>画面オブジェクトDAO</summary>
        private ObjectBaseDAO _objectDao;

        /// <summary>画面オブジェクトデータTable</summary>
        private DataTable _objectTbl;

        /// <summary>画面オブジェクトのユーザーグループＩＤリスト</summary>
        private List<Decimal> _objectUserGroupList = new List<Decimal>();

        /// <summary>画面オブジェクトのユーザー名</summary>
        private String _objectUserName;

        /// <summary>Tabキーかのフラグ</summary>
        private bool _isTabKey;

        #endregion

        #region コンストラクタ
        /// <summary>
        /// コンストラクタ 
        /// </summary>
        public ObjectList()
        {
            InitializeComponent();
        }

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        public ObjectList(JobArrangerWindow win, string objectId, Consts.ObjectEnum objectType)
        {
            InitializeComponent();
            _dadWindow = win;
            _objectId = objectId;
            _objectType = objectType;
            SetList();

        }
        #endregion

        #region プロパティ

        /// <summary>クラス名</summary>
        public override string ClassName
        {
            get
            {
                return "ObjectList";
            }
        }

        /// <summary>画面ID</summary>
        public override string GamenId
        {
            get
            {
                return Consts.WINDOW_200;
            }
        }
        /// <summary>オブジェクトＩＤ</summary>
        private String _objectId;
        public String ObjectId
        {
            get
            {
                return _objectId;
            }
            set
            {
                _objectId = value;
            }
        }
        /// <summary>オブジェクト種別</summary>
        private Consts.ObjectEnum _objectType;
        public Consts.ObjectEnum ObjectType
        {
            get
            {
                return _objectType;
            }
            set
            {
                _objectType = value;
            }
        }
        /// <summary>オブジェクト所有種別</summary>
        private Consts.ObjectOwnType _objectOwnType;
        public Consts.ObjectOwnType ListObjectOwnType
        {
            get
            {
                return _objectOwnType;
            }
            set
            {
                _objectOwnType = value;
            }
        }

        /// <summary>父ウィンドウ</summary>
        private JobArrangerWindow _dadWindow;
        public JobArrangerWindow DadWindow
        {
            get
            {
                return _dadWindow;
            }
            set
            {
                _dadWindow = value;
            }
        }

        #endregion

        #region イベント

        //*******************************************************************
        /// <summary>コンテクストメニューを表示</summary>
        /// <param name="sender">源</param>
        /// <param name="e">マウスイベント</param>
        //*******************************************************************
        private void ContextMenu_Open(object sender, RoutedEventArgs e)
        {
            SetContextStatus();

        }

        //*******************************************************************
        /// <summary>TreeViewのPreview右マウスクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">マウスイベント</param>
        //*******************************************************************
        private void dgObject_PreviewMouseRightButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {

            DependencyObject dep = (DependencyObject)e.OriginalSource;
            while ((dep != null) && !(dep is DataGridCell))
            {
                dep = VisualTreeHelper.GetParent(dep);
            }
            if (dep == null) return;

            if (dep is DataGridCell)
            {
                while ((dep != null) && !(dep is DataGridRow))
                {
                    dep = VisualTreeHelper.GetParent(dep);
                }
                DataGridRow row = dep as DataGridRow;
                dgObject.SelectedItem = row.DataContext;
            }

        }

        //*******************************************************************
        /// <summary>TreeViewのPreview右マウスクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">マウスイベント</param>
        //*******************************************************************
        private void dgObject_PreviewMouseRightButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            contextMenu.PlacementTarget = sender as UIElement;
            contextMenu.IsOpen = true;

            #if VIEWER
                this.contextMenu.Visibility = System.Windows.Visibility.Hidden;

            #else

                this.contextMenu.IsOpen = true;
                this.contextMenu.Visibility = System.Windows.Visibility.Visible;
            #endif

        }

        /// <summary>行をダブルクリック</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void dgObject_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (dgObject.SelectedValue != null)
            {
                // 開始ログ
                base.WriteStartLog("dgObject_MouseDoubleClick", Consts.PROCESS_004);

                string updDate = dgObject.SelectedValue.ToString();
                DataRow row = ((DataRowView)dgObject.SelectedItem).Row;

                //Viewerの場合
                #if VIEWER
                    _dadWindow.EditObject(_objectId, updDate, Consts.EditType.READ, _objectType);
                #else

                    // 運用モードの場合
                    if (LoginSetting.Mode == Consts.ActionMode.USE)
                    {
                        _dadWindow.EditObject(_objectId, updDate, Consts.EditType.READ, _objectType);
                    }
                    else
                    {
                        // ログインユーザーグループに属するオブジェクトではなく、特権ユーザーではない場合
                        if (_objectOwnType == Consts.ObjectOwnType.Other && !(LoginSetting.Authority == Consts.AuthorityEnum.SUPER))
                        {
                            _dadWindow.EditObject(_objectId, updDate, Consts.EditType.READ, _objectType);
                        }
                        else
                        {
                            // 有効の場合
                            if ((Int32)row["valid_flag"] == 1)
                            {
                                _dadWindow.EditObject(_objectId, updDate, Consts.EditType.READ, _objectType);
                            }
                            else
                            {
                                _dadWindow.EditObject(_objectId, updDate, Consts.EditType.Modify, _objectType);
                            }
                        }
                    }
                #endif
                // 終了ログ
                base.WriteEndLog("dgObject_MouseDoubleClick", Consts.PROCESS_004);
            }
        }

        /// <summary>フォーカス取得時処理</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DataGrid_GotFocus(object sender, RoutedEventArgs e)
        {
            if (dgObject.SelectedItems.Count < 1)
            {
                dgObject.SelectedItem = dgObject.Items[0];
            }
            else
            {
                if (_isTabKey)
                {
                    System.Windows.Controls.DataGridRow dgrow = (System.Windows.Controls.DataGridRow)dgObject.ItemContainerGenerator.ContainerFromItem(dgObject.Items[dgObject.SelectedIndex]);
                    System.Windows.Controls.DataGridCell dgc = dgObject.Columns[0].GetCellContent(dgrow).Parent as System.Windows.Controls.DataGridCell;
                    FocusManager.SetFocusedElement(dgObject, dgc as IInputElement);
                    _isTabKey = false;
                }
            }
        }

        private void DataGrid_MouseUp(object sender, MouseButtonEventArgs e)
        {
            _isTabKey = false;
        } 


        /// <summary>Delete key 押下</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DataGrid_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            _isTabKey = false;
            if (e.Key == Key.Delete)
            {
                DadWindow.DelFromMenuitemOrKey();
                e.Handled = true;
            }
            if (e.Key == Key.Return)
            {
                if (dgObject.SelectedValue != null)
                {
                    // 開始ログ
                    base.WriteStartLog("dgObject_MouseDoubleClick", Consts.PROCESS_004);

                    string updDate = dgObject.SelectedValue.ToString();
                    DataRow row = ((DataRowView)dgObject.SelectedItem).Row;

                    //Viewerの場合
                    #if VIEWER
                        _dadWindow.EditObject(_objectId, updDate, Consts.EditType.READ, _objectType);
                    #else

                        // 運用モードの場合
                        if (LoginSetting.Mode == Consts.ActionMode.USE)
                        {
                            _dadWindow.EditObject(_objectId, updDate, Consts.EditType.READ, _objectType);
                        }
                        else
                        {
                            // ログインユーザーグループに属するオブジェクトではなく、特権ユーザーではない場合
                            if (_objectOwnType == Consts.ObjectOwnType.Other && !(LoginSetting.Authority == Consts.AuthorityEnum.SUPER))
                            {
                                _dadWindow.EditObject(_objectId, updDate, Consts.EditType.READ, _objectType);
                            }
                            else
                            {
                                // 有効の場合
                                if ((Int32)row["valid_flag"] == 1)
                                {
                                    _dadWindow.EditObject(_objectId, updDate, Consts.EditType.READ, _objectType);
                                }
                                else
                                {
                                    _dadWindow.EditObject(_objectId, updDate, Consts.EditType.Modify, _objectType);
                                }
                            }
                        }
                    #endif
                    e.Handled = true;
                    // 終了ログ
                    base.WriteEndLog("dgObject_MouseDoubleClick", Consts.PROCESS_004);
                }

            }
            if (e.Key == Key.Tab)
            {
                _isTabKey = true;
            }
        }

        //*******************************************************************
        /// <summary>新規追加メニュークリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        //*******************************************************************
        private void MenuitemAdd_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("MenuitemAdd_Click", Consts.PROCESS_003);

            DadWindow.ObjectEdit = null;

            if (_objectType != null)
            {
                switch (_objectType)
                {
                    case Consts.ObjectEnum.CALENDAR:
                        DadWindow.ObjectEdit = new CalendarEdit(DadWindow);
                        break;

                    case Consts.ObjectEnum.SCHEDULE:
                        DadWindow.ObjectEdit = new ScheduleEdit(DadWindow);
                        break;

                    case Consts.ObjectEnum.JOBNET:
                        DadWindow.ObjectEdit = new JobEdit(DadWindow);
                        break;
                }
            }
            else
            {
                DadWindow.ObjectEdit = new CalendarEdit(DadWindow);
            }
            if (DadWindow.ObjectEdit.SuccessFlg)
            {
                DadWindow.ClearGridContent();

                DadWindow.JobNetGrid.Children.Add(DadWindow.ObjectEdit);

                DadWindow.Title = MessageUtil.GetMsgById(DadWindow.ObjectEdit.GamenId) + " - " + LoginSetting.JobconName;
            }
            // 終了ログ
            base.WriteEndLog("MenuitemAdd_Click", Consts.PROCESS_003);
        }


        /// <summary>オブジェクトを編集</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MenuitemEdit_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("MenuitemEdit_Click", Consts.PROCESS_004);

            string updDate = dgObject.SelectedValue.ToString();

            // 運用モードの場合
            if (LoginSetting.Mode == Consts.ActionMode.USE)
            {
                _dadWindow.EditObject(_objectId, updDate, Consts.EditType.READ, _objectType);
            }
            else
            {
                _dadWindow.EditObject(_objectId, updDate, Consts.EditType.Modify, _objectType);
            }
            // 終了ログ
            base.WriteEndLog("MenuitemEdit_Click", Consts.PROCESS_004);
        }

        /// <summary>オブジェクトをコピー新規</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MenuitemCopyNew_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("MenuitemCopyNew_Click", Consts.PROCESS_017);

            string updDate = dgObject.SelectedValue.ToString();

            _dadWindow.EditObject(_objectId, updDate, Consts.EditType.CopyNew, _objectType);

            // 終了ログ
            base.WriteEndLog("MenuitemCopyNew_Click", Consts.PROCESS_017);
        }

        /// <summary>オブジェクトをコピー新バージョン作成</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MenuitemCopyVer_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("MenuitemCopyVer_Click", Consts.PROCESS_005);

            string updDate = dgObject.SelectedValue.ToString();

            _dadWindow.EditObject(_objectId, updDate, Consts.EditType.CopyVer, _objectType);

            // 終了ログ
            base.WriteEndLog("MenuitemCopyVer_Click", Consts.PROCESS_005);
        }

        /// <summary>オブジェクトを有効</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MenuitemValid_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("MenuitemValid_Click", Consts.PROCESS_007);

            string updDate = dgObject.SelectedValue.ToString();

            // ジョブネットの場合、フローの整合性チェックをする
            if (_objectType == Consts.ObjectEnum.JOBNET)
            {
                _db.CreateSqlConnect();
                JobControlDAO jobControlDAO = new JobControlDAO(_db);
                FlowControlDAO flowControlDAO = new FlowControlDAO(_db);
                DataTable dtJob = jobControlDAO.GetEntityByJobIdForUpdate(_objectId, updDate);
                DataTable dtFlow = flowControlDAO.GetEntityByJobnet(_objectId, updDate);
                _db.CloseSqlConnect();

                if (!ConformJobnetCheck(dtJob, dtFlow))
                {
                    return;
                }
            }

            // スケジュールの場合、起動時刻、ジョブネット登録チェックをする
            if (_objectType == Consts.ObjectEnum.SCHEDULE)
            {
                _db.CreateSqlConnect();
                ScheduleDetailDAO scheduleDetailDAO = new ScheduleDetailDAO(_db);
                ScheduleJobnetDAO scheduleJobnetDAO = new ScheduleJobnetDAO(_db);
                DataTable dtBoot = scheduleDetailDAO.GetIdEntityBySchedule(_objectId, updDate);
                DataTable dtJobnet = scheduleJobnetDAO.GetIdEntityBySchedule(_objectId, updDate);
                _db.CloseSqlConnect();
                if (!ConfirmScheduleCheck(dtBoot, dtJobnet))
                {
                    return;
                }
            }

            try
            {
                DBUtil.SetObjectValid(_objectId, updDate, _objectType);
            }
            catch (DBException ex)
            {
                if (ex.MessageID.Equals(Consts.ERROR_DB_LOCK))
                {
                    CommonDialog.ShowErrorDialog(Consts.ERROR_DB_LOCK);
                }
                else
                {
                    throw ex;
                }
            }

            SetList();

            // 終了ログ
            base.WriteEndLog("MenuitemValid_Click", Consts.PROCESS_007);
        }

        /// <summary>オブジェクトを無効</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MenuitemInValid_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("MenuitemInValid_Click", Consts.PROCESS_008);
            try
            {
                DBUtil.SetObjectsInValid(_objectId, _objectType, GetSelectedRows());
            }
            catch (DBException ex)
            {
                if (ex.MessageID.Equals(Consts.ERROR_DB_LOCK))
                {
                    CommonDialog.ShowErrorDialog(Consts.ERROR_DB_LOCK);
                }
                else
                {
                    throw ex;
                }
            }
            SetList();

            // 終了ログ
            base.WriteEndLog("MenuitemInValid_Click", Consts.PROCESS_008);
        }

        /// <summary>オブジェクトを削除</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MenuitemDel_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("MenuitemDel_Click", Consts.PROCESS_006);

            DelObject(_objectId, _objectType, GetSelectedRows());

            // 終了ログ
            base.WriteEndLog("MenuitemDel_Click", Consts.PROCESS_006);
        }

        /// <summary>オブジェクトをエクスポート</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MenuitemExport_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("MenuitemExport_Click", Consts.PROCESS_012);

            ExportWindow exportWindow = new ExportWindow(_objectId, _objectType, GetSelectedRows());
            exportWindow.Owner = DadWindow;
            exportWindow.WindowStartupLocation = System.Windows.WindowStartupLocation.CenterScreen;
            exportWindow.ShowDialog();

            // 終了ログ
            base.WriteEndLog("MenuitemExport_Click", Consts.PROCESS_012);
        }

        #endregion

        #region public メソッド
        /// <summary>データ削及び一覧表示</summary>
        /// <param name="objectId">選択されたオブジェクトＩＤ</param>
        /// <param name="objectType">選択されたオブジェクト種別</param>
        /// <param name="rows">選択されたオブジェクトデータ</param>
        public void DelObject(String objectId, Consts.ObjectEnum objectType, DataRow[] rows)
        {
            if (!DBUtil.CheckForRelation4Del(objectId, objectType, rows))
            {
                CommonDialog.ShowErrorDialog(Consts.MSG_COMMON_007);
                return;
            }
            MessageBoxResult result = CommonDialog.ShowDeleteDialog();
            if (result == MessageBoxResult.Yes)
            {
                bool publicFlag = false;
                DataTable dt = DBUtil.GetObjectsById(objectId, objectType);
                if (dt.Rows.Count > 0 && (Int32)(dt.Rows[0]["public_flag"]) == 1)
                    publicFlag = true;
                try
                {
                    DBUtil.DelObject(objectId, objectType, rows);
                    SetList();
                    DadWindow.SetTreeObject(publicFlag, objectType, objectId);

                }
                catch (DBException e)
                {
                    if (e.MessageID.Equals(Consts.ERROR_DB_LOCK))
                    {
                        CommonDialog.ShowErrorDialog(Consts.ERROR_DB_LOCK);
                    }
                    else
                    {
                        throw e;
                    }
                }
            }
        }

        /// <summary>選択したデータ取得</summary>
        /// <return>選択されたDataRow配列</return>
        public DataRow[] GetSelectedRows()
        {
            DataRow[] rows = new DataRow[dgObject.SelectedItems.Count];
            for (int i = 0; i < dgObject.SelectedItems.Count; i++)
            {
                System.Data.DataRowView dataRowView = (System.Data.DataRowView)dgObject.SelectedItems[i];
                rows[i] = dataRowView.Row;

            }
            return rows;
        }
        #endregion

        #region private メソッド

        /// <summary>リスト内容をセット</summary>
        private void SetList()
        {
            _db.CreateSqlConnect();

            _objectDao = GetObjectDAO();
            if (_objectId != null && !_objectId.Equals(""))
            {
                _objectTbl = _objectDao.GetEntityByObjectId(_objectId);
                _objectTbl.Columns[0].ColumnName = "object_id";
                foreach (DataColumn cl in _objectTbl.Columns)
                {
                    if (!cl.ColumnName.Equals("user_name") && cl.ColumnName.IndexOf("_name") > 0)
                    {
                        cl.ColumnName = "object_name";
                    }
                }

                dgObject.ItemsSource = _objectTbl.DefaultView;

                dgObject.SelectedValuePath = Convert.ToString(_objectTbl.Columns["update_date"]);
                if (_objectTbl.Rows.Count > 0)
                {
                    _objectUserName = (String)_objectTbl.Rows[0]["user_name"];
                    _objectUserGroupList = DBUtil.GetGroupIDListByAlias(_objectUserName);
                }
                _objectOwnType = Consts.ObjectOwnType.Other;
                if (CheckUtil.isExistGroupId(LoginSetting.GroupList, _objectUserGroupList))
                {
                    _objectOwnType = Consts.ObjectOwnType.Owner;
                }
                if (_objectTbl.Rows.Count < 1)
                {
                    _objectId = null;
                }
            }
            _db.CloseSqlConnect();
        }

        /// <summary>オブジェクト種別によるオブジェクトＤＡＯ取得</summary>
        private ObjectBaseDAO GetObjectDAO()
        {
            ObjectBaseDAO objectDao = new CalendarControlDAO(_db);
            switch (_objectType)
            {
                case Consts.ObjectEnum.CALENDAR:
                    break;
                case Consts.ObjectEnum.SCHEDULE:
                    objectDao = new ScheduleControlDAO(_db);
                    break;
                case Consts.ObjectEnum.JOBNET:
                    objectDao = new JobnetControlDAO(_db);
                    break;
            }
            return objectDao;
        }

        private void SetContextStatus()
        {
            menuitemAdd.IsEnabled = true;
            menuitemEdit.IsEnabled = true;
            menuitemCopyNew.IsEnabled = true;
            menuitemCopyVer.IsEnabled = true;
            menuitemValid.IsEnabled = true;
            menuitemInValid.IsEnabled = true;
            menuitemDel.IsEnabled = true;
            menuitemExport.IsEnabled = true;

            if (LoginSetting.Mode == Consts.ActionMode.USE)
            {
                menuitemAdd.IsEnabled = false;
                menuitemEdit.IsEnabled = false;
                menuitemCopyNew.IsEnabled = false;
                menuitemCopyVer.IsEnabled = false;
                menuitemDel.IsEnabled = false;
            }

            if (dgObject.SelectedItems.Count < 1)
            {
                menuitemEdit.IsEnabled = false;
                menuitemCopyNew.IsEnabled = false;
                menuitemCopyVer.IsEnabled = false;
                menuitemValid.IsEnabled = false;
                menuitemInValid.IsEnabled = false;
                menuitemDel.IsEnabled = false;
                menuitemExport.IsEnabled = false;
                return;
            }

            if (_objectOwnType == Consts.ObjectOwnType.Other && !(LoginSetting.Authority == Consts.AuthorityEnum.SUPER))
            {
                menuitemEdit.IsEnabled = false;
                menuitemCopyNew.IsEnabled = false;
                menuitemCopyVer.IsEnabled = false;
                menuitemValid.IsEnabled = false;
                menuitemInValid.IsEnabled = false;
                menuitemDel.IsEnabled = false;
            }

            if (dgObject.SelectedItems.Count > 1)
            {
                menuitemEdit.IsEnabled = false;
                menuitemCopyNew.IsEnabled = false;
                menuitemCopyVer.IsEnabled = false;
                menuitemValid.IsEnabled = false;
            }

            if (dgObject.SelectedItems.Count == 1)
            {
                DataRowView dataRowView = (DataRowView)(dgObject.SelectedItems[0]);
                DataRow row = dataRowView.Row;
                if ((Int32)row["valid_flag"] == 1)
                {
                    menuitemEdit.IsEnabled = false;
                }
            }

        }

        //*******************************************************************
        /// <summary>ジョブネット整合性チェック </summary>
        /// <returns>チェック結果</returns>
        //*******************************************************************
        private bool ConformJobnetCheck(DataTable dtJob, DataTable dtFlow)
        {
            bool result = true;
            string jobId;
            ElementType jobType;
            int inFlowNum = 0;
            int outFlowNum = 0;
            int trueFlowNum = 0;
            int falseFlowNum = 0;
            DataRow[] flowRows = null;

            if (dtJob.Select().Length == 0)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_009);
                result = false;
                return result;
            }
            int startIconCount = 0;
            int endIconCount = 0;

            foreach (DataRow row in dtJob.Select())
            {
                // ジョブID 
                jobId = Convert.ToString(row["job_id"]);
                // ジョブタイプ 
                jobType = (ElementType)row["job_type"];
                // INフローの本数 
                flowRows = dtFlow.Select("end_job_id ='" + jobId + "'");
                if (flowRows != null)
                    inFlowNum = flowRows.Length;
                // OUTフローの本数 
                flowRows = dtFlow.Select("start_job_id ='" + jobId + "'");
                if (flowRows != null)
                    outFlowNum = flowRows.Length;

                // 整合性チェック 
                switch (jobType)
                {
                    // ジョブアイコン 
                    case ElementType.JOB:
                    // ジョブコントローラ変数アイコン 
                    case ElementType.ENV:
                    // 拡張ジョブアイコン 
                    case ElementType.EXTJOB:
                    // ジョブネットアイコン 
                    case ElementType.JOBNET:
                    // 計算アイコン 
                    case ElementType.CAL:
                    // タスクアイコン 
                    case ElementType.TASK:
                    // 情報取得アイコン 
                    case ElementType.INF:
                    // ファイル転送アイコン 
                    case ElementType.FCOPY:
                    // ファイル待ち合わせアイコン 
                    case ElementType.FWAIT:
                    // リブートアイコン 
                    case ElementType.REBOOT:
                    // 保留解除アイコン 
                    case ElementType.RELEASE:
                        {
                            // INフローの本数≠1、またはOUTフローの本数≠1の場合 
                            if (inFlowNum != 1 || outFlowNum != 1)
                                result = false;
                            break;
                        }
                    case ElementType.START:
                        {
                            startIconCount++;
                            // INフローの本数≠0、またはOUTフローの本数≠1の場合 
                            if (inFlowNum != 0 || outFlowNum != 1)
                                result = false;
                            break;
                        }

                    case ElementType.END:
                        {
                            endIconCount++;
                            // INフローの本数≠1、またはOUTフローの本数≠0の場合 
                            if (inFlowNum != 1 || outFlowNum != 0)
                                result = false;
                            break;
                        }

                    // 条件分岐アイコン 
                    case ElementType.IF:
                        {
                            // TRUEフローの本数を取得 
                            flowRows = dtFlow.Select(
                                "start_job_id ='" + jobId + "' and flow_type=1");
                            if (flowRows != null)
                                trueFlowNum = flowRows.Length;

                            // FALSEフロー本数を取得 
                            flowRows = dtFlow.Select(
                                "start_job_id ='" + jobId + "' and flow_type=2");
                            if (flowRows != null)
                                falseFlowNum = flowRows.Length;

                            // INフローの本数≠1、TRUEフローの本数≠1 
                            // またはFALSEフローの本数≠1の場合 
                            if (inFlowNum != 1 || trueFlowNum != 1 || falseFlowNum != 1)
                                result = false;
                            break;
                        }
                    // 並行処理開始アイコン 
                    case ElementType.MTS:
                        {
                            //INフローの本数≠1、またはOUTフローの本数=0の場合 
                            if (inFlowNum != 1 || outFlowNum < 1)
                                result = false;
                            break;
                        }
                    // 行処理終了アイコン 
                    case ElementType.MTE:
                        {
                            //INフローの本数=0、またはOUTフローの本数≠1の場合 
                            if (inFlowNum < 1 || outFlowNum != 1)
                                result = false;
                            break;
                        }
                    // ループアイコン 
                    case ElementType.LOOP:
                        {
                            //INフローの本数≠2、またはOUTフローの本数≠1の場合 
                            if (inFlowNum != 2 || outFlowNum != 1)
                                result = false;
                            break;
                        }
                    //added by kim 2012/11/14
                    // 分岐処理終了アイコン 
                    case ElementType.IFE:
                        {
                            //INフローの本数=0、またはOUTフローの本数≠1の場合 
                            if (inFlowNum < 1 || outFlowNum != 1)
                                result = false;
                            break;
                        }
                }


                // 整合しない場合 
                if (result == false)
                {
                    CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_005,
                        new string[] { jobId });
                    result = false;
                    break;
                }

            }
            // Start,Endアイコンチェック 
            if (result == true && (startIconCount != 1 || endIconCount < 1))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_012);
                result = false;
            }

            return result;
        }

        //*******************************************************************
        /// <summary>スケジュール起動時刻、ジョブネット登録チェック </summary>
        /// <returns>チェック結果</returns>
        //*******************************************************************
        private bool ConfirmScheduleCheck(DataTable dtBoot, DataTable dtJobnet)
        {
            bool result = true;
            if (dtBoot.Rows.Count == 0)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_SCHEDULE_003);
                result = false;
                return result;
            }
            if (dtJobnet.Rows.Count == 0)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_SCHEDULE_004);
                result = false;
                return result;
            }
            return result;
        }

        #endregion
    }
}
