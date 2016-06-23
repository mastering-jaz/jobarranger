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
using System.Windows.Media;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 偉 2012/10/04 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.JobEdit
{

    /// <summary>
    /// ジョブ編集画面用の定数クラス
    /// </summary>
    public class SystemConst
    {
        #region 先頭文字数

        /// <summary>ジョブ(ID)</summary>
        public static readonly int LEN_JOBID_JOB = 13;

        /// <summary>ジョブ(名)</summary>
        public static readonly int LEN_JOBNAME_JOB = 13;

        /// <summary>条件(ID)</summary>
        public static readonly int LEN_JOBID_IF = 13;

        /// <summary>条件(名)</summary>
        public static readonly int LEN_JOBNAME_IF = 13;

        /// <summary>変数(ID)</summary>
        public static readonly int LEN_JOBID_ENV = 13;

        /// <summary>変数(名)</summary>
        public static readonly int LEN_JOBNAME_ENV = 13;

        /// <summary>拡張ジョブ(ID)</summary>
        public static readonly int LEN_JOBID_EXTJOB = 13;

        /// <summary>拡張ジョブ(名)</summary>
        public static readonly int LEN_JOBNAME_EXTJOB = 13;

        /// <summary>開始(ID)</summary>
        public static readonly int LEN_JOBID_START = 13;

        /// <summary>開始(名)</summary>
        public static readonly int LEN_JOBNAME_START = 13;

        /// <summary>終了(ID)</summary>
        public static readonly int LEN_JOBID_END = 13;

        /// <summary>終了(名)</summary>
        public static readonly int LEN_JOBNAME_END = 13;

        /// <summary>計算(ID)</summary>
        public static readonly int LEN_JOBID_CAL = 13;

        /// <summary>計算(名)</summary>
        public static readonly int LEN_JOBNAME_CAL = 13;

        /// <summary>タスク(ID)</summary>
        public static readonly int LEN_JOBID_TASK = 13;

        /// <summary>タスク(名)</summary>
        public static readonly int LEN_JOBNAME_TASK = 13;

        /// <summary>情報取得(ID)</summary>
        public static readonly int LEN_JOBID_INF = 13;

        /// <summary>情報取得(名)</summary>
        public static readonly int LEN_JOBNAME_INF = 13;

        /// <summary>ジョブネット(ID)</summary>
        public static readonly int LEN_JOBID_JOBNET = 13;

        /// <summary>ジョブネット(名)</summary>
        public static readonly int LEN_JOBNAME_JOBNET = 13;

        /// <summary>ファイル転送(ID)</summary>
        public static readonly int LEN_JOBID_FCOPY = 13;

        /// <summary>ファイル転送(名)</summary>
        public static readonly int LEN_JOBNAME_FCOPY = 13;

        /// <summary>ファイル待ち合わせ((ID)</summary>
        public static readonly int LEN_JOBID_FWAIT = 13;

        /// <summary>ファイル待ち合わせ(名)</summary>
        public static readonly int LEN_JOBNAME_FWAIT = 13;

        /// <summary>リブート(ID)</summary>
        public static readonly int LEN_JOBID_REBOOT = 13;

        /// <summary>リブート(名)</summary>
        public static readonly int LEN_JOBNAME_REBOOT = 13;

        /// <summary>保留解除(ID)</summary>
        public static readonly int LEN_JOBID_RELEASE = 13;

        /// <summary>保留解除(名)</summary>
        public static readonly int LEN_JOBNAME_RELEASE = 13;

        /// <summary>アイコン表示用の接尾辞</summary>
        public static readonly string SUFFIX_JOB = "...";

        #endregion

        /// <summary>
        /// 色 
        /// </summary>
        public class ColorConst
        {
            /// <summary>選択状態の色</summary>
            public static Brush SelectedColor
            {
                get
                {
                    SolidColorBrush brush = new SolidColorBrush();
                    brush.Color = Color.FromArgb(255, 0, 102, 204);
                    return brush;
                }
            }

            /// <summary>開始アイコンの初期色</summary>
            public static Brush StartColor
            {
                get
                {
                    SolidColorBrush brush = new SolidColorBrush();
                    brush.Color = Color.FromArgb(255, 204, 236, 255);
                    return brush;
                }
            }

            /// <summary>終了アイコンの初期色</summary>
            public static Brush EndColor
            {
                get
                {
                    SolidColorBrush brush = new SolidColorBrush();
                    brush.Color = Color.FromArgb(255, 204, 236, 255);
                    return brush;
                }
            }

            /// <summary>ジョブアイコンの初期色</summary>
            public static Brush JobColor
            {
                get
                {
                    SolidColorBrush brush = new SolidColorBrush();
                    brush.Color = Color.FromArgb(255, 204, 204, 255);
                    return brush;
                }
            }

            /// <summary>部品区のアイコンの初期色</summary>
            public static Brush SampleColor
            {
                get
                {
                    SolidColorBrush brush = new SolidColorBrush();
                    brush.Color = Color.FromArgb(255, 255, 255, 255);
                    return brush;
                }
            }

            /// <summary>黒</summary>
            public static Brush BlackColor
            {
                get
                {
                    SolidColorBrush brush = new SolidColorBrush();
                    brush.Color = Color.FromArgb(255, 0, 0, 0);
                    return brush;
                }
            }
            /// <summary>保留色</summary>
            public static Brush HoldColor
            {
                get
                {
                    SolidColorBrush brush = new SolidColorBrush(Colors.MediumOrchid);
                    return brush;
                }
            }
            /// <summary>スキップ色</summary>
            public static Brush SkipColor
            {
                get
                {
                    SolidColorBrush brush = new SolidColorBrush(Colors.Gray);
                    return brush;
                }
            }            
        }
    }
}
