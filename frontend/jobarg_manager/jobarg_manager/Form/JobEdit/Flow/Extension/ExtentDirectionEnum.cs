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

//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 偉 2012/10/27 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.JobEdit
{
    /// <summary>曲線計算用の列挙型</summary>
    public enum ExtentDirectionEnum
    {
        /// <summary>第一象限</summary>
        Quadrant1 = 0,

        /// <summary>第二象限</summary>
        Quadrant2 = 1,

        /// <summary>第三象限</summary>
        Quadrant3 = 2,

        /// <summary>第四象限</summary>
        Quadrant4 = 3,

        /// <summary>X軸の正の部分</summary>
        XB = 4,

        /// <summary>X軸の負の部分</summary>
        XL = 5,

        /// <summary>Y軸の正の部分</summary>
        YB = 6,

        /// <summary>Y軸の負の部分</summary>
        YL = 7,

        /// <summary>原点</summary>
        Origin = 9 
    }
}
