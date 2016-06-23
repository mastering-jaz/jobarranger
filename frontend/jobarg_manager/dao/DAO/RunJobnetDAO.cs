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
    /// <>
    /// ユーザーテーブルのDAOクラス
    /// </>
    public class RunJobnetDAO : BaseDAO
    {
        #region フィールド


        private string _tableName = "ja_run_jobnet_table";

        private string[] _primaryKey = { "inner_jobnet_id" };

        private string _selectSql = "select * from ja_run_jobnet_table where 0!=0";

        private string _selectSqlByPk = "select * from ja_run_jobnet_table " +
                                        "where inner_jobnet_id = ? ";


        private DBConnect _db = null;

        #endregion

        #region コンストラクタ

        public RunJobnetDAO(DBConnect db)
        {
            _db = db;
        }
        #endregion

        #region プロパティ

        /// <>　テーブル名前 </>
        public override string TableName
        {
            get
            {
                return _tableName;
            }
        }

        /// <>　キー </>
        public override string[] PrimaryKey
        {
            get
            {
                return _primaryKey;
            }
        }

        /// <> 検索用のSQL </>
        public override string SelectSql
        {
            get
            {
                return _selectSql;
            }
        }

        /// <> 検索条件を指定したSQL文 </>
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
        /// <> テーブルの構築</>
        /// <return>テーブルの結構</return>
        //************************************************************************
        public DataTable GetEmptyTable()
        {
            _db.CreateSqlConnect();

            DataTable dt = _db.ExecuteQuery(SelectSql);

            return dt;

        }

        //************************************************************************
        /// <> データの取得</>
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

        #endregion
    }
}
