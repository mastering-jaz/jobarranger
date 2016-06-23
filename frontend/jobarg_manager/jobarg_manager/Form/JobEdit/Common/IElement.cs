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
using System.Windows;
using System.Windows.Media;
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
    /// <summary>各ジョブタイプ</summary>
    public enum ElementType { START = 0, END, IF, ENV, JOB, JOBNET, MTS, MTE, LOOP, EXTJOB, CAL, TASK, INF, IFE, FCOPY, FWAIT, REBOOT, RELEASE, NONE  }

    /// <summary>線のタイプ：直線、曲線</summary>
    public enum FlowLineType { Line = 0, Curve }

    /// <summary>線の条件タイプ</summary>
    public enum FlowType { DEFAULT = 0, TRUE, FALSE }

    /// <summary>アイコンのフォーカス状態</summary>
    public enum IElementState { Focus = 0, UnFocus, Selected }

    /// <summary>線の移動タイプ</summary>
    public enum LineMoveType { None = 0, Begin, Center, End, Line };
 
    /// <summary>連接点のタイプ</summary>
    public enum ConnectType { LEFT = 0, RIGHT, TOP, BOTTOM };

    /// <summary>実行ジョブ状態</summary>
    public enum RunJobStatusType { None = 0, Prepare, During, Normal, RunErr, Abnormal,ForceStop };

    /// <summary>ジョブ実行処理フラグ</summary>
    public enum RunJobMethodType { NORMAL = 0, HOLD, SKIP, STOP, RERUN };

    /// <summary>ジョブ実行タイムアウト</summary>
    public enum RunJobTimeoutType { NORMAL = 0, TIMEOUT };

    /// <summary>画面操作種別</summary>
    public enum OperationType { NONE, ADD_ITEMS, DEL_ITEMS, ADD_FLOW, DEL_FLOW, SET_TRUE_FLOW, 
                                SET_FALSE_FLOW, UPDATE_ICON, MOVE_ITEM, 
                                SET_HOLD, SET_UNHOLD, SET_SKIP, SET_UNSKIP};

    /// <summary>データ種別</summary>
    public enum DataType
    {
        JOB_CONTROL, FLOW, JOB, COMMAND, VALUE_JOB, VALUE_JOBCON, JOBNET, 
        CAL, END, IF, ENV, EXTJOB, INF, TASK, FCOPY, FWAIT, REBOOT, RELEASE
    };


    /// <summary>
    /// ジョブインターフェイス
    /// </summary>
    public interface IElement
    {
        #region プロパティ
        /// <summary>コンテナ</summary>
        IContainer Container { get; set; }

        /// <summary>幅 </summary>
        double PicWidth { get; }

        /// <summary>高さ</summary>
        double PicHeight { get; }

        /// <summary>ジョブID</summary>
        string JobId { get; set; }

        /// <summary>ジョブ名</summary>
        string JobName { get; set; }

        /// <summary>上連接点</summary>
        Point TopConnectPosition { get; }

        /// <summary>下連接点</summary>
        Point BottomConnectPosition { get; }

        /// <summary>左連接点</summary>
        Point LeftConnectPosition { get; }

        /// <summary>右連接点</summary>
        Point RightConnectPosition { get; }

        /// <summary>内部実行処理用ジョブID</summary>
        string InnerJobId { get; set; }

        /// <summary>処理フラグ</summary>
        RunJobMethodType MethodType { get; set; }

        //Point HotspotLeft { get; set; }
        //Point HotspotTop { get; set; }
        //Point HotspotRight { get; set; }
        //Point HotspotBottom { get; set; }

        //AbstractEntity Entity { get; set; }

        #endregion

        #region メッソド

        /// <summary>選択色をセット</summary>
        void SetSelectedColor();

        /// <summary>アイコンの色をリセット</summary>
        void ResetInitColor();

        /// <summary>フォーカス</summary>
        void SetFocus();

        /// <summary>フォーカスをはずす</summary>
        void SetUnFocus();

        /// <summary>選択</summary>
        void SetSelected();

        /// <summary>部品区の色に初期化</summary>
        void InitSampleColor();

        /// <summary>状態色をセット</summary>
        void SetStatusColor(SolidColorBrush color);

        //Point GetHotspot(ConnectType hotspotType);

        //ConnectType GetNearHotspot(Point point);
        #endregion

    }
}

