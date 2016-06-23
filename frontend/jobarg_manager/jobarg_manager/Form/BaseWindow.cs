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
using System.Data;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using jp.co.ftf.jobcontroller.Common;
using jp.co.ftf.jobcontroller.DAO;

//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 郭 暁宇 2012/10/15 新規作成<BR>                    *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.JobController
{
    /// <summary>
    /// 画面の共通クラス.
    /// </summary>
    public abstract class BaseWindow : Window
    {

        #region フィールド

        /// <summary>クラス名</summary>
        public abstract string ClassName { get; }

        /// <summary>画面名 </summary>
        public abstract string GamenId { get; }

        #endregion

        #region コンストラクタ

        public BaseWindow()
        {
        }

        #endregion

        #region publicメソッド

        #region ログ出力の処理


        /// <summary>処理の開始ログを出力</summary>
        /// <param name="strMethodName">メソッド名</param>
        /// <param name="strSyoriId">処理ID</param>
        public void WriteStartLog(string strMethodName, string strSyoriId)
        {
            // デバッグログを出力

            LogInfo.WriteStartDebugLog(ClassName, strMethodName);

            // INFOログを出力

            LogInfo.WriteStartInfoLog(GamenId, strSyoriId);

        }


        /// <summary>処理の終了ログを出力</summary>
        /// <param name="strMethodName">メソッド名</param>
        /// <param name="strSyoriId">処理ID</param>
        public void WriteEndLog(string strMethodName, string strSyoriId)
        {
            // デバッグログを出力

            LogInfo.WriteEndDebugLog(ClassName, strMethodName);

            // INFOログを出力

            LogInfo.WriteEndInfoLog(GamenId, strSyoriId);

        }


        /// <summary>WARN用のログを出力</summary>
        /// <param name="strWarnMsgId">WarnメッセージID</param>
        public void WriteWarnLog(string strWarnMsgId)
        {
            LogInfo.WriteWarnLog(strWarnMsgId);
        }


        /// <summary>ERROR用のログを出力</summary>
        /// <param name="strErrorMsgId">エラーメッセージＩＤ</param>
        /// <param name="ex">例外情報</param>
        public void WriteErrorLog(string strErrorMsgId, Exception ex)
        {
            LogInfo.WriteErrorLog(strErrorMsgId, ex);
        }

        /// <summary>FATAL用のログを出力</summary>
        /// <param name="ex">例外情報</param>
        public void WriteFatalLog(Exception ex)
        {
            LogInfo.WriteFatalLog(ex);
        }

        #endregion

        #region Nvlの処理

        public string NvlToStr(object obj)
        {
            if (Convert.IsDBNull(obj) || obj == null || obj.ToString().Length == 0)
            {
                return "";
            }
            else
            {
                return obj.ToString();
            }
        }

        public string NvlToDate(object obj)
        {
            if (Convert.IsDBNull(obj) || obj == null || obj.ToString().Length == 0)
            {
                return "00000000";
            }
            else
            {
                return obj.ToString();
            }
        }

        #endregion

        #endregion
    }
}
