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
using System.Data;
using System.Text;
using System.Data.Common;

//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 郭 暁宇 2012/10/15 新規作成<BR>                    *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.DAO
{
    /// <summary>
    /// SQLへのパラメータについてのクラス.
    /// </summary>
    public class ComSqlParam
    {
        #region フィールド

        /// <summary>パラメータの型</summary>
        private DbType _paramType ;

        /// <summary>パラメータの入力、出力</summary>
        private ParameterDirection _paramDirection;

        /// <summary>パラメータの名前</summary>
        private string _paramName;

        /// <summary>パラメータの値</summary>
        private object _paramValue;

        #endregion

        #region プロパティ

        /// <summary>パラメータの型</summary>
        public DbType ParamType
        {
            get
            {
                return _paramType;
            }
            set
            {
                _paramType = value;
            }
        }

        // <summary>パラメータの入力、出力</summary>
        public ParameterDirection ParamDirection
        {
            get
            {
                return _paramDirection;
            }
            set
            {
                _paramDirection = value;
            }
        }

        /// <summary>パラメータの名前</summary>
        public string ParamName
        {
            get
            {
                return _paramName;
            }
            set
            {
                _paramName = value;
            }
        }

        /// <summary>パラメータの値</summary>
        public object ParamValue
        {
            get
            {
                return _paramValue;
            }
            set
            {
                _paramValue = value;
            }
        }

        #endregion

        #region コンストラクタ

        /// <summary>コンストラクタ</summary>
        /// <param name="paramType">パラメータの型</param>
        /// <param name="paramName">パラメータの名前</param>
        /// <param name="paramValue">パラメータの値</param>
        public ComSqlParam(DbType paramType, string paramName, object paramValue)
        {
            _paramType = paramType;
            _paramName = paramName;
            _paramValue = paramValue;
        }

        /// <summary>コンストラクタ</summary>
        /// <param name="paramType">パラメータの型</param>
        /// <param name="paramDirection">パラメータの入力、出力</param>
        /// <param name="paramName">パラメータの名前</param>
        /// <param name="paramValue">パラメータの値</param>
        public ComSqlParam(DbType paramType, ParameterDirection paramDirection, 
                           string paramName, object paramValue)
        {
            _paramType = paramType;
            _paramDirection = paramDirection;
            _paramName = paramName;
            _paramValue = paramValue;
        }

        #endregion
     
    }
}
