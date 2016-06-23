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
using System.Windows;
using jp.co.ftf.jobcontroller.Common;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 偉 2012/10/04 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.JobEdit
{
    /// <summary>
    /// フローインターフェイス 
    /// </summary>
    public interface IFlow
    {
        #region プロパティ

        /// <summary>選択判定</summary>
        bool IsSelectd { get; set; }

        /// <summary>開始点</summary>
        Point BeginPosition { get; set; }

        /// <summary>終了点</summary>
        Point EndPosition { get; set; }

        /// <summary>開始点の連接タイプ（上、下、左、右）</summary>
        ConnectType BeginConType { get; set; }

        /// <summary>終了点の連接タイプ（上、下、左、右）</summary>
        ConnectType EndConType { get; set; }

        /// <summary>コンテナ</summary>
        IContainer Container { get; set; }

        /// <summary>開始アイコン</summary>
        IRoom BeginItem { get; set; }

        /// <summary>終了アイコン</summary>
        IRoom EndItem { get; set; }

        #endregion

        #region メッソド

        /// <summary>Trueをセット</summary>
        void SetTrue(Consts.EditType editType);

        /// <summary>Falseアイコン</summary>
        void SetFalse(Consts.EditType editType);

        /// <summary>条件の位置をセット</summary>
        void setRuleNameControlPosition();

        #endregion
    }
}