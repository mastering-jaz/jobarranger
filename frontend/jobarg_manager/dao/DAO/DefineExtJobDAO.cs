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
using System.Data;
using System.Text;
using jp.co.ftf.jobcontroller.Common;

//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 偉 2012/10/19 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.DAO
{
    /// <summary>
    /// 拡張ジョブ定義テーブルのDAOクラス
    /// </summary>
    public class DefineExtJobDAO : BaseDAO
    {
        #region フィールド


        private string _tableName = "ja_define_extjob_table";

        private string[] _primaryKey = { "command_id" };

        private string _selectSql = "select * from ja_define_extjob_table where 0!=0";

        private string _selectAllSql = "select command_id, command_name, memo from ja_define_extjob_table " +
                                       "order by command_id ASC";

        private string _selectSqlByPk = "select * from ja_define_extjob_table " +
                                        "where command_id = ? ";

        private string _selectLangSql = "select command_id, command_name, memo from ja_define_extjob_table where lang = ?" +
                               "order by command_id ASC";

        private DBConnect _db = null;

        #endregion

        #region コンストラクタ

        public DefineExtJobDAO(DBConnect db)
        {
            _db = db;
        }

        #endregion

        #region プロパティ

        /// <summary>　テーブル名前 </summary>
        public override string TableName
        {
            get
            {
                return _tableName;
            }
        }

        /// <summary>　キー </summary>
        public override string[] PrimaryKey
        {
            get
            {
                return _primaryKey;
            }
        }

        /// <summary> 検索用のSQL </summary>
        public override string SelectSql
        {
            get
            {
                return _selectSql;
            }
        }

        /// <summary> 検索条件を指定したSQL文 </summary>
        public override string SelectSqlByPk
        {
            get
            {
                return _selectSqlByPk;
            }
        }

        #endregion

        #region publicメソッド

        //************************************************************************
        /// <summary> テーブルの構築</summary>
        /// <return>テーブルの結構</return>
        //************************************************************************
        public DataTable GetEmptyTable()
        {
            _db.CreateSqlConnect();

            DataTable dt = _db.ExecuteQuery(SelectSql);

            return dt;

        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByNone()
        {
            _db.CreateSqlConnect();

            DataTable dt = _db.ExecuteQuery(this._selectAllSql, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="command_id">コマンドID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByPk(object command_id)
        {
            _db.CreateSqlConnect();

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@command_id", command_id));

            DataTable dt = _db.ExecuteQuery(this._selectSqlByPk, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="lang">言語</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByLang(object lang)
        {
            _db.CreateSqlConnect();

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@lang", lang));
            DataTable dt = _db.ExecuteQuery(this._selectLangSql, sqlParams, TableName);

            return dt;
        }

        #endregion
    }
}
