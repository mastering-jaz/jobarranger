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
    /// メッセージの処理クラス
    /// </summary>
    public class MessageUtil
    {
        #region publicメソッド

        //************************************************************************
        /// <summary>
        /// メッセージの置換処理

        /// </summary>
        /// <param name="BaseMessage">変換対象を含む文字列</param>
        /// <param name="ChangeMessage">置換文字列</param>
        /// <returns>変換後の文字列</returns>
        //************************************************************************
        public static string GetReplaceMessage(string BaseMessage, string[] ChangeMessage)
        {
            return string.Format(BaseMessage, ChangeMessage);
        }

        //************************************************************************
        /// <summary> 
        /// メッセージの取得

        /// </summary>
        /// <param name="msgId">情報ＩＤ</param>
        /// <return>メッセージの内容</return>
        //************************************************************************
        public static string GetMsgById(string msgId)
        {
            return Properties.Resources.ResourceManager.GetString(msgId, Properties.Resources.Culture);
        }

        #endregion
    }
}
