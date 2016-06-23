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
//  * @author KIM 2012/10/19 新規作成<BR>　　　               　     *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.DAO
{
    /// <summary>
    /// ユーザーテーブルのDAOクラス
    /// </summary>
    public class RunJobnetSummaryDAO : BaseDAO
    {
        #region フィールド


        private string _tableName = "ja_run_jobnet_summary_table";

        private string[] _primaryKey = { "inner_jobnet_id" };

        private string _selectSql = "select * from ja_run_jobnet_summary_table where 0!=0";

        private string _selectSqlByPk = "select * from ja_run_jobnet_summary_table " +
                                        "where inner_jobnet_id = ? ";

        //added by YAMA 2014/07/09
        private String _select_run_jobnet_super = "select JR.* from "
                                        + "ja_run_jobnet_summary_table AS JR "
                                        + "where ((JR.scheduled_time between ? and ?) OR (JR.start_time between ? and ?) OR ((JR.scheduled_time = 0) and (JR.start_time between ? and ?)) OR ((JR.scheduled_time = 0) and (JR.start_time = 0) and (multiple_start_up = 2))) order by JR.scheduled_time,JR.start_time,JR.inner_jobnet_id";


        //added by YAMA 2014/07/09
        private String _select_run_jobnet = "select JRAll.* from ((select JR.* from "
                                                + "ja_run_jobnet_summary_table AS JR "
                                                + "where ((JR.scheduled_time between ? and ?) OR (JR.start_time between ? and ?) OR ((JR.scheduled_time = 0) and (JR.start_time between ? and ?)) OR ((JR.scheduled_time = 0) and (JR.start_time = 0) and (multiple_start_up = 2))) and JR.public_flag=1) "
                                                + "union "
                                                + "(select JR.* from "
                                                + "ja_run_jobnet_summary_table AS JR, users AS U, users_groups AS UG1, users_groups AS UG2 "
                                                + "where ((JR.scheduled_time between ? and ?) OR (JR.start_time between ? and ?) OR ((JR.scheduled_time = 0) and (JR.start_time between ? and ?)) OR ((JR.scheduled_time = 0) and (JR.start_time = 0) and (multiple_start_up = 2)))  and "
                                                + "JR.public_flag=0 and JR.user_name=U.alias and U.userid=UG1.userid and UG2.userid=? and UG1.usrgrpid=UG2.usrgrpid)) "
                                                + "as JRAll "
                                                + "order by JRAll.scheduled_time,JRAll.start_time,JRAll.inner_jobnet_id";

        //特権ユーザー以外の実行ジョブエラーリスト取得SQL文
        private String _select_run_jobnet_err = "select JRAll.* from ((select JR.* "
                                                + "from ja_run_jobnet_summary_table AS JR where JR.job_status=2 and (JR.status=2 or JR.status=4 or JR.status=5) and JR.public_flag=1) "
                                                + "union "
                                                + "(select JR.* "
                                                + "from ja_run_jobnet_summary_table AS JR, users AS U, users_groups AS UG1, users_groups AS UG2 "
                                                + "where JR.job_status=2 and (JR.status=2 or JR.status=4 or JR.status=5) and "
                                                + "JR.public_flag=0 and JR.user_name=U.alias and U.userid=UG1.userid and UG2.userid=? and UG1.usrgrpid=UG2.usrgrpid)) "
                                                + "as JRAll "
                                                + "order by JRAll.start_time desc,JRAll.inner_jobnet_id desc";
        //特権ユーザーの実行ジョブエラーリスト取得SQL文
        private String _select_run_jobnet_err_super = "select JR.* from "
                                                + "ja_run_jobnet_summary_table AS JR "
                                                + "where JR.job_status=2 and (JR.status=2 or JR.status=4 or JR.status=5) order by start_time desc,inner_jobnet_id desc";
        //特権ユーザー以外の実行ジョブ実行中リスト取得SQL文
        private String _select_run_jobnet_running = "select JRAll.* from ((select JR.* from "
                                                + "ja_run_jobnet_summary_table AS JR "
                                                + "where JR.status=2 and "
                                                + "JR.public_flag=1) "
                                                + "union "
                                                + "(select JR.* from "
                                                + "ja_run_jobnet_summary_table AS JR, users AS U, users_groups AS UG1, users_groups AS UG2 "
                                                + "where JR.status=2 and "
                                                + "JR.public_flag=0 and JR.user_name=U.alias and U.userid=UG1.userid and UG2.userid=? and UG1.usrgrpid=UG2.usrgrpid)) "
                                                + "as JRAll "
                                                + "order by JRAll.start_time,JRAll.inner_jobnet_id";
        //特権ユーザーの実行ジョブ実行中リスト取得SQL文
        private String _select_run_jobnet_running_super = "select JR.* from "
                                                + "ja_run_jobnet_summary_table AS JR "
                                                + "where JR.status=2 order by start_time,inner_jobnet_id";

        //エラー実行中ジョブネットを停止SQL文
        private String _stop_err_jobnet = "update ja_run_jobnet_summary_table set jobnet_abort_flag=1 where inner_jobnet_id=?";


        private DBConnect _db = null;

        #endregion

        #region コンストラクタ

        public RunJobnetSummaryDAO(DBConnect db)
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
        /// <param name="fromTime">from時刻</param>
        /// <param name="endTime">to時刻</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntitySuperAll(object fromTime, object endTime, object startFromTime, object startEndTime)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@scheduled_time", fromTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@scheduled_time", endTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startFromTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startEndTime));

            //added by YAMA 2014/07/07
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startFromTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startEndTime));

            DataTable dt = _db.ExecuteQuery(_select_run_jobnet_super, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// データの取得.
        /// </summary>
        /// <param name="fromTime">from時刻</param>
        /// <param name="endTime">to時刻</param>
        /// <param name="userid">ユーザーＩＤ</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityAll(object fromTime, object endTime, object startFromTime, object startEndTime, object userid)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@scheduled_time", fromTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@scheduled_time", endTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startFromTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startEndTime));

            //added by YAMA 2014/07/07
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startFromTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startEndTime));

            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@scheduled_time", fromTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@scheduled_time", endTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startFromTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startEndTime));

            //added by YAMA 2014/07/07
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startFromTime));
            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@start_time", startEndTime));

            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@userid", userid));

            DataTable dt = _db.ExecuteQuery(_select_run_jobnet, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// データの取得.
        /// </summary>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntitySuperErr()
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();

            DataTable dt = _db.ExecuteQuery(_select_run_jobnet_err_super, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// データの取得.
        /// </summary>
        /// <param name="userid">ユーザーＩＤ</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityErr(object userid)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();

            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@userid", userid));

            DataTable dt = _db.ExecuteQuery(_select_run_jobnet_err, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// データの取得.
        /// </summary>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntitySuperRunning()
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();

            DataTable dt = _db.ExecuteQuery(_select_run_jobnet_running_super, sqlParams, TableName);

            return dt;
        }

        //************************************************************************
        /// <summary> 
        /// データの取得.
        /// </summary>
        /// <param name="userid">ユーザーＩＤ</param>
        /// <return>検索結果</return>
        //************************************************************************
        public DataTable GetEntityRunning(object userid)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();

            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@userid", userid));

            DataTable dt = _db.ExecuteQuery(_select_run_jobnet_running, sqlParams, TableName);

            return dt;
        }
        //************************************************************************
        /// <summary> 
        /// データの更新.
        /// </summary>
        /// <param name="inner_jobnet_id">実行用ジョブネット内部管理ID</param>
        /// <return>検索結果</return>
        //************************************************************************
        public int UpdateErrJobnetStop(object innerJobnetId)
        {
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();

            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@inner_jobnet_id", innerJobnetId));
            int count = _db.ExecuteNonQuery(_stop_err_jobnet, sqlParams);

            return count;
        }

        #endregion
    }
}
