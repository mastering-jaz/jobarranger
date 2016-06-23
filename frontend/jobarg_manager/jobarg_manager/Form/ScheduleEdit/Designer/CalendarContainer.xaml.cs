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
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Data;
using System.Collections;
using System.Windows.Forms;
using jp.co.ftf.jobcontroller.Common;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author KIM 2012/11/14 新規作成<BR>                            *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.ScheduleEdit
{
/// <summary>
/// Container.xaml の相互作用ロジック
/// </summary>
    public partial class CalendarContainer : System.Windows.Controls.UserControl
    {
        #region フィールド

        private CustomControls.MonthCalendar monthCalendar;

        #endregion

        #region コンストラクタ
        public CalendarContainer()
        {
            // 初期化 
            InitializeComponent();
            monthCalendar = new CustomControls.MonthCalendar();
            monthCalendar.CalendarDimensions = new System.Drawing.Size(4, 3);
            monthCalendar.Location = new System.Drawing.Point(0, 25);
            monthCalendar.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            monthCalendar.ViewStart = new DateTime(DateTime.Now.Year, 1, 1);
            this.winForm.Child = monthCalendar;
        }
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

        /// <summary>カレンダーID</summary>
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
        private void left_arrow_click(object sender, EventArgs e)
        {
            //YearCalendarDetailTable.Clear();
            String year = (Convert.ToInt16(textBox_year.Text) - 1).ToString();
            textBox_year.Text = year;
            monthCalendar.Enabled = true;
            monthCalendar.ViewStart = new DateTime(GetCurrentYear(), 1, 1);
            monthCalendar.Enabled = false;
            
        }

        private void right_arrow_click(object sender, EventArgs e)
        {
            //YearCalendarDetailTable.Clear();
            String year = (Convert.ToInt16(textBox_year.Text) + 1).ToString();
            textBox_year.Text = year;
            monthCalendar.Enabled = true;
            monthCalendar.ViewStart = new DateTime(GetCurrentYear(), 1, 1);
            monthCalendar.Enabled = false;

        }


        #endregion

        #region publicメッソド
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
            monthCalendar.Enabled = true;
            monthCalendar.SelectionRanges.Clear();
            monthCalendar.ViewStart = new DateTime(GetCurrentYear(), 1, 1);
            DataRow[] rows = CalendarDetailTable.Select();
            SetSelectedDates(rows);
            DataRow[] maxRows = CalendarDetailTable.Select("operating_date = MAX(operating_date)");
            last_operation_day_value.Text = "";
            if (maxRows.Length > 0)
            {
                last_operation_day_value.Text = ConvertUtil.ConverIntYYYYMMDD2Date((Int32)maxRows[0]["operating_date"]).ToShortDateString();
            }
            monthCalendar.Enabled = false;

        }

        #endregion

        #region privateメッソド

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
