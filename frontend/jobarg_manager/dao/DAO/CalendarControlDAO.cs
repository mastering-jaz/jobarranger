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
//  * @author KIM 2012/11/095 新規作成<BR>                    *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.DAO
{
    /// <summary>
    /// カレンダー管理テーブルのDAO
    /// </summary>
    public class CalendarControlDAO : ObjectBaseDAO
    {
        #region フィールド

        private string _tableName = "ja_calendar_control_table";

        private string[] _primaryKey = { "calendar_id", "update_date" };

        private string _selectSql = "select * from ja_calendar_control_table where 0!=0";

        /// <summary>ロック </summary>
        private string _selectCountWithLock = "select * from ja_calendar_control_table " +
            "where calendar_id = ? and update_date = ? for update nowait ";

        /// <summary>ロックしない </summary>
        private string _selectCountNoLock = "select count(1) as count from ja_calendar_control_table " +
            "where calendar_id = ? and update_date = ?";

        private string _selectCountByJobNetIdSql = "select count(1) as count from ja_calendar_control_table " +
            "where calendar_id = ? and public_flag = '1'";

        private string _selectValidByCalendarIdSql = "select * from ja_calendar_control_table " +
            "where calendar_id = ? and valid_flag = '1'";

        private string _selectSqlByUserNm = "select calendar_id, calendar_name, MAX(update_date) from " +
            "ja_calendar_control_table where public_flag = '1' and user_name in (select alias from users " +
            "where userid in (select distinct userid from users_groups where usrgrpid in (select b.usrgrpid " +
            "from users a inner join users_groups b on a.userid = b.userid where a.alias = ?))) " +
            "order by calendar_id ASC";

        private string _selectSqlByPk = "select * from ja_calendar_control_table " +
            "where calendar_id = ? and update_date = ? ";

        private string _selectSqlByCalendarId = "select * from ja_calendar_control_table " +
            "where calendar_id = ? order by update_date desc";

        /// <summary>カレンダーIDの重複登録チェック用のSQL </summary>
        private string _selectSqlForCheck = "select count(1) as count from ja_calendar_control_table " +
             "where calendar_id = ?";

        /// <summary>カレンダーIDの最新データ取得用のSQL </summary>
        private string _selectMaxUpdateDateByCalendarIdSql = "select * from ja_calendar_control_table " +
             "where calendar_id = ? and update_date = (select max(update_date) from ja_calendar_control_table where calendar_id=?)";

        private string _deleteSqlByPk = "delete from ja_calendar_control_table " +
              "where calendar_id = ? and update_date = ? ";

        private DBConnect _db = null;

        #endregion

        #region コンストラクタ

        public CalendarControlDAO(DBConnect db)
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
        /// <param name="calendar_id">カレンダーID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        //public string GetCountByPkWithLock(object calendar_id, object update_date)
        //{
        //    string count = "";

        //    List<ComSqlParam> sqlParams = new List<ComSqlParam>();
        //    sqlParams.Add(new ComSqlParam(DbType.String, "calendar_id", calendar_id));
        //    sqlParams.Add(new ComSqlParam(DbType.Int64, "update_date", update_date));
        //    DataTable dt = _db.ExecuteQuery(_selectCountWithLock, sqlParams, TableName);

        //    if (dt != null)
        //        count = Convert.ToString(dt.Rows[0]["count"]);

        //    return count;
        //}

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetCountByPk(object calendar_id, object update_date)
        {
            string count = "";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "calendar_id", calendar_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_selectCountNoLock, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得(For Update)</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetCountByPkForUpdate(object calendar_id, object update_date)
        {
            string count = "";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "calendar_id", calendar_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_selectCountWithLock, sqlParams, TableName);

            // データ有の場合

            if (dt != null || dt.Rows.Count > 0)
                return "1";

            return count;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetLockByPk(object calendar_id, object update_date)
        {
            string count = "";

            string _getLock = "SELECT GET_LOCK('ja_calendar_control_table."+ calendar_id +
                 "." + update_date + "', 0) as count";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "calendar_id", calendar_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_getLock, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string RealseLockByPk(object calendar_id, object update_date)
        {
            string count = "";

            string _releaseLock = "SELECT RELEASE_LOCK('ja_calendar_control_table." + calendar_id +
                "." + update_date + "') as count";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "calendar_id", calendar_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_releaseLock, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetCountByJobNetId(object calendar_id)
        {
            string count = "";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "calendar_id", calendar_id));
            DataTable dt = _db.ExecuteQuery(_selectCountByJobNetIdSql, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得(カレンダーIDの重複チェック用)</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public string GetCountForCheck(object calendar_id)
        {
            string count = "";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "calendar_id", calendar_id));
            DataTable dt = _db.ExecuteQuery(_selectSqlForCheck, sqlParams, TableName);

            if (dt != null)
                count = Convert.ToString(dt.Rows[0]["count"]);

            return count;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetInfoByUserNm(object user_name)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "alias", user_name));
            DataTable dt = _db.ExecuteQuery(_selectSqlByUserNm, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 有効データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetValidEntityById(object calendar_id)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@calendar_id", calendar_id));
            DataTable dt = _db.ExecuteQuery(_selectValidByCalendarIdSql, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByPk(object calendar_id, object update_date)
        {

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@calendar_id", calendar_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            DataTable dt = _db.ExecuteQuery(_selectSqlByPk, sqlParams, TableName);

            return dt;

        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityByCalendarId(object calendar_id)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@calendar_id", calendar_id));
            DataTable dt = _db.ExecuteQuery(_selectSqlByCalendarId, sqlParams, TableName);

            return dt;

        }

        //************************************************************************
        /// <summary> 
        /// データを削除.
        /// </summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <param name="update_date">更新日</param>
        /// <return>検索結果</return>
        //************************************************************************
        public int DeleteByPk(object calendar_id, object update_date)
        {

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@calendar_id", calendar_id));
            sqlParams.Add(new ComSqlParam(DbType.Int64, "@update_date", update_date));
            int count = _db.ExecuteNonQuery(_deleteSqlByPk, sqlParams);

            return count;

        }

        //************************************************************************
        /// <summary> データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public override DataTable GetEntityByObjectId(object calendar_id)
        {
            DataTable dt = GetEntityByCalendarId(calendar_id);

            return dt;
        }

        //************************************************************************
        /// <summary> 最新データの取得</summary>
        /// <param name="jobnet_id">カレンダーID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetMaxUpdateDateEntityById(object calendar_id)
        {

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@calendar_id", calendar_id));
            sqlParams.Add(new ComSqlParam(DbType.String, "@calendar_id", calendar_id));
            DataTable dt = _db.ExecuteQuery(_selectMaxUpdateDateByCalendarIdSql, sqlParams, TableName);

            return dt;

        }

        //************************************************************************
        /// <summary> 有効データが存在する場合有効データ、存在しない場合最新データの取得</summary>
        /// <param name="calendar_id">カレンダーID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetValidORMaxUpdateDateEntityById(object calendar_id)
        {
            DataTable dt = GetValidEntityById(calendar_id);
            if (dt.Rows.Count == 1)
            {
                return dt;
            }

            return GetMaxUpdateDateEntityById(calendar_id);

        }

        #endregion
    }
}
