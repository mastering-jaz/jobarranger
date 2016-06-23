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
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;

namespace jp.co.ftf.jobcontroller.JobController.Form.JobEdit
{
    public class HistoryData
    {
        #region フィールド
        #endregion

        #region コンストラクタ
        /// <summary>
        /// コンストラクタ 
        /// </summary>
        public HistoryData()
        {
        }
        #endregion

        #region プロパティ
        /// <summary>ジョブ管理テーブル</summary>
        public DataTable JobControlTable { get; set; }

        /// <summary>フロー管理テーブル</summary>
        public DataTable FlowControlTable { get; set; }

        /// <summary>計算アイコン設定テーブル</summary>
        public DataTable IconCalcTable { get; set; }

        /// <summary>終了アイコン設定テーブル</summary>
        public DataTable IconEndTable { get; set; }

        /// <summary>拡張ジョブアイコン設定テーブル</summary>
        public DataTable IconExtjobTable { get; set; }

        // <summary>条件分岐アイコン設定テーブル</summary>
        public DataTable IconIfTable { get; set; }

        // <summary>情報取得アイコン設定テーブル</summary>
        public DataTable IconInfoTable { get; set; }

        // <summary>ジョブネットアイコン設定テーブル</summary>
        public DataTable IconJobnetTable { get; set; }

        // <summary>ジョブアイコン設定テーブル</summary>
        public DataTable IconJobTable { get; set; }

        // <summary>ジョブコマンド設定テーブル</summary>
        public DataTable JobCommandTable { get; set; }

        // <summary>ジョブ変数設定テーブル</summary>
        public DataTable ValueJobTable { get; set; }

        // <summary>ジョブコントローラ変数設定テーブル</summary>
        public DataTable ValueJobConTable { get; set; }

        // <summary>タスクアイコン設定テーブル</summary>
        public DataTable IconTaskTable { get; set; }

        // <summary>ジョブコントローラ変数アイコン設定テーブル</summary>
        public DataTable IconValueTable { get; set; }

        // <summary>ジョブコントローラ変数定義テーブル</summary>
        public DataTable DefineValueJobconTable { get; set; }

        // <summary>拡張ジョブ定義テーブル</summary>
        public DataTable DefineExtJobTable { get; set; }

        // <summary>ファイル転送アイコン設定テーブル</summary>
        public DataTable IconFcopyTable { get; set; }

        // <summary>ファイル待ち合わせアイコン設定テーブル</summary>
        public DataTable IconFwaitTable { get; set; }

        // <summary>リブートアイコン設定テーブル</summary>
        public DataTable IconRebootTable { get; set; }

        // <summary>保留解除アイコン設定テーブル</summary>
        public DataTable IconReleaseTable { get; set; }

        public Hashtable SetedJobIds { get; set; }

        public Hashtable JobIdNos { get; set; }

        OperationType _operationType;
        public OperationType OperationType
        {
            get
            {
                return _operationType;
            }
        }
        jp.co.ftf.jobcontroller.JobController.Form.JobEdit.Container _container;
        public jp.co.ftf.jobcontroller.JobController.Form.JobEdit.Container Container
        {
            get
            {
                return _container;
            }
        }
        List<String> _beforeJobidList;
        public List<String> BeforeJobidList
        {
            get
            {
                if (_beforeJobidList == null)
                    _beforeJobidList = new List<String>();
                return _beforeJobidList;
            }
        }

        List<String> _afterJobidList;
        public List<String> AfterJobidList
        {
            get
            {
                if (_afterJobidList == null)
                    _afterJobidList = new List<String>();
                return _afterJobidList;
            }
        }

        Hashtable _beforeData;
        public Hashtable BeforeData
        {
            get
            {
                if (_beforeData == null)
                    _beforeData = new Hashtable();
                return _beforeData;
            }
        }
        Hashtable _afterData;
        public Hashtable AfterData
        {
            get
            {
                if (_afterData == null)
                    _afterData = new Hashtable();
                return _afterData;
            }
        }

        Hashtable _beforeSetedJobId;
        public Hashtable BeforeSetedJobId
        {
            get
            {
                if (_beforeSetedJobId == null)
                    _beforeSetedJobId = new Hashtable();
                return _beforeSetedJobId;
            }
            set
            {
                _beforeSetedJobId = value;
            }
        }
        Hashtable _afterSetedJobId;
        public Hashtable AfterSetedJobId
        {
            get
            {
                if (_afterSetedJobId == null)
                    _afterSetedJobId = new Hashtable();
                return _afterSetedJobId;
            }
            set
            {
                _afterSetedJobId = value;
            }
        }
        public String StartJobIdForFlow { get; set; }
        public String EndJobIdForFlow { get; set; }

        #endregion

        #region publicメソッド
        public void AddBeforeData(DataType dataType, DataRow row)
        {
            if(!BeforeData.Contains(dataType))
            {
                List<DataRow> list = new List<DataRow>();
                list.Add(row);
                BeforeData.Add(dataType, list);
            }
            else
            {
               ((List<DataRow>)BeforeData[dataType]).Add(row);
            }
        }

        public void AddData2ContainerTable(DataTable dt, DataType dataType)
        {
            if (BeforeData.Contains(dataType))
            {
                DataRow newRow;
                foreach (DataRow row in (List<DataRow>)BeforeData[dataType])
                {
                    newRow = dt.NewRow();
                    newRow.ItemArray = row.ItemArray;
                    dt.Rows.Add(newRow);
                }                          
            }
        }

        #endregion
    }
}
