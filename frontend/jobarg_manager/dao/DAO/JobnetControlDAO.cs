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
//  * @author DHC 郭 暁宇 2012/10/15 新規作成<BR>                    *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.DAO
{
    /// <summary>
    /// ジョブネット管理テーブルのDAO
    /// </summary>
    public class JobnetControlDAO : ObjectBaseDAO
    {
        #region フィールド

        private string _tableName = "ja_jobnet_control_table";

        private string[] _primaryKey = { "jobnet_id", "update_date" };

        private string _selectSql = "select * from ja_jobnet_control_table where 0!=0";

        /// <summary>ロック </summary>
        private string _selectCountWithLock = "select * from ja_jobnet_control_table " +
            "where jobnet_id = ? and update_date = ? for update nowait ";

        /// <summary>ロックしない </summary>
        private string _selectCountNoLock = "select count(1) as count from ja_jobnet_control_table " +
            "where jobnet_id = ? and update_date = ?";

        private string _selectCountByJobNetIdSql = "select count(1) as count from ja_jobnet_control_table " +
            "where jobnet_id = ?";

        private string _selectSqlForJobInfo = "select jobnet_id, MAX(update_date) as update_date, jobnet_name from " +
            "ja_jobnet_control_table where jobnet_id = ?";

        private string _selectValidByJobnetIdSql = "select * from ja_jobnet_control_table " +
             "where jobnet_id = ? and valid_flag = '1'";

        private string _selectSqlByUserId = "SELECT jobnet.jobnet_id, jobnet.jobnet_name, jobnet.update_date " +
                                                "FROM ( "+
                                                "("+
                                                "SELECT C.jobnet_id, C.jobnet_name, C.update_date "+
                                                "FROM ja_jobnet_control_table as C "+
                                                "WHERE C.public_flag =1 "+
                                                "and C.update_date="+
                                                "( SELECT MAX(D.update_date) "+
                                                "FROM ja_jobnet_control_table AS D "+
                                                "WHERE D.jobnet_id = C.jobnet_id) " +
                                                ") "+
                                                "UNION ("+
                                                "SELECT A.jobnet_id, A.jobnet_name, A.update_date "+
                                                "FROM ja_jobnet_control_table AS A, users AS U, users_groups AS UG1, users_groups AS UG2 "+
                                                "WHERE A.user_name = U.alias "+
                                                "AND U.userid = UG1.userid "+
                                                "AND UG2.userid=? "+
                                                "AND UG1.usrgrpid = UG2.usrgrpid "+
                                                "AND A.update_date = ( "+
                                                "SELECT MAX( B.update_date ) "+
                                                "FROM ja_jobnet_control_table AS B "+
                                                "WHERE B.jobnet_id = A.jobnet_id "+
                                                "GROUP BY B.jobnet_id ) "+
                                                "AND A.public_flag =0"+
                                                ")"+
                                                ") AS jobnet "+
                                                "ORDER BY jobnet.jobnet_id ";
        private string _selectSqlByUserIdSuper = "SELECT A.jobnet_id,A.jobnet_name,A.update_date "+
                                                "FROM ja_jobnet_control_table AS A "+
                                                "WHERE A.update_date="+
                                                "( SELECT MAX(B.update_date) "+
                                                "FROM ja_jobnet_control_table AS B "+
                                                "WHERE A.jobnet_id = B.jobnet_id) "+
                                                "order by A.jobnet_id";

        private string _selectSqlByPk = "select * from ja_jobnet_control_table " +
            "where jobnet_id = ? and update_date = ? ";

        private string _selectSqlByJobnetId = "select * from ja_jobnet_control_table " +
            "where jobnet_id = ? order by update_date desc";

        /// <summary>ジョブネットIDの重複登録チェック用のSQL </summary>
        private string _selectSqlForCheck = "select count(1) as count from ja_jobnet_control_table " +
             "where jobnet_id = ?";

        /// <summary>ジョブネットIDの最新データ取得用のSQL </summary>
        private string _selectMaxUpdateDateByJobNetIdSql = "select * from ja_jobnet_control_table " +
             "where jobnet_id = ? and update_date = (select max(update_date) from ja_jobnet_control_table where jobnet_id=?)";

        private string _deleteSqlByPk = "delete from ja_jobnet_control_table " +
              "where jobnet_id = ? and update_date = ? ";

        private DBConnect _db = null;

        #endregion

        #region コンストラクタ

        public JobnetControlDAO(DBConnect db)
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
        /// <summary> 
        /// テーブルの構築.
        /// </summary>
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
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        //public string GetCountByPkWithLock(object jobnet_id, object update_date)
        //{
        //    string count = "";

        //    List<ComSqlParam> sqlParams = new List<ComSqlParam>();
        //    sqlParams.Add(new ComSqlParam(DbType.String, "jobnet_id", jobnet_id));
        //    sqlParams.Add(new ComSqlParam(DbType.Int64, "update_date", update_date));
        //    DataTable dt = _db.ExecuteQuery(_selectCountWithLock, sqlParams, TableName);

        //    if (dt != null)
        //        count = Convert.ToString(dt.Rows[0]["count"]);

        //    return count;
        //}

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetCountByPk(object jobnet_id, object update_date)
        {
            string count = "";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_selectCountNoLock, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得(For Update)</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetCountByPkForUpdate(object jobnet_id, object update_date)
        {
            string count = "";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_selectCountWithLock, sqlParams, TableName);

            // データ有の場合

            if (dt != null && dt.Rows.Count > 0)
                return "1";

            return count;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetLockByPk(object jobnet_id, object update_date)
        {
            string count = "";

            string _getLock = "SELECT GET_LOCK('ja_jobnet_control_table."+ jobnet_id +
                 "." + update_date + "', 0) as count";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_getLock, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string RealseLockByPk(object jobnet_id, object update_date)
        {
            string count = "";

            string _releaseLock = "SELECT RELEASE_LOCK('ja_jobnet_control_table." + jobnet_id +
                "." + update_date + "') as count";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_releaseLock, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetCountByJobNetId(object jobnet_id)
        {
            string count = "";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            DataTable dt = _db.ExecuteQuery(_selectCountByJobNetIdSql, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得(ジョブネットIDの重複チェック用)</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetCountForCheck(object jobnet_id)
        {
            string count = "";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            DataTable dt = _db.ExecuteQuery(_selectSqlForCheck, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="userid">ユーザーID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetInfoByUserId(object userid)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@userid", userid));
            DataTable dt = _db.ExecuteQuery(_selectSqlByUserId, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="userid">ユーザーID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetInfoByUserIdSuper()
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();

            DataTable dt = _db.ExecuteQuery(_selectSqlByUserIdSuper, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetInfoForJobInfo(object jobnet_id)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            DataTable dt = _db.ExecuteQuery(_selectSqlForJobInfo, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByPk(object jobnet_id, object update_date)
        {

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_selectSqlByPk, sqlParams, TableName);

            return dt;

        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByJobnetId(object jobnet_id)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            DataTable dt = _db.ExecuteQuery(_selectSqlByJobnetId, sqlParams, TableName);

            return dt;

        }

        //************************************************************************
        /// <summary> 
        /// データを削除.
        /// </summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public int DeleteByPk(object jobnet_id, object update_date)
        {

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            int count = _db.ExecuteNonQuery(_deleteSqlByPk, sqlParams);

            return count;

        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public override DataTable GetEntityByObjectId(object jobnet_id)
        {
            DataTable dt = GetEntityByJobnetId(jobnet_id);

            return dt;
        }

        //************************************************************************
        /// <summary> 有効データの取得</summary>
        /// <param name="calendar_id">ジョブネットID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetValidEntityById(object jobnet_id)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            DataTable dt = _db.ExecuteQuery(_selectValidByJobnetIdSql, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetMaxUpdateDateEntityById(object jobnet_id)
        {

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            sqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnet_id));
            DataTable dt = _db.ExecuteQuery(_selectMaxUpdateDateByJobNetIdSql, sqlParams, TableName);

            return dt;

        }

        //************************************************************************
        /// <summary> 有効データが存在する場合有効データ、存在しない場合最新データの取得</summary>
        /// <param name="jobnet_id">ジョブネットID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetValidORMaxUpdateDateEntityById(object jobnet_id)
        {
            DataTable dt = GetValidEntityById(jobnet_id);
            if (dt.Rows.Count == 1)
            {
                return dt;
            }

            return GetMaxUpdateDateEntityById(jobnet_id);

        }

        #endregion
    }
}
