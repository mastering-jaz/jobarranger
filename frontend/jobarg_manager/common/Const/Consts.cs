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
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 偉 2012/10/15 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.Common
{
    public static class Consts
    {

        #region フィールド

        #region 画面ID
        /// <summary>1．ログイン画面</summary>
        public static readonly String WINDOW_100 = "WINDOW_NAME_100";

        /// <summary>2．オブジェクト一覧画面</summary>
        public static readonly String WINDOW_200 = "WINDOW_NAME_200";

        /// <summary>2-1．カレンダー編集画面</summary>
        public static readonly String WINDOW_210 = "WINDOW_NAME_210";

        /// <summary>2-1-1．稼働日ファイル読込画面</summary>
        public static readonly String WINDOW_211 = "WINDOW_NAME_211";

        /// <summary>2-1-2．カレンダー初期登録画面</summary>
        public static readonly String WINDOW_212 = "WINDOW_NAME_212";

        /// <summary>2-2．スケジュール編集画面</summary>
        public static readonly String WINDOW_220 = "WINDOW_NAME_220";

        /// <summary>2-2-1．起動時刻登録画面</summary>
        public static readonly String WINDOW_221 = "WINDOW_NAME_221";

        /// <summary>2-2-2．ジョブネット登録画面</summary>
        public static readonly String WINDOW_222 = "WINDOW_NAME_222";

        /// <summary>2-3．ジョブ編集画面<summary>
        public static readonly String WINDOW_230 = "WINDOW_NAME_230";

        /// <summary>2-3-1．ジョブアイコン設定画面<summary>
        public static readonly String WINDOW_231 = "WINDOW_NAME_231";

        /// <summary>2-3-2．条件分岐アイコン設定画面<summary>
        public static readonly String WINDOW_232 = "WINDOW_NAME_232";

        /// <summary>2-3-3．ジョブコントローラ変数アイコン設定画面<summary>
        public static readonly String WINDOW_233 = "WINDOW_NAME_233";

        /// <summary>2-3-4．拡張ジョブアイコン設定画面<summary>
        public static readonly String WINDOW_234 = "WINDOW_NAME_234";

        /// <summary>2-3-5．終了アイコン設定画面<summary>
        public static readonly String WINDOW_235 = "WINDOW_NAME_235";

        /// <summary>2-3-6．計算アイコン設定画面<summary>
        public static readonly String WINDOW_236 = "WINDOW_NAME_236";

        /// <summary>2-3-7．タスクアイコン設定画面<summary>
        public static readonly String WINDOW_237 = "WINDOW_NAME_237";

        /// <summary>2-3-8．情報取得アイコン設定画面<summary>
        public static readonly String WINDOW_238 = "WINDOW_NAME_238";

        /// <summary>2-3-9．その他のアイコン設定画面<summary>
        public static readonly String WINDOW_239 = "WINDOW_NAME_239";

        /// <summary>2-4．エクスポート画面<summary>
        public static readonly String WINDOW_240 = "WINDOW_NAME_240";

        /// <summary>2-5．インポート画面<summary>
        public static readonly String WINDOW_250 = "WINDOW_NAME_250";

        /// <summary>3．実行ジョブ管理画面<summary>
        public static readonly String WINDOW_300 = "WINDOW_NAME_300";

        /// <summary>3-１．実行ジョブ詳細画面<summary>
        public static readonly String WINDOW_310 = "WINDOW_NAME_310";

        /// <summary> 4- 0．ジョブネット起動中画面 </summary>
        public static readonly String WINDOW_400 = "WINDOW_NAME_400";
        #endregion

        #region メッセージID

        /// <summary>MSG_COMMON_001<summary>
        public static readonly String MSG_COMMON_001 = "MSG_COMMON_001";

        /// <summary>MSG_COMMON_002<summary>
        public static readonly String MSG_COMMON_002 = "MSG_COMMON_002";

        /// <summary>MSG_COMMON_003<summary>
        public static readonly String MSG_COMMON_003 = "MSG_COMMON_003";

        /// <summary>MSG_COMMON_004<summary>
        public static readonly String MSG_COMMON_004 = "MSG_COMMON_004";

        /// <summary>MSG_COMMON_005<summary>
        public static readonly String MSG_COMMON_005 = "MSG_COMMON_005";

        /// <summary>MSG_COMMON_006<summary>
        public static readonly String MSG_COMMON_006 = "MSG_COMMON_006";

        /// <summary>MSG_COMMON_007<summary>
        public static readonly String MSG_COMMON_007 = "MSG_COMMON_007";

        /// <summary>SYSERR_001<summary>
        public static readonly String SYSERR_001 = "SYSERR_001";

        /// <summary>SYSERR_002<summary>
        public static readonly String SYSERR_002 = "SYSERR_002";

        /// <summary>SYSERR_003<summary>
        public static readonly String SYSERR_003 = "SYSERR_003";

        /// <summary>SYSERR_004<summary>
        public static readonly String SYSERR_004 = "SYSERR_004";

        /// <summary>ERROR_COMMON_001<summary>
        public static readonly String ERROR_COMMON_001 = "ERROR_COMMON_001";

        /// <summary>ERROR_COMMON_002<summary>
        public static readonly String ERROR_COMMON_002 = "ERROR_COMMON_002";

        /// <summary>ERROR_COMMON_003<summary>
        public static readonly String ERROR_COMMON_003 = "ERROR_COMMON_003";

        /// <summary>ERROR_COMMON_004<summary>
        public static readonly String ERROR_COMMON_004 = "ERROR_COMMON_004";

        /// <summary>ERROR_COMMON_005<summary>
        public static readonly String ERROR_COMMON_005 = "ERROR_COMMON_005";

        /// <summary>ERROR_COMMON_006<summary>
        public static readonly String ERROR_COMMON_006 = "ERROR_COMMON_006";

        /// <summary>ERROR_COMMON_007<summary>
        public static readonly String ERROR_COMMON_007 = "ERROR_COMMON_007";

        /// <summary>ERROR_COMMON_008<summary>
        public static readonly String ERROR_COMMON_008 = "ERROR_COMMON_008";

        /// <summary>ERROR_COMMON_009<summary>
        public static readonly String ERROR_COMMON_009 = "ERROR_COMMON_009";

        /// <summary>ERROR_COMMON_010<summary>
        public static readonly String ERROR_COMMON_010 = "ERROR_COMMON_010";

        /// <summary>ERROR_COMMON_011<summary>
        public static readonly String ERROR_COMMON_011 = "ERROR_COMMON_011";

        /// <summary>ERROR_COMMON_012<summary>
        public static readonly String ERROR_COMMON_012 = "ERROR_COMMON_012";

        /// <summary>ERROR_COMMON_013<summary>
        public static readonly String ERROR_COMMON_013 = "ERROR_COMMON_013";

        /// <summary>ERROR_COMMON_014<summary>
        public static readonly String ERROR_COMMON_014 = "ERROR_COMMON_014";

        /// <summary>ERROR_COMMON_015<summary>
        public static readonly String ERROR_COMMON_015 = "ERROR_COMMON_015";

        /// <summary>ERROR_JOBEDIT_001<summary>
        public static readonly String ERROR_JOBEDIT_001 = "ERROR_JOBEDIT_001";

        /// <summary>ERROR_JOBEDIT_002<summary>
        public static readonly String ERROR_JOBEDIT_002 = "ERROR_JOBEDIT_002";

        /// <summary>ERROR_JOBEDIT_003<summary>
        public static readonly String ERROR_JOBEDIT_003 = "ERROR_JOBEDIT_003";

        /// <summary>ERROR_JOBEDIT_004<summary>
        public static readonly String ERROR_JOBEDIT_004 = "ERROR_JOBEDIT_004";

        /// <summary>ERROR_JOBEDIT_005<summary>
        public static readonly String ERROR_JOBEDIT_005 = "ERROR_JOBEDIT_005";

        /// <summary>ERROR_JOBEDIT_006<summary>
        public static readonly String ERROR_JOBEDIT_006 = "ERROR_JOBEDIT_006";

        /// <summary>ERROR_JOBEDIT_007<summary>
        public static readonly String ERROR_JOBEDIT_007 = "ERROR_JOBEDIT_007";

        /// <summary>ERROR_JOBEDIT_008<summary>
        public static readonly String ERROR_JOBEDIT_008 = "ERROR_JOBEDIT_008";

        /// <summary>ERROR_JOBEDIT_009<summary>
        public static readonly String ERROR_JOBEDIT_009 = "ERROR_JOBEDIT_009";

        /// <summary>ERROR_JOBEDIT_010<summary>
        public static readonly String ERROR_JOBEDIT_010 = "ERROR_JOBEDIT_010";

        /// <summary>ERROR_JOBEDIT_011<summary>
        public static readonly String ERROR_JOBEDIT_011 = "ERROR_JOBEDIT_011";

        /// <summary>ERROR_JOBEDIT_012<summary>
        public static readonly String ERROR_JOBEDIT_012 = "ERROR_JOBEDIT_012";

        /// <summary> ERROR_LOGIN_001 </summary>
        public static readonly String ERROR_LOGIN_001 = "ERROR_LOGIN_001";

        /// <summary> ERROR_LOGIN_002 </summary>
        public static readonly String ERROR_LOGIN_002 = "ERROR_LOGIN_002";

        /// <summary> ERROR_LOGIN_003 </summary>
        public static readonly String ERROR_LOGIN_003 = "ERROR_LOGIN_003";

        /// <summary> ERROR_LOGIN_004 </summary>
        public static readonly String ERROR_LOGIN_004 = "ERROR_LOGIN_004";

        /// <summary> ERROR_LOGIN_005 </summary>
        public static readonly String ERROR_LOGIN_005 = "ERROR_LOGIN_005";

        /// <summary> ERROR_LOGIN_006 </summary>
        public static readonly String ERROR_LOGIN_006 = "ERROR_LOGIN_006";

        /// <summary>ERROR_CALENDAR_001<summary>
        public static readonly String ERROR_CALENDAR_001 = "ERROR_CALENDAR_001";

        /// <summary>ERROR_CALENDAR_002<summary>
        public static readonly String ERROR_CALENDAR_002 = "ERROR_CALENDAR_002";

        /// <summary> ERROR_CALENDAR_FILE_READ_001 </summary>
        public static readonly String ERROR_CALENDAR_FILE_READ_001 = "ERROR_CALENDAR_FILE_READ_001";

        /// <summary>ERROR_SCHEDULE_001<summary>
        public static readonly String ERROR_SCHEDULE_001 = "ERROR_SCHEDULE_001";

        /// <summary>ERROR_SCHEDULE_002<summary>
        public static readonly String ERROR_SCHEDULE_002 = "ERROR_SCHEDULE_002";

        /// <summary>ERROR_SCHEDULE_003<summary>
        public static readonly String ERROR_SCHEDULE_003 = "ERROR_SCHEDULE_003";

        /// <summary>ERROR_SCHEDULE_004<summary>
        public static readonly String ERROR_SCHEDULE_004 = "ERROR_SCHEDULE_004";

        /// <summary> ERROR_IMPORT_001 </summary>
        public static readonly String ERROR_IMPORT_001 = "ERROR_IMPORT_001";

        /// <summary> ERROR_IMPORT_002 </summary>
        public static readonly String ERROR_IMPORT_002 = "ERROR_IMPORT_002";

        /// <summary> ERROR_IMPORT_003 </summary>
        public static readonly String ERROR_IMPORT_003 = "ERROR_IMPORT_003";

        /// <summary> ERROR_IMPORT_004 </summary>
        public static readonly String ERROR_IMPORT_004 = "ERROR_IMPORT_004";

        /// <summary> ERROR_IMPORT_005 </summary>
        public static readonly String ERROR_IMPORT_005 = "ERROR_IMPORT_005";

        /// <summary> ERROR_RUN_JOBNET_001 </summary>
        public static readonly String ERROR_RUN_JOBNET_001 = "ERROR_RUN_JOBNET_001";

        /// <summary> ERROR_BOOT_TIME_001 </summary>
        public static readonly String ERROR_BOOT_TIME_001 = "ERROR_BOOT_TIME_001";

        /// <summary> ERROR_BOOT_TIME_002 </summary>
        public static readonly String ERROR_BOOT_TIME_002 = "ERROR_BOOT_TIME_002";

        /// <summary> ERROR_BOOT_TIME_003 </summary>
        public static readonly String ERROR_BOOT_TIME_003 = "ERROR_BOOT_TIME_003";

        /// <summary> ERROR_BOOT_TIME_004 </summary>
        public static readonly String ERROR_BOOT_TIME_004 = "ERROR_BOOT_TIME_004";

        /// <summary> ERROR_JOBNET_LOAD_001 </summary>
        public static readonly String ERROR_JOBNET_LOAD_001 = "ERROR_JOBNET_LOAD_001";


        #endregion

        #region 処理ID
        /// <summary>PROCESS_001<summary>
        public static readonly String PROCESS_001 = "PROCESS_001";

        /// <summary>PROCESS_002<summary>
        public static readonly String PROCESS_002 = "PROCESS_002";

        /// <summary>PROCESS_003<summary>
        public static readonly String PROCESS_003 = "PROCESS_003";

        /// <summary>PROCESS_004<summary>
        public static readonly String PROCESS_004 = "PROCESS_004";

        /// <summary>PROCESS_005<summary>
        public static readonly String PROCESS_005 = "PROCESS_005";

        /// <summary>PROCESS_006<summary>
        public static readonly String PROCESS_006 = "PROCESS_006";

        /// <summary>PROCESS_007<summary>
        public static readonly String PROCESS_007 = "PROCESS_007";

        /// <summary>PROCESS_008<summary>
        public static readonly String PROCESS_008 = "PROCESS_008";

        /// <summary>PROCESS_009<summary>
        public static readonly String PROCESS_009 = "PROCESS_009";

        /// <summary>PROCESS_010<summary>
        public static readonly String PROCESS_010 = "PROCESS_010";

        /// <summary>PROCESS_011<summary>
        public static readonly String PROCESS_011 = "PROCESS_011";

        /// <summary>PROCESS_012<summary>
        public static readonly String PROCESS_012 = "PROCESS_012";

        /// <summary>PROCESS_013<summary>
        public static readonly String PROCESS_013 = "PROCESS_013";

        /// <summary>PROCESS_014<summary>
        public static readonly String PROCESS_014 = "PROCESS_014";

        /// <summary>PROCESS_015<summary>
        public static readonly String PROCESS_015 = "PROCESS_015";

        /// <summary>PROCESS_016<summary>
        public static readonly String PROCESS_016 = "PROCESS_016";

        /// <summary>PROCESS_017<summary>
        public static readonly String PROCESS_017 = "PROCESS_017";

        /// <summary>PROCESS_018<summary>
        public static readonly String PROCESS_018 = "PROCESS_018";

        /// <summary>PROCESS_019<summary>
        public static readonly String PROCESS_019 = "PROCESS_019";

        /// <summary>PROCESS_020<summary>
        public static readonly String PROCESS_020 = "PROCESS_020";

        /// <summary>PROCESS_021<summary>
        public static readonly String PROCESS_021 = "PROCESS_021";

        /// <summary>PROCESS_022<summary>
        public static readonly String PROCESS_022 = "PROCESS_022";

        /// <summary>PROCESS_023<summary>
        public static readonly String PROCESS_023 = "PROCESS_023";

        /// <summary>IMPORT_PATH<summary>
        public static String IMPORT_PATH = "";

        /// <summary>IMPORT_PATH<summary>
        public static String EXPORT_PATH = "";

        /// <summary>IMPORT_PATH<summary>
        public static String FILEREAD_PATH = "";

        #endregion

        /// <summary>動作モード</summary>
        public enum ActionMode { USE = 0, DEVELOP };

        /// <summary>オブジェクト種類</summary>
        public enum ObjectEnum
        {
            CALENDAR = 0,
            SCHEDULE,
            JOBNET
        }

        /// <summary>ジョブフローの編集タイプ</summary>
        public enum EditType { Add = 0, Modify, CopyVer, CopyNew, READ }

        /// <summary>ユーザー権限</summary>
        public enum AuthorityEnum
        {
            GENERAL = 1,
            ADMIN = 2,
            SUPER = 3
        }

        /// <summary>DBのタイプ</summary>
        public enum DBTYPE { MYSQL = 0, POSTGRES, NONE };

        /// <summary>オブジェクトの所有タイプ</summary>
        public enum ObjectOwnType { Owner = 0, Other };

        /// <summary>ジョブネット実行タイプ</summary>
        public enum RunTypeEnum {General = 0, Immediate, Reservation, Test, Time, Job}

        /// <summary>インポート実行結果タイプ</summary>
        public enum ImportResultType { Success = 0, DubleKeyErr, RelationErr }

        #endregion
    }
}
