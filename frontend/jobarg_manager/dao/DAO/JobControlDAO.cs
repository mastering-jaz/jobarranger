﻿/*
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
//  * @author DHC 郭 暁宇 2012/10/15 新規作成<BR>                    *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.DAO
{
    /// <summary>
    /// ジョブ管理テーブルのDAO
    /// </summary>
    public class JobControlDAO : BaseDAO
    {
        #region フィールド



        private string _tableName = "ja_job_control_table";

        private string[] _primaryKey = { "jobnet_id", "job_id", "update_date" };

        private string _selectSql = "select * from ja_job_control_table where 0!=0";

        private string _selectSqlByPk = "select * from ja_job_control_table " +
            "where jobnet_id = ? and job_id = ? and update_date = ? ";

        private string _selectSqlBynetJobId = "select * from ja_job_control_table " +
            "where jobnet_id = ? and update_date = ?";

        private DBConnect _db = null;

        #endregion

        #region コンストラクタ

        public JobControlDAO(DBConnect db)
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
            DataTable dt = _db.ExecuteQuery(SelectSql);

            return dt;

        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <param name="job_id">ジョブID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByPk(object jobnet_id, object job_id, object update_date)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            sqlParams.Add(new ComSqlParam(DbType.String, "@job_id", job_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));

            DataTable dt = _db.ExecuteQuery(this._selectSqlByPk, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// データの取得
        /// </summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <param name="job_id">ジョブID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByJobIdForUpdate(object jobnet_id, object update_date)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));

            DataTable dt = _db.ExecuteQuery(this._selectSqlBynetJobId, sqlParams, TableName);

            return dt;
        }

        #endregion
    }
}
