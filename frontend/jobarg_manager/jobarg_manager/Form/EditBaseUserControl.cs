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
using System.Text;
using System.Data;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using jp.co.ftf.jobcontroller.Common;
using jp.co.ftf.jobcontroller.DAO;

//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author KIM 2012/11/15 新規作成<BR>                            *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.JobController
{
    /// <summary>
    /// コントロールの共通クラス.
    /// </summary>
    public abstract class EditBaseUserControl : BaseUserControl
    {

        #region フィールド

        /// <summary>親</summary>
        public abstract JobArrangerWindow ParantWindow { get;  set;}

        /// <summary>成功フラグ</summary>
        public abstract bool SuccessFlg { get; set; }

        /// <summary>ヘルスチェックフラグ</summary>
        public abstract bool HealthCheckFlag { get; set; }

        /// <summary>テキストボックス半角制御</summary>
        public HankakuTextChangeEvent HankakuTextChangeEvent = new HankakuTextChangeEvent();

        #endregion

        #region コンストラクタ

        public EditBaseUserControl()
        {
        }

        #endregion

        #region publicメソッド

        public virtual bool HasEditedCheck()
        {
            return true;
        }

        public virtual bool RegistObject()
        {
            return true;
        }

        public virtual void Rollback()
        {
        }

        public virtual void Commit()
        {
        }

        public virtual void ResetTree()
        {
        }

        #endregion

    }
}
