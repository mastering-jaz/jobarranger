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
using System.Windows.Controls;
using jp.co.ftf.jobcontroller.Common;
using jp.co.ftf.jobcontroller.JobController;
using System.Windows.Input;
using jp.co.ftf.jobcontroller.JobController.Form.CalendarEdit;
using jp.co.ftf.jobcontroller.JobController.Form.ScheduleEdit;
using jp.co.ftf.jobcontroller.JobController.Form.JobEdit;
using System.Configuration;
using jp.co.ftf.jobcontroller.DAO;
using System.Data;
using System;
using System.Windows.Media;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author KIM 2012/11/15 新規作成<BR>                           *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.ScheduleEdit
{
    /// <summary>
    /// JobArrangerWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class ScheduleCalendarRegistWindow : BaseWindow
    {
        #region フィールド

        /// <summary>DBアクセスインスタンス</summary>
        private DBConnect dbAccess = new DBConnect(LoginSetting.ConnectStr);

        /// <summary>全部テーブル格納場所（編集状態判定用） </summary>
        private DataSet dataSet = new DataSet();

        /// <summary> カレンダーテーブル（公開フラグ更新用） </summary>
        private DataTable _calendarTbl;

        /// <summary> カレンダー管理テーブル </summary>
        private CalendarControlDAO _calendarControlDAO;

        /// <summary> カレンダー稼働日テーブル </summary>
        private CalendarDetailDAO _calendarDetailDAO;

        #endregion

        #region コンストラクタ
        public ScheduleCalendarRegistWindow(Container parantContainer)
        {
            InitializeComponent();
            LoadForInit(parantContainer);
            treeViewCalendar.IsSelected = true;

        }
        #endregion

        #region プロパティ
        /// <summary>クラス名</summary>
        public override string ClassName
        {
            get
            {
                return "ScheduleCalendarRegistWindow";
            }
        }

        /// <summary>画面ID</summary>
        public override string GamenId
        {
            get
            {
                return Consts.WINDOW_221;
            }
        }
        /// <summary>カレンダーＩＤ</summary>
        private string _calendarId;
        public string CalendarId
        {
            get
            {
                return _calendarId;
            }
            set
            {
                _calendarId = value;
            }
        }
        /// <summary>親Container</summary>
        private Container _parantContainer;
        public Container ParentContainer
        {
            get
            {
                return _parantContainer;
            }
            set
            {
                _parantContainer = value;
            }
        }
        #endregion

        #region イベント
        /// <summary>オブジェクトを選択</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        private void Calendar_Selected(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = (TreeViewItem)sender;
            _calendarId = ((TreeViewItem)sender).Header.ToString();

            // 各テーブルのデータをコピー追加 
            FillTables(_calendarId);
            // 情報エリアの表示 
            SetInfoArea();
            // カレンダー稼働日の表示 
            ShowCalendarDetail();
            btnRegist.IsEnabled = true;
            container.textBox_bootTimeHH.IsEnabled = true;
            container.textBox_bootTimeMI.IsEnabled = true;
            container.btnLeft.IsEnabled = true;
            container.btnRight.IsEnabled = true;
            e.Handled = true;

        }

        //*******************************************************************
        /// <summary>登録ボタンをクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        //*******************************************************************
        private void regist_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("regist_Click", Consts.PROCESS_001);

            // 入力チェック 
            if (!InputCheck())
                return;
            String bootTime = container.textBox_bootTimeHH.Text + container.textBox_bootTimeMI.Text;
            if (bootTime.Length == 3) bootTime = "0" + bootTime;
            DataRow[] rows = ParentContainer.ScheduleDetailTable.Select("calendar_id='" + CalendarId + "' and boot_time='" + bootTime + "'");

            if (rows.Length < 1)
            {
                DataRow row = ParentContainer.ScheduleDetailTable.NewRow();
                row["calendar_id"] = CalendarId;
                row["boot_time"] = bootTime;
                row["schedule_id"] = ParentContainer.ScheduleId;
                ParentContainer.ScheduleDetailTable.Rows.Add(row);
                CalendarControlDAO calendarControlDAO = new CalendarControlDAO(dbAccess);
                DataTable dt = calendarControlDAO.GetValidORMaxUpdateDateEntityById(_calendarId);
                row["calendar_name"] = dt.Rows[0]["calendar_name"];
            }
            this.Close();
            // 終了ログ
            base.WriteEndLog("regist_Click", Consts.PROCESS_001);
        }

        //*******************************************************************
        /// <summary> キャンセルボタンをクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        //*******************************************************************
        private void cancel_Click(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("regist_Click", Consts.PROCESS_002);

            this.Close();

            // 終了ログ
            base.WriteEndLog("regist_Click", Consts.PROCESS_002);
        }

        /// <summary>公開カレンダーを展開</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Public_Calendar_Expended(object sender, RoutedEventArgs e)
        {
            SetTreeCalendar(true);
        }


        /// <summary>プライベートカレンダーを展開</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Private_Calendar_Expended(object sender, RoutedEventArgs e)
        {
            SetTreeCalendar(false);
        }


        /// <summary>公開カレンダーを選択</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Public_Calendar_Selected(object sender, RoutedEventArgs e)
        {
            SetTreeCalendar(true);
        }


        /// <summary>プライベートカレンダーを選択</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Private_Calendar_Selected(object sender, RoutedEventArgs e)
        {
            SetTreeCalendar(false);
        }

        #endregion

        #region  private メッソド
        //*******************************************************************
        /// <summary>初期化</summary>
        //*******************************************************************
        private void LoadForInit(Container parantContainer)
        {
            // DAOの初期化 
            InitialDAO();

            // 空のテーブルを取得 
            SetTables();

            //　親Containerをセット 
            _parantContainer = parantContainer;

            // プロパティをセット 
            container.ParantWindow = this;

            btnRegist.IsEnabled = false;
            container.textBox_bootTimeHH.IsEnabled = false;
            container.textBox_bootTimeMI.IsEnabled = false;
            ShowCalendarDetail();
            container.btnLeft.IsEnabled = false;
            container.btnRight.IsEnabled = false;
        }

        //*******************************************************************
        /// <summary> DAOの初期化処理</summary>
        //*******************************************************************
        private void InitialDAO()
        {
            // カレンダー管理テーブル 
            _calendarControlDAO = new CalendarControlDAO(dbAccess);

            // カレンダー稼働日テーブル 
            _calendarDetailDAO = new CalendarDetailDAO(dbAccess);

        }

        //*******************************************************************
        /// <summary> 空テーブルをDataTableにセット(新規追加用)</summary>
        //*******************************************************************
        private void SetTables()
        {
            // カレンダー管理テーブル 
            dbAccess.CreateSqlConnect();

            // カレンダーテーブル 
            container.CalendarControlTable = _calendarControlDAO.GetEmptyTable();
            // カレンダー稼働日テーブル 
            container.CalendarDetailTable = _calendarDetailDAO.GetEmptyTable();

            dbAccess.CloseSqlConnect();
        }

        //*******************************************************************
        /// <summary> カレンダーデータの検索（編集、コピー新規用）</summary>
        /// <param name="calendarId">`カレンダーID</param>
        /// <param name="updDate">`更新日</param>
        //*******************************************************************
        private void FillTables(string calendarId)
        {

            // カレンダー管理テーブル 
            container.CalendarControlTable = _calendarControlDAO.GetValidORMaxUpdateDateEntityById(calendarId);


            // カレンダー稼働日テーブル 
            container.CalendarDetailTable = _calendarDetailDAO.GetEntityByCalendar(calendarId, container.CalendarControlTable.Rows[0]["update_date"]);
        }

        //*******************************************************************
        /// <summary>情報エリアをセット（編集、コピー新規用）</summary>
        //*******************************************************************
        private void SetInfoArea()
        {
            DataRow row = container.CalendarControlTable.Select()[0];
            // カレンダーIDをセット 
            this.lblCalendarId.Text = Convert.ToString(row["calendar_id"]);

            // カレンダー名をセット 
            lblCalendarName.Text = Convert.ToString(row["calendar_name"]);

            // 公開チェックボックス 
            int openFlg = Convert.ToInt16(row["public_flag"]);
            if (openFlg == 0)
                lblOpen.Text = "";
            else if (openFlg == 1)
                lblOpen.Text = "○";

            // 説明 
            lblComment.Text = Convert.ToString(row["memo"]);

            //更新日
            lblUpdDate.Text = (ConvertUtil.ConverIntYYYYMMDDHHMISS2Date(Convert.ToInt64(row["update_date"]))).ToString();
            //ユーザー名
            lblUserName.Text = Convert.ToString(row["user_name"]);
        }

        //*******************************************************************
        /// <summary>カレンダー稼働日の表示</summary>
        //*******************************************************************

        private void ShowCalendarDetail()
        {
            int year = DateTime.Now.Year;
            container.SetYearCalendarDetail(year.ToString());
        }

        /// <summary>カレンダーを展開</summary>
        /// <param name="public_flg">公開フラグ</param>
        public void SetTreeCalendar(bool public_flg)
        {
            DataTable dataTbl;
            DBConnect dbaccess = new DBConnect(LoginSetting.ConnectStr);
            dbaccess.CreateSqlConnect();

            int flg;
            TreeViewItem treeViewItem;
            if (public_flg)
            {
                flg = 1;
                treeViewItem = publicCalendar;
            }
            else
            {
                flg = 0;
                treeViewItem = privateCalendar;
            }


            string selectSql;
            if (public_flg)
            {
                selectSql = "select calendar_id, max(update_date) from ja_calendar_control_table where public_flag= " +
                                flg + " group by calendar_id order by calendar_id";
            }
            else
            {
                if (!(LoginSetting.Authority == Consts.AuthorityEnum.SUPER))
                {
                    selectSql = "SELECT distinct A.calendar_id,A.update_date "
                                                    + "FROM ja_calendar_control_table AS A,users AS U,users_groups AS UG1,users_groups AS UG2 "
                                                    + "WHERE A.user_name = U.alias and U.userid=UG1.userid and UG2.userid=" + LoginSetting.UserID
                                                    + " and UG1.usrgrpid = UG2.usrgrpid and "
                                                    + "A.update_date= "
                                                    + "( SELECT MAX(B.update_date) FROM ja_calendar_control_table AS B "
                                                    + "WHERE B.calendar_id = A.calendar_id group by B.calendar_id) and A.public_flag=0 order by A.calendar_id";
                }
                else
                {
                    selectSql = "select calendar_id, max(update_date) from ja_calendar_control_table where public_flag= " +
                                    flg + " group by calendar_id order by calendar_id";

                }
            }

            dataTbl = dbaccess.ExecuteQuery(selectSql);

            if (dataTbl != null)
            {
                treeViewItem.Items.Clear();
                foreach (DataRow row in dataTbl.Rows)
                {
                    TreeViewItem item = new TreeViewItem();
                    item.Header = row["calendar_id"].ToString();
                    item.Tag = Consts.ObjectEnum.JOBNET;
                    item.FontFamily = new FontFamily("MS Gothic");
                    treeViewItem.Items.Add(item);
                }
            }

            TreeViewItem itemCalendar;
            foreach (object item in treeViewItem.Items)
            {
                itemCalendar = (TreeViewItem)item;
                itemCalendar.Selected += Calendar_Selected;
            }

            dbaccess.CloseSqlConnect();
        }

        //*******************************************************************
        /// <summary>入力チェック </summary>
        /// <returns>チェック結果</returns>
        //*******************************************************************
        private bool InputCheck()
        {
            // 起動時刻時間を取得 
            string bootTimeHH = container.textBox_bootTimeHH.Text.Trim();

            String[] errItem = new String[2];
            errItem[0] = Properties.Resources.err_message_boot_time_hh;
            errItem[1] = "2";
            if (CheckUtil.IsNullOrEmpty(bootTimeHH))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, errItem);
                return false;
            }
            if (CheckUtil.IsLenOver(bootTimeHH, 2))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_003, errItem);
                return false;
            }
            if (!CheckUtil.IsHankakuNum(bootTimeHH))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, errItem);
                return false;
            }

            // 起動時刻分を取得 
            string bootTimeMI = container.textBox_bootTimeMI.Text.Trim();

            errItem[0] = Properties.Resources.err_message_boot_time_mi;
            errItem[1] = "2";
            if (CheckUtil.IsNullOrEmpty(bootTimeMI))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_001, errItem);
                return false;
            }
            if (!CheckUtil.IsLen(bootTimeMI, 2))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_012, errItem);
                return false;
            }
            if (!CheckUtil.IsHankakuNum(bootTimeMI))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_007, errItem);
                return false;
            }

            int iMI = int.Parse(bootTimeMI);

            if (iMI < 0 || iMI > 59)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_BOOT_TIME_001, errItem);
                return false;
            }

            return true;
        }

        #endregion

        private void container_Loaded(object sender, RoutedEventArgs e)
        {

        }
    }
}
