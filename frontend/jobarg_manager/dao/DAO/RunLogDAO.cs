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
//  * @author KIM 2013/06/05 新規作成<BR>　　　               　     *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.DAO
{
    /// <summary>
    /// ユーザーテーブルのDAOクラス
    /// </summary>
    public class RunLogDAO : BaseDAO
    {
        #region フィールド


        private string _tableName = "ja_run_log_table";

        private string[] _primaryKey = { "inner_jobnet_id" };

        private string _selectSql = "select * from ja_run_log_table where 0!=0";

        private string _selectSqlByPk = "select * from ja_run_log_table " +
                                        "where inner_jobnet_id = ? ";

        private String _select_run_result_super_manageid = "select JR.* from "
                                                + "ja_run_log_table AS JR "
                                                + "where JR.inner_jobnet_main_id = ? order by JR.inner_jobnet_main_id,JR.log_date";
        private String _select_run_result_super_from_to = "select JR.* from "
                                                + "ja_run_log_table AS JR "
                                                + "where JR.log_date >= ? and JR.log_date <= ? order by JR.inner_jobnet_main_id,JR.log_date";
        private String _select_run_result_super_from = "select JR.* from "
                                                + "ja_run_log_table AS JR "
                                                + "where JR.log_date >= ? order by JR.inner_jobnet_main_id,JR.log_date";

        private String _select_run_result_manageid = "select JRAll.* from ( (select JR.* from "
                                                + "ja_run_log_table AS JR "
                                                + "where JR.inner_jobnet_main_id = ? and JR.public_flag=1) "
                                                + "union "
                                                + "(select JR.* from "
                                                + "ja_run_log_table AS JR, users AS U, users_groups AS UG1, users_groups AS UG2 "
                                                + "where JR.inner_jobnet_main_id = ? and "
                                                + "JR.public_flag=0 and JR.user_name=U.alias and U.userid=UG1.userid and UG2.userid=? and UG1.usrgrpid=UG2.usrgrpid)) "
                                                + "as JRAll "
                                                + "order by JRAll.inner_jobnet_main_id,JRAll.log_date";

        private String _select_run_result_from_to = "select JRAll.* from ( (select JR.* from "
                                                + "ja_run_log_table AS JR "
                                                + "where JR.log_date >= ? and JR.log_date <= ? and JR.public_flag=1) "
                                                + "union "
                                                + "(select JR.* from "
                                                + "ja_run_log_table AS JR, users AS U, users_groups AS UG1, users_groups AS UG2 "
                                                + "where JR.log_date >= ? and JR.log_date <= ? and "
                                                + "JR.public_flag=0 and JR.user_name=U.alias and U.userid=UG1.userid and UG2.userid=? and UG1.usrgrpid=UG2.usrgrpid)) "
                                                + "as JRAll "
                                                + "order by JRAll.inner_jobnet_main_id,JRAll.log_date";

        private String _select_run_result_from = "select JRAll.* from ( (select JR.* from "
                                                + "ja_run_log_table AS JR "
                                                + "where JR.log_date >= ? and JR.public_flag=1) "
                                                + "union "
                                                + "(select JR.* from "
                                                + "ja_run_log_table AS JR, users AS U, users_groups AS UG1, users_groups AS UG2 "
                                                + "where JR.log_date >= ? and "
                                                + "JR.public_flag=0 and JR.user_name=U.alias and U.userid=UG1.userid and UG2.userid=? and UG1.usrgrpid=UG2.usrgrpid)) "
                                                + "as JRAll "
                                                + "order by JRAll.inner_jobnet_main_id,JRAll.log_date";




        private DBConnect _db = null;

        #endregion

        #region コンストラクタ

        public RunLogDAO(DBConnect db)
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
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "inner_jobnet_id", user_id));

            DataTable dt = _db.ExecuteQuery(this._selectSqlByPk, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// データの取得.
        /// </summary>
        /// <param name="manageId">管理ID</param>
        /// <param name="from">from日付</param>
        /// <param name="to">to日付</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntitySuper(object manageId, object from, object to)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            if (manageId != null)
                sqlParams.Add(new ComSqlParam(DbType.UInt64, "@inner_jobnet_main_id", manageId));
            if (from != null)
                sqlParams.Add(new ComSqlParam(DbType.UInt64, "@log_date", from));
            if (to != null)
                sqlParams.Add(new ComSqlParam(DbType.UInt64, "@log_date", to));

            DataTable dt;
            if (manageId != null)
            {
                dt = _db.ExecuteQuery(_select_run_result_super_manageid, sqlParams, TableName);
            }
            else if (to != null)
            {
                dt = _db.ExecuteQuery(_select_run_result_super_from_to, sqlParams, TableName);
            }
            else
            {
                dt = _db.ExecuteQuery(_select_run_result_super_from, sqlParams, TableName);
            }


            return dt;
        }


        //************************************************************************
        /// <summary> 
        /// データの取得.
        /// </summary>
        /// <param name="manageId">管理ID</param>
        /// <param name="from">from日付</param>
        /// <param name="to">to日付</param>
        /// <param name="userid">ユーザーＩＤ</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntity(object manageId, object from, object to, object userid)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            if (manageId != null)
                sqlParams.Add(new ComSqlParam(DbType.UInt64, "@inner_jobnet_main_id", manageId));
            if (from != null)
                sqlParams.Add(new ComSqlParam(DbType.UInt64, "@log_date", from));
            if (to != null)
                sqlParams.Add(new ComSqlParam(DbType.UInt64, "@log_date", to));
            if (manageId != null)
                sqlParams.Add(new ComSqlParam(DbType.UInt64, "@inner_jobnet_main_id", manageId));
            if (from != null)
                sqlParams.Add(new ComSqlParam(DbType.UInt64, "@log_date", from));
            if (to != null)
                sqlParams.Add(new ComSqlParam(DbType.UInt64, "@log_date", to));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@userid", userid));

            DataTable dt;
            if (manageId != null)
            {
                dt = _db.ExecuteQuery(_select_run_result_manageid, sqlParams, TableName);
            }
            else if (to != null)
            {
                dt = _db.ExecuteQuery(_select_run_result_from_to, sqlParams, TableName);

            }
            else
            {
                dt = _db.ExecuteQuery(_select_run_result_from, sqlParams, TableName);
            }

            return dt;
        }

        #endregion
    }
}
