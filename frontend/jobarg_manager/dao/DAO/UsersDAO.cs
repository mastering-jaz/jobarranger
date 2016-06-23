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
    /// ユーザーテーブルのDAOクラス
    /// </summary>
    public class UsersDAO : BaseDAO
    {
        #region フィールド


        private string _tableName = "users";

        private string[] _primaryKey = { "user_id" };

        private string _selectSql = "select * from users where 0!=0";

        private string _selectSqlByPk = "select * from users " +
                                        "where user_id = ? ";

        private string _selectSqlByNameAndPass = "select * from users " +
                        "where alias = ? and passwd = ?";

        private string _selectAllSql = "select * from users order by alias";

        private string _selectGroupUserSql = "select * from users U where " +
                        "U.userid in (select userid from users_groups where " +
                        "usrgrpid in (select usrgrpid from users_groups where userid=?)) " +
                        "order by alias";

        private DBConnect _db = null;

        #endregion

        #region コンストラクタ

        public UsersDAO(DBConnect db)
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
        /// <param name="user_id">ユーザーＩＤ</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByPk(object user_id)
        {
            _db.CreateSqlConnect();

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "user_id", user_id));

            DataTable dt = _db.ExecuteQuery(this._selectSqlByPk, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// データの取得.
        /// </summary>
        /// <param name="name">ユーザー名</param>
        /// <param name="pass">パスワード</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByNameAndPass(object name, object pass)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@alias", name));
            sqlParams.Add(new ComSqlParam(DbType.String, "@passwd", pass));

            DataTable dt = _db.ExecuteQuery(_selectSqlByNameAndPass, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// 全データの取得.
        /// </summary>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetAllEntity()
        {

            DataTable dt = _db.ExecuteQuery(_selectAllSql, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// 全グループデータの取得.
        /// </summary>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetGroupUser(object userid)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@userid", userid));
            DataTable dt = _db.ExecuteQuery(_selectGroupUserSql, sqlParams, TableName);

            return dt;
        }

        #endregion
    }
}
