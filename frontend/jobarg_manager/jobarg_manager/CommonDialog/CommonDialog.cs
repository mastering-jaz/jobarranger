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
using System.Windows;


//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 郭 暁宇 2012/10/15 新規作成<BR>                    *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.JobController
{
    /// <summary>
    /// 共通ダイアログ
    /// </summary>
    public class CommonDialog
    {
        #region フィールド

        /// <summary>エラーダイアログのタイトル</summary>
        private static String errDialogTitle = Properties.Resources.Title_Msgbox_Error;

        /// <summary>終了確認ダイアログのタイトル</summary>
        private static String endDialogTitle = Properties.Resources.Title_Msgbox_End;

        /// <summary>終了確認ダイアログのメッセージ</summary>
        private static String endDialogMessage = Properties.Resources.MSG_COMMON_001;

        /// <summary>編集登録確認ダイアログのタイトル</summary>
        private static String editRegistDialogTitle = Properties.Resources.Title_Msgbox_Edit;

        /// <summary>編集登録確認ダイアログのメッセージ</summary>
        private static String editRegistMessage = Properties.Resources.MSG_COMMON_002;

        /// <summary>バージョンダイアログのタイトル</summary>
        private static String versionDialogTitle = Properties.Resources.Title_Msgbox_Head;

        /// <summary>バージョンダイアログのメッセージ</summary>
        private static String versionDialogMessage = Properties.Resources.MSG_COMMON_003;

        /// <summary>削除確認ダイアログのタイトル</summary>
        private static String deleteDialogTitle = Properties.Resources.Title_Msgbox_Delete;

        /// <summary>削除確認ダイアログのメッセージ</summary>
        private static String deleteMessage = Properties.Resources.MSG_COMMON_004;

        /// <summary>ジョブネット起動ダイアログのメッセージ</summary>
        private static String jobnetStartMessage = Properties.Resources.MSG_COMMON_005;

        /// <summary>削除確認ダイアログのタイトル</summary>
        private static String jobnetStartDialogTitle = Properties.Resources.Title_Msgbox_Jobnet_Start;

        /// <summary>キャンセル確認ダイアログのタイトル</summary>
        private static String cancelDialogTitle = Properties.Resources.Title_Msgbox_Cancel;

        /// <summary>キャンセル確認ダイアログのメッセージ</summary>
        private static String cancelMessage = Properties.Resources.MSG_COMMON_008;

        #endregion

        #region publicメソッド
        /// <summary>
        /// エラーダイアログ
        /// </summary>
        /// <param name="msgId">メッセージＩＤ</param>
        public static MessageBoxResult ShowErrorDialogFromMessage(String msg)
        {
            return MessageBox.Show(msg, errDialogTitle, MessageBoxButton.OK);
        }

        /// <summary>
        /// エラーダイアログ
        /// </summary>
        /// <param name="msgId">メッセージＩＤ</param>
        public static MessageBoxResult ShowErrorDialog(String msgId)
        {
            string msg = MessageUtil.GetMsgById(msgId);
            return MessageBox.Show(msg, errDialogTitle, MessageBoxButton.OK);
        }

        /// <summary>
        /// エラーダイアログ
        /// </summary>
        /// <param name="msgId">メッセージＩＤ</param>
        /// <param name="ChangeMsg">置換文字列</param>
        public static void ShowErrorDialog(String msgId, string[] ChangeMsg)
        {
            string msg = MessageUtil.GetMsgById(msgId);
            msg = MessageUtil.GetReplaceMessage(msg, ChangeMsg);
            MessageBox.Show(msg, errDialogTitle, MessageBoxButton.OK);
        }

        /// <summary>
        /// 終了確認ダイアログ
        /// </summary>
        public static MessageBoxResult ShowEndDialog()
        {
            return System.Windows.MessageBox.Show(endDialogMessage, endDialogTitle,
                                   System.Windows.MessageBoxButton.YesNo,
                                   System.Windows.MessageBoxImage.None, MessageBoxResult.No);
        }

        /// <summary>
        /// 編集登録確認ダイアログ
        /// </summary>
        public static MessageBoxResult ShowEditRegistDialog()
        {
            return System.Windows.MessageBox.Show(editRegistMessage, editRegistDialogTitle,
                                   System.Windows.MessageBoxButton.YesNo);
        }

        /// <summary>
        /// キャンセル確認ダイアログ
        /// </summary>
        public static MessageBoxResult ShowCancelDialog()
        {
            return System.Windows.MessageBox.Show(cancelMessage, cancelDialogTitle,
                                   System.Windows.MessageBoxButton.YesNo);
        }

        /// <summary>
        /// バージョンダイアログ
        /// </summary>
        public static void ShowVersionDialog()
        {
            jp.co.ftf.jobcontroller.JobController.JobArrangerMessageBox.Show(versionDialogTitle, versionDialogMessage,
                            System.Windows.MessageBoxButton.OK);
        }

        /// <summary>
        /// 削除確認ダイアログ
        /// </summary>
        public static MessageBoxResult ShowDeleteDialog()
        {
            return System.Windows.MessageBox.Show(deleteMessage, deleteDialogTitle,
                                   System.Windows.MessageBoxButton.YesNo,
                                  System.Windows.MessageBoxImage.None, MessageBoxResult.No);
        }

        /// <summary>
        /// ジョブネット起動ダイアログ
        /// </summary>
        public static MessageBoxResult ShowJobnetStartDialog()
        {
            return System.Windows.MessageBox.Show(jobnetStartMessage, jobnetStartDialogTitle,
                                   System.Windows.MessageBoxButton.YesNo,
                                   System.Windows.MessageBoxImage.None, MessageBoxResult.No);
        }
        #endregion

        #region コンストラクタ
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CommonDialog()
        {
            //
            // TODO: コンストラクタ ロジックをここに追加してください。

            //
        }
        #endregion
    }
}
