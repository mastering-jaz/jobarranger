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

//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 郭 暁宇 2012/10/15 新規作成<BR>                    *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.Common
{
    /// <summary>
    /// ファイル処理に関する例外の処理

    /// </summary>
    public class FileException : BaseException
    {
        #region コンストラクタ

        /// <summary>コンストラクタ</summary>
        /// <param name="messageID">例外メッセージＩＤ</param>
        public FileException(string messageID)
            : base(messageID)
        {
        }

        /// <summary>コンストラクタ</summary>
        /// <param name="messageID">例外メッセージＩＤ</param>
        /// <param name="ex">現在の例外の原因である例外。</param>
        public FileException(string messageID, Exception ex)
            : base(messageID, ex)
        {
        }

        /// <summary>コンストラクタ</summary>
        /// <param name="source">発生元プログラムID</param>
        /// <param name="messageID">例外メッセージＩＤ</param>
        /// <param name="ex">現在の例外の原因である例外。</param>
        public FileException(string source, string messageID, Exception ex)
            : base(source, messageID, ex) 
        {
        }

        /// <summary>コンストラクタ</summary>
        /// <param name="source">発生元プログラムID</param>
        /// <param name="messageID">例外メッセージＩＤ</param>
        /// <param name="message">例外メッセージ</param>
        /// <param name="ex">現在の例外の原因である例外。</param>
        public FileException(string source, string messageID, string message, Exception ex)
            : base(source, messageID, message, ex)
        {
        }

        #endregion
    }
}
