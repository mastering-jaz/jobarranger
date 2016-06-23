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
    /// 例外の処理.
    /// </summary>
    public class BaseException:Exception
    {
        #region フィールド


        /// <summary>エラーメッセージ</summary>
        private string _strMessage = string.Empty;

        /// <summary>発生元プログラムID</summary>
        private string _strSource = string.Empty;

        /// <summary>メッセージID</summary>
        private string _strMessageID = string.Empty;

        #endregion

        #region プロパティ
        /// <summary>エラーメッセージ</summary>
        public override string Message
        {
            get
            {
                return _strMessage;
            }
        }

        /// <summary>発生元プログラムID</summary>
        public override string Source
        {
            get
            {
                return _strSource; 
            }
            set 
            { 
                _strSource = value; 
            }
        }

        /// <summary>メッセージID</summary>
        public string MessageID
        {
            get 
            {
                return _strMessageID; 
            }
            set 
            { 
                _strMessageID = value; 
            }
        }
      
        #endregion

        #region コンストラクタ

        /// <summary>コンストラクタ</summary>
        /// <param name="messageID">例外メッセージＩＤ</param>
        public BaseException(string messageID)
            : this(string.Empty, messageID, string.Empty, null) { }

        /// <summary>コンストラクタ</summary>
        /// <param name="messageID">例外メッセージＩＤ</param>
        /// <param name="ex">現在の例外の原因である例外。</param>
        public BaseException(string messageID, Exception ex)
            : this(string.Empty, messageID, string.Empty, ex) { }

        /// <summary>コンストラクタ</summary>
        /// <param name="source">発生元プログラムID</param>
        /// <param name="messageID">例外メッセージID</param>
        /// <param name="ex">現在の例外の原因である例外。</param>
        public BaseException(string source, string messageID, Exception ex)
            : this(source, messageID, string.Empty, ex) { }

        /// <summary>コンストラクタ</summary>
        /// <param name="source">発生元プログラムID</param>
        /// <param name="messageID">例外メッセージID</param>
        /// <param name="message">例外メッセージ</param>
        /// <param name="ex">現在の例外の原因である例外。</param>
        public BaseException(string source, string messageID, string message, Exception ex)
            : base(message, ex)
        {
            this._strSource = source;
            this._strMessageID = messageID;
            this._strMessage = message;
        }
		#endregion
       
    }
}
