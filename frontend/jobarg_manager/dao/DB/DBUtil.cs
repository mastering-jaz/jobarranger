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
using System.Collections;
using jp.co.ftf.jobcontroller;
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
    public class DBUtil
    {
        //エクスポートカレンダーオブジェクト関連テーブル一覧最初が管理テーブル
        private static String[] EXPORT_CALENDAR_TABLES = { "ja_calendar_control_table", 
                                                    "ja_calendar_detail_table" 
                                                  };
        //エクスポートスケジュールオブジェクト関連テーブル一覧最初が管理テーブル
        private static String[] EXPORT_SCHEDULE_TABLES = { "ja_schedule_control_table", 
                                                    "ja_schedule_detail_table", 
                                                    "ja_schedule_jobnet_table" 
                                                  };
        //エクスポートジョブネットオブジェクト関連テーブル一覧最初が管理テーブル
        private static String[] EXPORT_JOBNET_TABLES = { "ja_jobnet_control_table",
                                                  "ja_job_control_table",
                                                  "ja_flow_control_table",
                                                  "ja_icon_calc_table",
                                                  "ja_icon_end_table",
                                                  "ja_icon_extjob_table",
                                                  "ja_icon_if_table",
                                                  "ja_icon_info_table",
                                                  "ja_icon_jobnet_table",
                                                  "ja_icon_job_table",
                                                  "ja_job_command_table",
                                                  "ja_value_job_table",
                                                  "ja_value_jobcon_table",
                                                  "ja_icon_task_table",
                                                  "ja_icon_value_table",
                                                  "ja_icon_fcopy_table",
                                                  "ja_icon_fwait_table",
                                                  "ja_icon_reboot_table",
                                                  "ja_icon_release_table"
                                                };
        //インポート時、重複チェック用情報
        private static Hashtable KEY_FOR_DOUBLE_CHECK = new Hashtable();
        //インポート時、オブジェクト間関連チェック用情報
        private static Hashtable KEY_FOR_RELATION_CHECK = new Hashtable();

        private static bool SET_DOUBLE_KEY = false;
        //整合性チェック情報セットフラグ
        private static bool SET_RELATE_KEY = true;
        #region 採番処理


        /// <summary>通番の取得</summary>
        /// <param name="strIndexType">通番のタイプ</param>
        /// <return>通番ID</return>
        public static string GetNextId(string strIndexType)
        {
            string strNextId = "";

            string strSql = "SELECT nextid FROM ja_index_table WHERE count_id = " + strIndexType+" for update";

            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            db.BeginTransaction();

            DataTable dt = db.ExecuteQuery(strSql);

            if (dt.Rows.Count == 1)
            {
                strNextId = dt.Rows[0]["NEXTID"].ToString();

                strSql = "UPDATE ja_index_table SET nextid = nextid + 1 WHERE count_id = " + strIndexType;

                db.ExecuteNonQuery(strSql);

                db.TransactionCommit();

                db.CloseSqlConnect();
            }
            else
            {
                db.TransactionRollback();
                db.CloseSqlConnect();
                throw new DBException(Consts.SYSERR_004, null);
            }          
            
            return strNextId;
        }

        /// <summary>有効なジョブネットを取得</summary>
        /// <param name="jobnetId">ジョブネットＩＤ</param>
        /// <return>データ</return>
        public static DataTable GetValidJobnetVer(string jobnetId)
        {
            String sql = "select * from ja_jobnet_control_table where jobnet_id='" + jobnetId + "' and valid_flag=1";
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            DataTable dt = db.ExecuteQuery(sql);
            db.CloseSqlConnect();
            return dt;
        }

        /// <summary>ジョブネット画面から実行時、実行テーブルに追加する</summary>
        /// <param name="jobnetRow">ジョブネットデータ</param>
        /// <param name="runType">実行種別</param>
        /// <return>実行ジョブネット内部管理ＩＤ</return>
        public static String InsertRunJobnet(DataRow jobnetRow, Consts.RunTypeEnum runType)
        {
            String innerJobnetId = DBUtil.GetNextId("2");
            String sql = "insert into ja_run_jobnet_table "
                            + "(inner_jobnet_id, inner_jobnet_main_id, inner_job_id, update_date, run_type, "
                            + "main_flag, status, start_time, end_time, public_flag, jobnet_id, user_name, jobnet_name, memo, execution_user_name) "
                            + "VALUES (?,?,0,?,?,0,0,0,0,?,?,?,?,?,?)";
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);

            List<ComSqlParam> insertRunJobnetSqlParams = new List<ComSqlParam>();
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", innerJobnetId));
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_main_id", innerJobnetId));
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@update_date", jobnetRow["update_date"]));
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@run_type", (int)runType));
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@public_flag", jobnetRow["public_flag"]));
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", jobnetRow["jobnet_id"]));
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@user_name", jobnetRow["user_name"]));
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_name", jobnetRow["jobnet_name"]));
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@memo", jobnetRow["memo"]));
            insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@execution_user_name", LoginSetting.UserName));
            db.CreateSqlConnect();
            db.BeginTransaction();
            int count = db.ExecuteNonQuery(sql, insertRunJobnetSqlParams);
            db.TransactionCommit();
            db.CloseSqlConnect();
            if (count == 1) return innerJobnetId;
            return null;
        }

        /// <summary>実行ジョブネットが展開されたか判断</summary>
        /// <param name="innerJobnetId">実行ジョブネット内部管理ＩＤ</param>
        /// <return>展開された場合True、されてない場合False</return>
        public static DataTable GetRunJobnetSummary(String innerJobnetId)
        {
            String sql = "select * from ja_run_jobnet_summary_table where inner_jobnet_id=" + innerJobnetId + " and invo_flag=1";
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            DataTable dt = db.ExecuteQuery(sql);
            db.CloseSqlConnect();
            return dt;
        }


        /// <summary>パラメータ取得ary>
        /// <param name="parameterName">パラメータ名</param>
        /// <return>パラメータ値</return>
        public static string GetParameterVelue(string parameterName)
        {
            string strParameterVelue = "";

            string strSql = "SELECT value FROM ja_parameter_table WHERE parameter_name = '" + parameterName + "'";

            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();

            DataTable dt = db.ExecuteQuery(strSql);

            if (dt.Rows.Count == 1)
            {
                strParameterVelue = dt.Rows[0]["value"].ToString();

                db.CloseSqlConnect();
            }
            else
            {
                strParameterVelue = "60";
                db.CloseSqlConnect();
            }

            return strParameterVelue;
        }

        /// <summary>ユーザーが属するグループ取得ary>
        /// <param name="alias">別名</param>
        /// <return>グループＩＤリスト</return>
        public static List<Decimal> GetGroupIDListByAlias(string alias)
        {
            List<Decimal> groupList = new List<Decimal>();

            string strSql = "select UG.usrgrpid from users_groups AS UG,users AS U where U.alias='"+alias+"' and U.userid=UG.userid";

            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();

            DataTable dt = db.ExecuteQuery(strSql);

            foreach(DataRow row in dt.Rows)
            {
                groupList.Add(Convert.ToDecimal(row["usrgrpid"]));

            }
            db.CloseSqlConnect();

            return groupList;
        }

        /// <summary>ユーザーが属するグループ取得ary>
        /// <param name="alias">別名</param>
        /// <return>グループＩＤリスト</return>
        public static List<Decimal> GetGroupIDListByID(string objectId, Consts.ObjectEnum objectType)
        {
            List<Decimal> groupList = new List<Decimal>();

            String tableName = "ja_calendar_control_table";
            String idColumnName = "calendar_id";
            if (objectType == Consts.ObjectEnum.SCHEDULE)
            {
                tableName = "ja_schedule_control_table";
                idColumnName = "schedule_id";
            }
            if (objectType == Consts.ObjectEnum.JOBNET)
            {
                tableName = "ja_jobnet_control_table";
                idColumnName = "jobnet_id";
            }
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            String objectSql = "select * from " + tableName + " where " + idColumnName + "='" + objectId + "'";
            DataTable objectDt = db.ExecuteQuery(objectSql);
            if (objectDt.Rows.Count > 0)
            {

                string strSql = "select UG.usrgrpid from users_groups AS UG,users AS U where U.alias='" + (String)objectDt.Rows[0]["user_name"] + "' and U.userid=UG.userid";

                DataTable dt = db.ExecuteQuery(strSql);

                foreach (DataRow row in dt.Rows)
                {
                    groupList.Add(Convert.ToDecimal(row["usrgrpid"]));

                }
            }
            db.CloseSqlConnect();

            return groupList;
        }

        /// <summary>ユーザーが属するグループ取得ary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        /// <return>オブジェクトDataTable</return>
        public static DataTable GetObjectsById(string objectId, Consts.ObjectEnum objectType)
        {

            String tableName = "ja_calendar_control_table";
            String idColumnName = "calendar_id";
            if (objectType == Consts.ObjectEnum.SCHEDULE)
            {
                tableName = "ja_schedule_control_table";
                idColumnName = "schedule_id";
            }
            if (objectType == Consts.ObjectEnum.JOBNET)
            {
                tableName = "ja_jobnet_control_table";
                idColumnName = "jobnet_id";
            }
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            String objectSql = "select * from " + tableName + " where " + idColumnName + "='" + objectId + "'";
            DataTable objectDt = db.ExecuteQuery(objectSql);

            db.CloseSqlConnect();

            return objectDt;
        }

        /// <summary>オブジェクトを有効にする<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="updDate">更新日</param>
        /// <param name="objectType">オブジェクト種別</param>
        public static void SetObjectValid(String objectId, String updDate, Consts.ObjectEnum objectType)
        {
            String tableName = "ja_calendar_control_table";
            String idColumnName = "calendar_id";
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            db.BeginTransaction();
            try
            {
                if (objectType == Consts.ObjectEnum.CALENDAR)
                {
                    tableName = "ja_calendar_control_table";
                    idColumnName = "calendar_id";
                    CalendarControlDAO calendarControlDAO = new CalendarControlDAO(db);
                    calendarControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
                if (objectType == Consts.ObjectEnum.SCHEDULE)
                {
                    tableName = "ja_schedule_control_table";
                    idColumnName = "schedule_id";
                    ScheduleControlDAO scheduleControlDAO = new ScheduleControlDAO(db);
                    scheduleControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
                if (objectType == Consts.ObjectEnum.JOBNET)
                {
                    tableName = "ja_jobnet_control_table";
                    idColumnName = "jobnet_id";
                    JobnetControlDAO jobnetControlDAO = new JobnetControlDAO(db);
                    jobnetControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
            }
            catch (DBException e)
            {
                e.MessageID = Consts.ERROR_DB_LOCK;
                throw e;
            }
            string strSql1 = "update "+ tableName + " set valid_flag=0 where " + idColumnName + "='" + objectId + "' and valid_flag=1";
            string strSql2 = "update " + tableName + " set valid_flag=1 where " + idColumnName + "='" + objectId + "' and update_date=" + updDate;
            db.AddBatch(strSql1);
            db.AddBatch(strSql2);
            db.ExecuteBatchUpdate();
            db.TransactionCommit();
            db.CloseSqlConnect();
        }

        /// <summary>実行中ジョブネットを強制停止する。<summary>
        /// <param name="innerJobnetId">実行ジョブネット内部管理ＩＤ</param>
        public static void StopRunningJobnet(object innerJobnetId)
        {
            //エラー実行中ジョブネットを停止SQL文
            String _stop_err_jobnet = "update ja_run_jobnet_summary_table set jobnet_abort_flag=1 where inner_jobnet_id=? and status=2";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();

            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@inner_jobnet_id", innerJobnetId));
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            db.BeginTransaction();
            db.ExecuteNonQuery(_stop_err_jobnet, sqlParams);
            db.TransactionCommit();
            db.CloseSqlConnect();
        }

        /// <summary>未実行ジョブネットを停止する。<summary>
        /// <param name="innerJobnetId">実行ジョブネット内部管理ＩＤ</param>
        public static void StopUnexecutedJobnet(object innerJobnetId)
        {
            //エラー実行中ジョブネットを停止SQL文
            int jobnetLoadSpan = Convert.ToInt32(DBUtil.GetParameterVelue("JOBNET_LOAD_SPAN"));
            DateTime now = DateTime.Now;
            decimal startTime = ConvertUtil.ConverDate2IntYYYYMMDDHHMISS(now);
            decimal endTime = ConvertUtil.ConverDate2IntYYYYMMDDHHMISS(now.AddMinutes(2 * jobnetLoadSpan));
            String _stop_enexecuted_jobnet = "update ja_run_jobnet_summary_table set status=5, start_time=" + startTime + " ,end_time="+endTime+" where inner_jobnet_id=? and status=0";

            List<ComSqlParam> sqlParams = new List<ComSqlParam>();

            sqlParams.Add(new ComSqlParam(DbType.UInt64, "@inner_jobnet_id", innerJobnetId));
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            db.BeginTransaction();
            db.ExecuteNonQuery(_stop_enexecuted_jobnet, sqlParams);
            db.TransactionCommit();
            db.CloseSqlConnect();
        }

        /// <summary>オブジェクトを無効にする<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        /// <param name="rows">オブジェクトRows</param>
        public static void SetObjectsInValid(String objectId, Consts.ObjectEnum objectType, DataRow[] rows)
        {
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            db.BeginTransaction();
            String tableName = "ja_calendar_control_table";
            String idColumnName = "calendar_id";
            try
            {
                if (objectType == Consts.ObjectEnum.CALENDAR)
                {
                    tableName = "ja_calendar_control_table";
                    idColumnName = "calendar_id";
                    CalendarControlDAO calendarControlDAO = new CalendarControlDAO(db);
                    calendarControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
                if (objectType == Consts.ObjectEnum.SCHEDULE)
                {
                    tableName = "ja_schedule_control_table";
                    idColumnName = "schedule_id";
                    ScheduleControlDAO scheduleControlDAO = new ScheduleControlDAO(db);
                    scheduleControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
                if (objectType == Consts.ObjectEnum.JOBNET)
                {
                    tableName = "ja_jobnet_control_table";
                    idColumnName = "jobnet_id";
                    JobnetControlDAO jobnetControlDAO = new JobnetControlDAO(db);
                    jobnetControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
            }
            catch (DBException e)
            {
                e.MessageID = Consts.ERROR_DB_LOCK;
                throw e;
            }
            foreach (DataRow row in rows)
            {
                if ((Int32)row["valid_flag"] == 1)
                {
                    string strSql = "update " + tableName + " set valid_flag=0 where " + idColumnName + "='" + objectId + "' and update_date=" + Convert.ToString(row["update_date"]);
                    db.AddBatch(strSql);
                }
            }

            db.ExecuteBatchUpdate();
            db.TransactionCommit();
            db.CloseSqlConnect();

        }

        /// <summary>オブジェクトを削除するため、関連データ有無チェック<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        /// <param name="rows">オブジェクトRows</param>
        public static bool CheckForRelation4Del(String objectId, Consts.ObjectEnum objectType, DataRow[] rows)
        {
            if (objectType == Consts.ObjectEnum.CALENDAR)
            {
                if (IsExistRelate4Calendar(objectId, rows)) return false;
            }

            if (objectType == Consts.ObjectEnum.JOBNET)
            {
                if (IsExistRelate4Jobnet(objectId, rows)) return false;
            }
            return true;
        }

        /// <summary>オブジェクトを削除する<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        /// <param name="rows">オブジェクトRows</param>
        public static void DelObject(String objectId, Consts.ObjectEnum objectType, DataRow[] rows)
        {
            if (rows == null)
            {
                DelAllVer(objectId, objectType);
            }
            else
            {
                DelSpecialVer(objectId, objectType, rows);
            }
        }

        /// <summary>オブジェクトを削除する<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        private static void DelAllVer(String objectId, Consts.ObjectEnum objectType)
        {
            /*
            if (objectType == Consts.ObjectEnum.CALENDAR)
            {
                if (IsExistRelate4Calendar(objectId, null)) return false;
            }

            if (objectType == Consts.ObjectEnum.JOBNET)
            {
                if (IsExistRelate4Jobnet(objectId, null)) return false;
            }
            */
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            db.BeginTransaction();
            String tableName = "ja_calendar_control_table";
            String idColumnName = "calendar_id";
            try
            {
                if (objectType == Consts.ObjectEnum.CALENDAR)
                {
                    tableName = "ja_calendar_control_table";
                    idColumnName = "calendar_id";
                    CalendarControlDAO calendarControlDAO = new CalendarControlDAO(db);
                    calendarControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
                if (objectType == Consts.ObjectEnum.SCHEDULE)
                {
                    tableName = "ja_schedule_control_table";
                    idColumnName = "schedule_id";
                    ScheduleControlDAO scheduleControlDAO = new ScheduleControlDAO(db);
                    scheduleControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
                if (objectType == Consts.ObjectEnum.JOBNET)
                {
                    tableName = "ja_jobnet_control_table";
                    idColumnName = "jobnet_id";
                    JobnetControlDAO jobnetControlDAO = new JobnetControlDAO(db);
                    jobnetControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
            }
            catch (DBException e)
            {
                e.MessageID = Consts.ERROR_DB_LOCK;
                throw e;
            }

            string strSql = "delete from " + tableName + " where " + idColumnName + "='" + objectId + "'";
            db.ExecuteNonQuery(strSql);
            db.TransactionCommit();
            db.CloseSqlConnect();
        }

        /// <summary>オブジェクトを削除する<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        /// <param name="rows">オブジェクトRows</param>
        private static void DelSpecialVer(String objectId, Consts.ObjectEnum objectType, DataRow[] rows)
        {
            /*
            if (objectType == Consts.ObjectEnum.CALENDAR)
            {
                if (IsExistRelate4Calendar(objectId, rows)) return false;
            }

            if (objectType == Consts.ObjectEnum.JOBNET)
            {
                if (IsExistRelate4Jobnet(objectId, rows)) return false;
            }
            */

            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            db.BeginTransaction();
            String tableName = "ja_calendar_control_table";
            String idColumnName = "calendar_id";
            try
            {
                if (objectType == Consts.ObjectEnum.CALENDAR)
                {
                    tableName = "ja_calendar_control_table";
                    idColumnName = "calendar_id";
                    CalendarControlDAO calendarControlDAO = new CalendarControlDAO(db);
                    calendarControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
                if (objectType == Consts.ObjectEnum.SCHEDULE)
                {
                    tableName = "ja_schedule_control_table";
                    idColumnName = "schedule_id";
                    ScheduleControlDAO scheduleControlDAO = new ScheduleControlDAO(db);
                    scheduleControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
                if (objectType == Consts.ObjectEnum.JOBNET)
                {
                    tableName = "ja_jobnet_control_table";
                    idColumnName = "jobnet_id";
                    JobnetControlDAO jobnetControlDAO = new JobnetControlDAO(db);
                    jobnetControlDAO.GetLock(objectId, LoginSetting.DBType);
                }
            }
            catch (DBException e)
            {
                e.MessageID = Consts.ERROR_DB_LOCK;
                throw e;
            }
            foreach (DataRow row in rows)
            {
                string strSql = "delete from " + tableName + " where " + idColumnName + "='" + objectId + "' and update_date=" + Convert.ToString(row["update_date"]);
                db.AddBatch(strSql);
            }

            db.ExecuteBatchUpdate();
            db.TransactionCommit();
            db.CloseSqlConnect();
        }

        private static bool IsExistRelate4Calendar(String objectId, DataRow[] rows)
        {
            String sql = "select * from ja_schedule_detail_table where calendar_id='" + objectId + "'";
            DataTable dt;
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            if (rows == null)
            {
                dt = db.ExecuteQuery(sql);
                if (dt.Rows.Count > 0) return true;
            }
            else
            {
                DataTable objectDT = GetObjectsById(objectId, Consts.ObjectEnum.CALENDAR);
                if (objectDT.Rows.Count == rows.Length)
                {
                    dt = db.ExecuteQuery(sql);
                    if (dt.Rows.Count > 0) return true;
                }
            }
            return false;

        }

        private static bool IsExistRelate4Jobnet(String objectId, DataRow[] rows)
        {
            String sqlSchedule = "select * from ja_schedule_jobnet_table where jobnet_id='" + objectId + "'";
            String sqlLinkedJobnet = "select * from ja_icon_jobnet_table where link_jobnet_id='" + objectId + "'";
            String sqlSubmitJobnet = "select * from ja_icon_task_table where submit_jobnet_id='" + objectId + "'";
            DataTable dt;
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            if (rows == null)
            {
                dt = db.ExecuteQuery(sqlSchedule);
                if (dt.Rows.Count > 0) return true;
                dt = db.ExecuteQuery(sqlLinkedJobnet);
                if (dt.Rows.Count > 0) return true;
                dt = db.ExecuteQuery(sqlSubmitJobnet);
                if (dt.Rows.Count > 0) return true;
            }
            else
            {
                DataTable objectDT = GetObjectsById(objectId, Consts.ObjectEnum.JOBNET);
                if (objectDT.Rows.Count == rows.Length)
                {
                    dt = db.ExecuteQuery(sqlSchedule);
                    if (dt.Rows.Count > 0) return true;
                    dt = db.ExecuteQuery(sqlLinkedJobnet);
                    if (dt.Rows.Count > 0) return true;
                    dt = db.ExecuteQuery(sqlSubmitJobnet);
                    if (dt.Rows.Count > 0) return true;
                }
            }
            return false;

        }


        /// <summary>オブジェクトをエクスポーする<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        /// <param name="rows">オブジェクトRows</param>
        public static DataSet Export(String objectId, Consts.ObjectEnum objectType, DataRow[] rows)
        {

            if (objectId == null)
            {
                return ExportAll();

            }
            if (rows == null)
            {
                return ExportAllVer(objectId, objectType);
            }
            return ExportSpecialVer(objectId, objectType, rows);
        }
        /// <summary>全バージョンオブジェクトをエクスポーする<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        public static DataSet ExportAll()
        {
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            String idColumnName = "calendar_id";
            String dsName = "All";
            String[] sqlCalendar = new String[EXPORT_CALENDAR_TABLES.Length];
            int i = 0;
            foreach (String calendarTableName in EXPORT_CALENDAR_TABLES)
            {
                idColumnName = "calendar_id";
                sqlCalendar[i] = "select * from " + calendarTableName;
                if (!(LoginSetting.Authority == Consts.AuthorityEnum.SUPER))
                {
                    if (i == 0)
                    {
                        sqlCalendar[i] = "select distinct A.* from " + calendarTableName + " AS A,users AS U,users_groups AS UG1, users_groups AS UG2 where A.user_name=U.alias and UG1.userid=U.userid and UG2.userid=" + LoginSetting.UserID + " and UG1.usrgrpid=UG2.usrgrpid";
                    }
                    else
                    {
                        sqlCalendar[i] = "select distinct B.* from " + calendarTableName + " AS B," + EXPORT_CALENDAR_TABLES[0] + " AS A,users AS U,users_groups AS UG1, users_groups AS UG2 "
                                                + "where A." + idColumnName + "=B." + idColumnName + " and A.update_date=B.update_date and "
                                                + "A.user_name=U.alias and UG1.userid=U.userid and UG2.userid=" + LoginSetting.UserID + " and UG1.usrgrpid=UG2.usrgrpid";
                    }
                }
                db.AddSelectBatch(sqlCalendar[i], calendarTableName);
                i++;
            }
            String[] sqlSchedule = new String[EXPORT_SCHEDULE_TABLES.Length];
            i = 0;
            foreach (String scheduleTableName in EXPORT_SCHEDULE_TABLES)
            {
                idColumnName = "schedule_id";
                sqlSchedule[i] = "select * from " + scheduleTableName;
                if (!(LoginSetting.Authority == Consts.AuthorityEnum.SUPER))
                {
                    if (i == 0)
                    {
                        sqlSchedule[i] = "select distinct A.* from " + scheduleTableName + " AS A,users AS U,users_groups AS UG1, users_groups AS UG2 where A.user_name=U.alias and UG1.userid=U.userid and UG2.userid=" + LoginSetting.UserID + " and UG1.usrgrpid=UG2.usrgrpid";
                    }
                    else
                    {
                        sqlSchedule[i] = "select distinct B.* from " + scheduleTableName + " AS B," + EXPORT_SCHEDULE_TABLES[0] + " AS A,users AS U,users_groups AS UG1, users_groups AS UG2 "
                                                + "where A." + idColumnName + "=B." + idColumnName + " and A.update_date=B.update_date and "
                                                + "A.user_name=U.alias and UG1.userid=U.userid and UG2.userid=" + LoginSetting.UserID + " and UG1.usrgrpid=UG2.usrgrpid";
                    }
                }
                db.AddSelectBatch(sqlSchedule[i], scheduleTableName);
                i++;
            }

            String[] sqlJobnet = new String[EXPORT_JOBNET_TABLES.Length];
            i = 0;
            foreach (String jobnetTableName in EXPORT_JOBNET_TABLES)
            {
                idColumnName = "jobnet_id";
                sqlJobnet[i] = "select * from " + jobnetTableName;
                if (!(LoginSetting.Authority == Consts.AuthorityEnum.SUPER))
                {
                    if (i == 0)
                    {
                        sqlJobnet[i] = "select distinct A.* from " + jobnetTableName + " AS A,users AS U,users_groups AS UG1, users_groups AS UG2 where A.user_name=U.alias and UG1.userid=U.userid and UG2.userid=" + LoginSetting.UserID + " and UG1.usrgrpid=UG2.usrgrpid";
                    }
                    else
                    {
                        sqlJobnet[i] = "select distinct B.* from " + jobnetTableName + " AS B," + EXPORT_JOBNET_TABLES[0] + " AS A,users AS U,users_groups AS UG1, users_groups AS UG2 "
                                                + "where A." + idColumnName + "=B." + idColumnName + " and A.update_date=B.update_date and "
                                                + "A.user_name=U.alias and UG1.userid=U.userid and UG2.userid=" + LoginSetting.UserID + " and UG1.usrgrpid=UG2.usrgrpid";
                    }
                }
                db.AddSelectBatch(sqlJobnet[i], jobnetTableName);
                i++;
            }
            DataSet ds = db.ExecuteBatchQuery();
            ds.DataSetName = dsName;
            db.CloseSqlConnect();
            setExportUserInfo(ds);
            return ds;

        }
        /// <summary>全バージョンオブジェクトをエクスポーする<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        public static DataSet ExportAllVer(String objectId, Consts.ObjectEnum objectType)
        {
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            String idColumnName = "calendar_id";
            String dsName = "Calendar";
            String[] tables = EXPORT_CALENDAR_TABLES;
            if (objectType == Consts.ObjectEnum.SCHEDULE)
            {
                idColumnName = "schedule_id";
                dsName = "Schedule";
                tables = EXPORT_SCHEDULE_TABLES;
            }
            if (objectType == Consts.ObjectEnum.JOBNET)
            {
                idColumnName = "jobnet_id";
                dsName = "Jobnet";
                tables = EXPORT_JOBNET_TABLES;
            }
            //setExportUserInfo(ds);
            String[] sql = new String[tables.Length];
            int i = 0;
            foreach (String tableName in tables)
            {
                sql[i] = "select * from " + tables[i] + " where " + idColumnName + "='" + objectId + "'";
                db.AddSelectBatch(sql[i], tableName);
                i++;
            }
            DataSet ds = db.ExecuteBatchQuery();
            ds.DataSetName = dsName;
            db.CloseSqlConnect();
            setExportUserInfo(ds);
            return ds;
        }
        /// <summary>個別バージョンオブジェクトをエクスポーする<summary>
        /// <param name="objectId">オブジェクトＩＤ</param>
        /// <param name="objectType">オブジェクト種別</param>
        /// <param name="rows">オブジェクトRows</param>
        public static DataSet ExportSpecialVer(String objectId, Consts.ObjectEnum objectType, DataRow[] rows)
        {
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            String idColumnName = "calendar_id";
            String dsName = "Calendar";
            String[] tables = EXPORT_CALENDAR_TABLES;
            if (objectType == Consts.ObjectEnum.SCHEDULE)
            {
                idColumnName = "schedule_id";
                dsName = "Schedule";
                tables = EXPORT_SCHEDULE_TABLES;
            }
            if (objectType == Consts.ObjectEnum.JOBNET)
            {
                idColumnName = "jobnet_id";
                dsName = "Jobnet";
                tables = EXPORT_JOBNET_TABLES;
            }

            String[] sql = new String[tables.Length];

            int i = 0;
            int count = 0;
            foreach (String tableName in tables)
            {
                sql[i] = "select * from " + tables[i] + " where " + idColumnName + "='" + objectId + "' and (";
                count = 0;
                foreach (DataRow row in rows)
                {
                    count++;
                    sql[i] = sql[i] + "update_date=" + Convert.ToString(row["update_date"]);
                    if (count < rows.Length)
                    {
                        sql[i] = sql[i] + " OR ";
                    }
                    else
                    {
                        sql[i] = sql[i] + ")";
                    }

                }
                db.AddSelectBatch(sql[i], tableName);
                i++;

            }
            DataSet ds = db.ExecuteBatchQuery();
            ds.DataSetName = dsName;
            db.CloseSqlConnect();
            setExportUserInfo(ds);
            return ds;

        }

        /// <summary>システム時間を取得</summary>
        /// <returns>システム時間</returns>
        public static DateTime GetSysTime()
        {
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();

            string strSql = "SELECT CURRENT_TIMESTAMP AS systemtime";

            DataTable resultTbl = db.ExecuteQuery(strSql);

            DateTime sysTime = Convert.ToDateTime(resultTbl.Rows[0]["systemtime"]);

            db.CloseSqlConnect();

            return sysTime;

        }

        /// <summary>DataSetにエクスポートユーザー情報をセット</summary>
        /// <param name="ds">DataSet</param>
        private static void setExportUserInfo(DataSet ds)
        {
            DataTable dt = new DataTable("UserInfo");
            dt.Columns.Add("user_name");
            dt.Columns.Add("type");
            DataRow dr = dt.NewRow();
            dr["user_name"] = LoginSetting.UserName;
            dr["type"] = LoginSetting.Authority;
            dt.Rows.Add(dr);
            DataTable[] tables = new DataTable[ds.Tables.Count];
            for (int i = 0; i < ds.Tables.Count; i++)
            {
                tables[i] = ds.Tables[i];
            }
            ds.Tables.Clear();
            ds.Tables.Add(dt);
            foreach (DataTable table in tables)
            {
                ds.Tables.Add(table);
            }

        }

        /// <summary>ＤＢにインポート</summary>
        /// <param name="ds">DataSet</param>
        /// <param name="overrideFlag">上書きフラグ</param>
        public static Consts.ImportResultType ImportForm(DataSet ds, bool overrideFlag)
        {
            setKeyForDoubleCheck();
            setKeyForRelationCheck();

            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            db.BeginTransaction();
            String insertSql = "";
            foreach (DataTable dt in ds.Tables)
            {
                if (!dt.TableName.Equals("UserInfo"))
                {
                    insertSql = createInserSql(dt);
                    foreach (DataRow dr in dt.Rows)
                    {
                        if (!checkDouble(dr, db, overrideFlag))
                        {
                            db.TransactionRollback();
                            db.CloseSqlConnect();
                            return Consts.ImportResultType.DubleKeyErr;
                        }

                        if (!checkRelation(ds, dr, db))
                        {
                            db.TransactionRollback();
                            db.CloseSqlConnect();
                            return Consts.ImportResultType.RelationErr;
                        }

                        List<ComSqlParam> sqlParams = new List<ComSqlParam>();
                        foreach (DataColumn dc in dt.Columns)
                        {
                            if (!dc.ColumnName.Equals("valid_flag"))
                            {
                                sqlParams.Add(new ComSqlParam(DbType.String, "@" + dc.ColumnName, dr[dc.ColumnName]));
                            }
                            else
                            {
                                sqlParams.Add(new ComSqlParam(DbType.String, "@" + dc.ColumnName, "0"));
                            }

                        }
                        db.ExecuteNonQuery(insertSql, sqlParams);
                    }
                }
            }
            db.TransactionCommit();
            db.CloseSqlConnect();
            return Consts.ImportResultType.Success;
        }

        /// <summary>インポート重複テーブル情報セット</summary>
        private static void setKeyForDoubleCheck()
        {
            if (!SET_DOUBLE_KEY)
            {
                KEY_FOR_DOUBLE_CHECK.Add("ja_calendar_control_table",
                    new ImportDoubleRelationCheck("ja_calendar_control_table", new String[] { "calendar_id", "update_date" }, null, null));
                KEY_FOR_DOUBLE_CHECK.Add("ja_schedule_control_table",
                    new ImportDoubleRelationCheck("ja_schedule_control_table", new String[] { "schedule_id", "update_date" }, null, null));
                KEY_FOR_DOUBLE_CHECK.Add("ja_jobnet_control_table",
                    new ImportDoubleRelationCheck("ja_jobnet_control_table", new String[] { "jobnet_id", "update_date" }, null, null));
                SET_DOUBLE_KEY = true;
            }
        }

        /// <summary>インポート関連テーブル情報セット</summary>
        private static void setKeyForRelationCheck()
        {
            if (!SET_RELATE_KEY)
            {
                KEY_FOR_RELATION_CHECK.Add("ja_schedule_detail_table",
                    new ImportDoubleRelationCheck("ja_schedule_detail_table", new String[] { "calendar_id" }, "ja_calendar_control_table", new String[] { "calendar_id" }));
                KEY_FOR_RELATION_CHECK.Add("ja_schedule_jobnet_table",
                    new ImportDoubleRelationCheck("ja_schedule_jobnet_table", new String[] { "jobnet_id" }, "ja_jobnet_control_table", new String[] { "jobnet_id" }));
                KEY_FOR_RELATION_CHECK.Add("ja_icon_jobnet_table",
                    new ImportDoubleRelationCheck("ja_icon_jobnet_table", new String[] { "link_jobnet_id" }, "ja_jobnet_control_table", new String[] { "jobnet_id" }));
                KEY_FOR_RELATION_CHECK.Add("ja_icon_task_table",
                    new ImportDoubleRelationCheck("ja_icon_task_table", new String[] { "submit_jobnet_id" }, "ja_jobnet_control_table", new String[] { "jobnet_id" }));
                KEY_FOR_RELATION_CHECK.Add("ja_icon_extjob_table",
                    new ImportDoubleRelationCheck("ja_icon_extjob_table", new String[] { "command_id" }, "ja_define_extjob_table", new String[] { "command_id" }));
                KEY_FOR_RELATION_CHECK.Add("ja_icon_value_table",
                    new ImportDoubleRelationCheck("ja_icon_value_table", new String[] { "value_name" }, "ja_define_value_jobcon_table", new String[] { "value_name" }));
                SET_RELATE_KEY = true;
            }

        }

        /// <summary>インポート追加ＳＱＬ作成</summary>
        /// <param name="dt">DataTable</param>
        /// <return>InsertSQL文</return>
        private static String createInserSql(DataTable dt)
        {
            String insertSql;
            int collumnCount = 0;
            insertSql = "insert into " + dt.TableName + " (";
            foreach (DataColumn dc in dt.Columns)
            {
                if (collumnCount > 0) insertSql = insertSql + ",";
                insertSql = insertSql + dc.ColumnName;
                collumnCount++;
            }
            insertSql = insertSql + ") values (";
            collumnCount = 0;
            foreach (DataColumn dc in dt.Columns)
            {
                if (collumnCount > 0) insertSql = insertSql + ",";
                insertSql = insertSql + "?";
                collumnCount++;
            }
            insertSql = insertSql + ")";
            return insertSql;
        }


        /// <summary>上書き許可の場合、重複チェックして、存在する場合、削除する</summary>
        /// <param name="dr">データ</param>
        /// <param name="db">ＤＢ</param>
        private static bool checkDouble(DataRow dr, DBConnect db, bool overrideFlag)
        {
            String checkSql = "";
            String deleteSql = "";
            if (KEY_FOR_DOUBLE_CHECK.ContainsKey(dr.Table.TableName))
            {
                ImportDoubleRelationCheck checkInfo = (ImportDoubleRelationCheck)KEY_FOR_DOUBLE_CHECK[dr.Table.TableName];
                checkSql = "select * from " + dr.Table.TableName + " where ";
                deleteSql = "delete from " + dr.Table.TableName + " where ";
                for (int i = 0; i < checkInfo.Keys.Length; i++)
                {
                    if (i > 0)
                    {
                        checkSql = checkSql + " and ";
                        deleteSql = deleteSql + " and ";
                    }
                    checkSql = checkSql + checkInfo.Keys[i] + "=?";
                    deleteSql = deleteSql + checkInfo.Keys[i] + "=?";
                }
                //OdbcCommand command = new OdbcCommand(checkSql, connection, tran);
                List<ComSqlParam> sqlParams = new List<ComSqlParam>();

                for (int i = 0; i < checkInfo.Keys.Length; i++)
                {
                    sqlParams.Add(new ComSqlParam(DbType.String, "@" + checkInfo.Keys[i], dr[checkInfo.Keys[i]]));
                }
                DataTable dt = db.ExecuteQuery(checkSql, sqlParams);
                if ((int)dt.Rows.Count > 0)
                {
                    if (!overrideFlag) return false;
                    db.ExecuteNonQuery(deleteSql, sqlParams);
                }
            }
            return true;
        }

        /// <summary>関連データが存在するか確認</summary>
        /// <param name="ds">DataSet</param>
        /// <param name="dr">データ</param>
        /// <param name="db">ＤＢ</param>
        /// <return>関連チェック結果</return>
        private static bool checkRelation(DataSet ds, DataRow dr, DBConnect db)
        {
            if (KEY_FOR_RELATION_CHECK.ContainsKey(dr.Table.TableName))
            {
                ImportDoubleRelationCheck checkInfo = (ImportDoubleRelationCheck)KEY_FOR_RELATION_CHECK[dr.Table.TableName];
                if (!checkRelationForDB(checkInfo, ds, dr, db))
                {
                    return checkRelationForDataSet(checkInfo, ds, dr);
                }
            }
            return true;
        }

        /// <summary>ＤＢ上関連データが存在するか確認</summary>
        /// <param name="checkInfo">関連情報</param>
        /// <param name="ds">DataSet</param>
        /// <param name="dr">データ</param>
        /// <param name="db">ＤＢ</param>
        /// <return>ＤＢ関連チェック結果</return>
        private static bool checkRelationForDB(ImportDoubleRelationCheck checkInfo, DataSet ds, DataRow dr, DBConnect db)
        {
            String checkSql = "select * from " + checkInfo.RefTableName + " where ";
            for (int i = 0; i < checkInfo.RefKeys.Length; i++)
            {
                if (i > 0)
                {
                    checkSql = checkSql + " and ";
                }
                checkSql = checkSql + checkInfo.RefKeys[i] + "=?";
            }
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            for (int i = 0; i < checkInfo.RefKeys.Length; i++)
            {
                sqlParams.Add(new ComSqlParam(DbType.String, "@" + checkInfo.RefKeys[i], dr[checkInfo.Keys[i]]));
            }

            DataTable dtDB = db.ExecuteQuery(checkSql, sqlParams);

            if (dtDB.Rows.Count == 0)
            {
                return false;
            }
            return true;
        }

        /// <summary>インポートデータ上関連データが存在するか確認</summary>
        /// <param name="checkInfo">関連情報</param>
        /// <param name="ds">DataSet</param>
        /// <param name="dr">データ</param>
        /// <return>インポートデータ関連チェック結果</return>
        private static bool checkRelationForDataSet(ImportDoubleRelationCheck checkInfo, DataSet ds, DataRow dr)
        {
            foreach (DataTable dt in ds.Tables)
            {
                if (dt.TableName.Equals(checkInfo.RefTableName))
                {
                    String checkSql = "";
                    for (int i = 0; i < checkInfo.RefKeys.Length; i++)
                    {
                        if (i > 0)
                        {
                            checkSql = checkSql + ",";
                        }
                        checkSql = checkSql + checkInfo.RefKeys[i] + "='" + dr[checkInfo.Keys[i]] + "'";
                    }
                    DataRow[] selectedRows = dt.Select(checkSql);
                    if (selectedRows.Length == 0)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        /// <summary>インポート権限チェック</summary>
        /// <param name="dr">データ</param>
        /// <return>権限が有りの場合True、権限がない場合False</return>
        public static bool checkImportAuth(DataRow dr)
        {
            if(Convert.ToString(dr["user_name"]).Equals(LoginSetting.UserName))
                return true;
            DBConnect db = new DBConnect(LoginSetting.ConnectStr);
            db.CreateSqlConnect();
            String sql = "select UG1.usrgrpid from users U1,users U2,users_groups AS UG1,users_groups AS UG2 where U1.alias=? and U2.alias=? and UG1.userid=U1.userid and UG2.userid=U2.userid and UG1.usrgrpid = UG2.usrgrpid";
            List<ComSqlParam> sqlParams = new List<ComSqlParam>();
            sqlParams.Add(new ComSqlParam(DbType.String, "@alias", dr["user_name"]));
            sqlParams.Add(new ComSqlParam(DbType.String, "@alias", LoginSetting.UserName));
            DataTable dt = db.ExecuteQuery(sql, sqlParams);
            if (dt.Rows.Count > 0) return true;
            return false;
        }

        #endregion

    }
}
