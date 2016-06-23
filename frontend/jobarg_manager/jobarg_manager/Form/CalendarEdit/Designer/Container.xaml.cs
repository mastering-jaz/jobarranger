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
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Data;
using System.Collections;
using jp.co.ftf.jobcontroller.Common;
using CustomControls;
using System.Windows.Forms.Integration;
using System.Windows.Forms;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 偉 2012/10/04 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.CalendarEdit
{
    /// <summary>
    /// Container.xaml の相互作用ロジック
    /// </summary>
    public partial class Container : System.Windows.Controls.UserControl
    {
        #region コンストラクタ
        public Container()
        {
            // 初期化 
            InitializeComponent();

            monthCalendar = new CustomControls.MonthCalendar();
            monthCalendar.CalendarDimensions = new System.Drawing.Size(4, 3);
            monthCalendar.Location = new System.Drawing.Point(0, 25);
            monthCalendar.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            monthCalendar.ViewStart = new DateTime(DateTime.Now.Year, 1, 1);
            monthCalendar.SelectionRanges.Clear();
            monthCalendar.DateSelected += new System.EventHandler<System.Windows.Forms.DateRangeEventArgs>(monthCalendarDate_Select);
            this.winForm.Child = monthCalendar;
        }
        #endregion

        #region フィールド

        private CustomControls.MonthCalendar monthCalendar;

        #endregion

        #region プロパティ

        /// <summary>ウィンドウ</summary>
        ContentControl _parantWindow;
        public ContentControl ParantWindow
        {
            get
            {
                return _parantWindow;
            }
            set
            {
                _parantWindow = value;
            }
        }


        /// <summary> 選択コントローラリスト</summary>
        List<System.Windows.Controls.Control> _currentSelectedControlCollection;
        public List<System.Windows.Controls.Control> CurrentSelectedControlCollection
        {
            get
            {
                if (_currentSelectedControlCollection == null)
                    _currentSelectedControlCollection = new List<System.Windows.Controls.Control>();
                return _currentSelectedControlCollection;
            }
        }

        /// <summary>ジョブネットID</summary>
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

        /// <summary>更新日</summary>
        private string _updDate;
        public string UpdDate
        {
            get
            {
                return _updDate;
            }
            set
            {
                _updDate = value;
            }
        }

        /// <summary>仮更新日</summary>
        private string _tmpUpdDate;
        public string TmpUpdDate
        {
            get
            {
                return _tmpUpdDate;
            }
            set
            {
                _tmpUpdDate = value;
            }
        }

        #endregion

        #region データ格納場所

        /// <summary>カレンダー管理テーブル</summary>
        public DataTable CalendarControlTable { get; set; }

        /// <summary>カレンダー稼働日テーブル</summary>
        public DataTable CalendarDetailTable { get; set; }

        /// <summary>年毎カレンダー稼働日テーブル</summary>
        public DataTable YearCalendarDetailTable { get; set; }


        #endregion

        #region イベント

        //*******************************************************************
        /// <summary>カレンダー選択された日付が変更された時</summary>
        /// <param name="sender">源</param>
        /// <param name="e">マウスイベント</param>
        //*******************************************************************
        private void monthCalendarDate_Select(object sender, DateRangeEventArgs e)
        {
            for (DateTime operation_date = e.Start; operation_date <= e.End; operation_date = operation_date.AddDays(1))
            {
                decimal operating_date = ConvertUtil.ConverDate2IntYYYYMMDD(operation_date);
                DataRow[] rows = CalendarDetailTable.Select("operating_date='" + operating_date + "'");
                // カレンダー稼働日テーブルから削除 
                if (rows != null && rows.Count() > 0)
                {
                    rows[0].Delete();
                }
                else
                {
                    DataRow row = CalendarDetailTable.NewRow();
                    //ジョブネットID 
                    row["calendar_id"] = _calendarId;
                    // 稼働日
                    row["operating_date"] = operating_date;
                    // 更新日 
                    row["update_date"] = _tmpUpdDate;

                    CalendarDetailTable.Rows.Add(row);
                }

            }
        }

        //*******************************************************************
        /// <summary>カレンダー年右矢印ボタンクリック時</summary>
        /// <param name="sender">源</param>
        /// <param name="e">マウスイベント</param>
        //*******************************************************************
        private void left_arrow_click(object sender, EventArgs e)
        {
            String year = (Convert.ToInt16(textBox_year.Text) - 1).ToString();
            ViewYearCalendarDetail(year);

        }

        //*******************************************************************
        /// <summary>カレンダー年左矢印ボタンクリック時</summary>
        /// <param name="sender">源</param>
        /// <param name="e">マウスイベント</param>
        //*******************************************************************
        private void right_arrow_click(object sender, EventArgs e)
        {
            String year = (Convert.ToInt16(textBox_year.Text) + 1).ToString();
            ViewYearCalendarDetail(year);
        }


        #endregion

        #region publicメソッド

        //*******************************************************************
        /// <summary>初回、矢印遷移の場合稼働日セット</summary>
        /// <param name="year">年</param>
        //*******************************************************************
        public void SetYearCalendarDetail(String year)
        {
            if (year != null)
            {
                textBox_year.Text = year;
            }
            else
            {
                year = textBox_year.Text;
            }

            DataRow[] rows = CalendarDetailTable.Select();
            monthCalendar.ViewStart = new DateTime(GetCurrentYear(), 1, 1);
            monthCalendar.SelectionRanges.Clear();
            SetSelectedDates(rows);
            DataRow[] maxRows = CalendarDetailTable.Select("operating_date = MAX(operating_date)");
            if (maxRows.Length > 0)
                last_operation_day_value.Text = ConvertUtil.ConverIntYYYYMMDD2Date((Int32)maxRows[0]["operating_date"]).ToShortDateString();
            this.winForm.Child = monthCalendar;
        }


        #endregion

        #region privateメソッド
        //*******************************************************************
        /// <summary>初回、矢印遷移の場合稼働日セット</summary>
        /// <param name="year">年</param>
        //*******************************************************************
        private void ViewYearCalendarDetail(String year)
        {
            textBox_year.Text = year;
            monthCalendar.ViewStart = new DateTime(GetCurrentYear(), 1, 1);
            if (monthCalendar.Enabled == false)
            {
                monthCalendar.Enabled = true;
                monthCalendar.Enabled = false;
            }
        }
        //*******************************************************************
        /// <summary>カレンダー選択日付をセット</summary>
        /// <param name="rows">カレンダー詳細データ</param>
        //*******************************************************************
        private void SetSelectedDates(DataRow[] rows)
        {
            foreach (DataRow row in rows)
            {
                DateTime date = ConvertUtil.ConverIntYYYYMMDD2Date(Convert.ToDecimal(row["operating_date"]));
                SetSelectDate(date);
            }
        }

        //*******************************************************************
        /// <summary>カレンダー選択日付をセット</summary>
        /// <param name="date">カレンダー稼動日</param>
        //*******************************************************************
        private void SetSelectDate(DateTime date)
        {
            SelectionRange selectionRange = new SelectionRange(date, date);
            monthCalendar.SelectionRanges.Add(selectionRange);
        }

        //*******************************************************************
        /// <summary>カレンダー選択日付をセット</summary>
        /// <return>現在表示年</return>
        //*******************************************************************
        private int GetCurrentYear()
        {
            return Convert.ToInt32(textBox_year.Text);
        }

        #endregion
    }

}
