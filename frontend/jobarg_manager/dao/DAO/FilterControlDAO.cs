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
//  * @author 孟凡軍 2014/11/095 新規作成<BR>                       *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.DAO
{
    /// <summary>
    /// フィルター管理テーブルのDAO
    /// </summary>
    public class FilterControlDAO : ObjectBaseDAO
    {
        #region フィールド

        private string _tableName = "ja_filter_control_table";

        private string[] _primaryKey = { "filter_id", "update_date" };

        private string _selectSql = "select * from ja_filter_control_table where 0!=0";

        /// <summary>ロック </summary>
        private string _selectWithLock = "select * from ja_filter_control_table " +
            "where filter_id = ? for update nowait ";

        /// <summary>ロックしない </summary>
        private string _selectCountByPk = "select count(1) as count from ja_filter_control_table " +
            "where filter_id = ? and update_date = ?";

        /// <summary>フィルターIDの重複登録チェック用のSQL </summary>
        private string _selectSqlForCheck = "select count(1) as count from ja_filter_control_table " +
             "where filter_id = ?";

        private string _selectValidFilterSql = "select * from ja_filter_control_table " +
            "where filter_id = ? and valid_flag = '1'";

        private string _selectSqlByUserNm = "select filter_id, filter_name, MAX(update_date) from " +
            "ja_filter_control_table where public_flag = '1' and user_name in (select alias from users " +
            "where userid in (select distinct userid from users_groups where usrgrpid in (select b.usrgrpid " +
            "from users a inner join users_groups b on a.userid = b.userid where a.alias = ?))) " +
            "order by filter_id ASC";

        private string _selectSqlByPk = "select * from ja_filter_control_table " +
            "where filter_id = ? and update_date = ? ";

        private string _selectSqlByFilterId = "select * from ja_filter_control_table " +
            "where filter_id = ? order by update_date desc";

        private string _deleteSqlByPk = "delete from ja_filter_control_table " +
              "where filter_id = ? and update_date = ? ";

        /// <summary>フィルターIDの最新データ取得用のSQL </summary>
        private string _selectMaxUpdateDateByFilterIdSql = "select * from ja_filter_control_table " +
             "where filter_id = ? and update_date = (select max(update_date) from ja_filter_control_table where filter_id=?)";
        private DBConnect _db = null;

        #endregion

        #region コンストラクタ

        public FilterControlDAO(DBConnect db)
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
        /// <param name="filter_id">スケジュールID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public int GetCountByPk(object filter_id, object update_date)
        {
            int count = 0;

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_selectCountByPk, sqlParams, TableName);

            if (dt != null)
                count = int.Parse(Convert.ToString(dt.Rows[0]["count"]));

            return count;
        }
        //************************************************************************
        /// <summary> データの取得(フィルターIDの重複チェック用)</summary>
        /// <param name="filter_id">フィルターID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetCountForCheck(object filter_id)
        {
            string count = "";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "filter_id", filter_id));
            DataTable dt = _db.ExecuteQuery(_selectSqlForCheck, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary>MysqlDBのIDデータロック取得</summary>
        /// <param name="filter_id">スケジュールID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public int GetLock4Mysql(object filter_id)
        {
            int count = 0;
            string _getLock = "SELECT GET_LOCK('ja_filter_control_table." + filter_id + "', 0) as count";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            DataTable dt = _db.ExecuteQuery(_getLock, sqlParams, TableName);

            count = int.Parse(Convert.ToString(dt.Rows[0]["count"]));
            if (count < 1)
            {
                RealseLock(filter_id);
                _db.CloseSqlConnect();
                throw new DBException();
            }
            return count;

        }

        //************************************************************************
        /// <summary>NotMysqlDBのIDデータロック取得</summary>
        /// <param name="filter_id">スケジュールID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public int GetLock4NotMysql(object filter_id)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            DataTable dt = _db.ExecuteQuery(_selectWithLock, sqlParams, TableName);

            return dt.Rows.Count;

        }

        //************************************************************************
        /// <summary>IDデータロック取得</summary>
        /// <param name="filter_id">スケジュールID</param>
        /// <param name="dbType">DB種別</param>
        /// <return>検索結果</return>
        //************************************************************************
        public int GetLock(object filter_id, Consts.DBTYPE dbType)
        {
            if (dbType == Consts.DBTYPE.MYSQL)
            {
                return GetLock4Mysql(filter_id);
            }
            return GetLock4NotMysql(filter_id);

        }


        //************************************************************************
        /// <summary>MysqlDBのIDデータロック開放</summary>
        /// <param name="filter_id">スケジュールID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public String RealseLock(object filter_id)
        {
            string count = "";

            string _releaseLock = "SELECT RELEASE_LOCK('ja_filter_control_table." + filter_id + "') as count";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            DataTable dt = _db.ExecuteQuery(_releaseLock, sqlParams, TableName);

            if (dt != null)
            {
                count = Convert.ToString(dt.Rows[0]["count"]);
            }

            return count;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="filter_id">フィルターID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetInfoByUserNm(object user_name)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@alias", user_name));
            DataTable dt = _db.ExecuteQuery(_selectSqlByUserNm, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="filter_id">スフィルターID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetValidEntityById(object filter_id)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            DataTable dt = _db.ExecuteQuery(_selectValidFilterSql, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="filter_id">フィルターID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByPk(object filter_id, object update_date)
        {

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_selectSqlByPk, sqlParams, TableName);

            return dt;

        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="filter_id">フィルターID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByFilterId(object filter_id)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            DataTable dt = _db.ExecuteQuery(_selectSqlByFilterId, sqlParams, TableName);

            return dt;

        }

        //************************************************************************
        /// <summary> 
        /// データを削除.
        /// </summary>
        /// <param name="filter_id">フィルターID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public int DeleteByPk(object filter_id, object update_date)
        {

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            int count = _db.ExecuteNonQuery(_deleteSqlByPk, sqlParams);

            return count;

        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="filter_id">フィルターID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public override DataTable GetEntityByObjectId(object filter_id)
        {
            DataTable dt = GetEntityByFilterId(filter_id);

            return dt;
        }

        //************************************************************************
        /// <summary> 最新データの取得</summary>
        /// <param name="filterId">フィルターID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetMaxUpdateDateEntityById(object filter_id)
        {

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            sqlParams.Add(new ComSqlParam(DbType.String, "@filter_id", filter_id));
            DataTable dt = _db.ExecuteQuery(_selectMaxUpdateDateByFilterIdSql, sqlParams, TableName);

            return dt;

        }

        //************************************************************************
        /// <summary> 有効データが存在する場合有効データ、存在しない場合最新データの取得</summary>
        /// <param name="filterId">フィルターID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetValidORMaxUpdateDateEntityById(object filter_id)
        {
            DataTable dt = GetValidEntityById(filter_id);
            if (dt.Rows.Count == 1)
            {
                return dt;
            }

            return GetMaxUpdateDateEntityById(filter_id);

        }
        #endregion
    }
}
