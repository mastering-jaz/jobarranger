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
using System.Text;
using System.Text.RegularExpressions;
using System.Collections.Generic;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 旭 2012/10/15 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.Common
{
    /// <summary>
    /// 共通チェックユーティリティクラス.
    /// </summary>
    public class CheckUtil
    {
        #region フィールド

        /// <summary>半角英数字とハイフン</summary>
        private const string MATCH_HANKAKU_HYPHEN = "^[0-9a-zA-Z\\-]*$";

        /// <summary>半角数字</summary>
        private const string MATCH_HANKAKU = "^[0-9]*$";

        /// <summary>ASCII</summary>
        private const string MATCH_ASCII = "^[\\x00-\\x7F]*$";

        /// <summary>全半角英数字</summary>
        private const string MATCH_ZENKAKU_HANKAKU = "^[0-9a-zA-Z０-９ａ-ｚＡ-Ｚ]*$";

        /// <summary>半角数字、カンマ、ハイフン</summary>
        private const string MATCH_HANKAKU_COMMA_HYPHEN = "^[0-9\\,\\-]*$";

        /// <summary>半角英数字とハイフン、スラッシュ</summary>
        private const string MATCH_HANKAKU_HYPHEN_SLASH = "^[0-9a-zA-Z\\-\\/]*$";

        /// <summary>半角英数字とハイフン、アンダーバー</summary>
        private const string MATCH_HANKAKU_HYPHEN_UNDERBAR = "^[0-9a-zA-Z_-]*$";

        /// <summary>半角英数字とアンダーバー</summary>
        private const string MATCH_HANKAKU_UNDERBAR = "^[0-9a-zA-Z_]*$";

        /// <summary>予約語（START）</summary>
        private const string HOLD_START = "START";

        public static Encoding EncSJis = Encoding.GetEncoding("shift_jis");

        #endregion

        #region publicメソッド

        /// <summary>必須チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：null または空の文字列の場合」「False：それ以外」</return>
        public static bool IsNullOrEmpty(string str)
        {
            if (str == null || str.Length == 0)
            {
                return true;
            }
            return false;
        }

        /// <summary>ASCII文字チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：ASCII文字」「False：それ以外」</return>
        public static bool IsASCIIStr(string str)
        {
            if (str == null || str.Length == 0
                || Regex.IsMatch(str, MATCH_ASCII))
            {
                return true;
            }
            return false;
        }

        /// <summary>文字列桁数チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <param name="intLen">指定された長さ</param>
        /// <return>「True：指定された長さ以上の文字列」「False：指定された長さ以内の文字列」</return>
        public static bool IsGetaLenOver(string str, int intLen)
        {
            if (str == null || str.Length == 0)
            {
                return false;
            }
            return str.Length > intLen;
        }

        /// <summary>文字列バイト数チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <param name="intLen">指定されたバイト長</param>
        /// <return>「True：指定された長さ以上の文字列」「False：指定された長さ以内の文字列」</return>
        public static bool IsLenOver(string str, int intLen)
        {
            if (str == null || str.Equals(""))
            {
                return false;
            }
            return (EncSJis.GetByteCount(str)) > intLen;
        }

        /// <summary>文字列バイト数チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <param name="intLen">指定されたバイト長</param>
        /// <return>「True：指定された長さ以下の文字列」「False：指定された長さ以上の文字列」</return>
        public static bool IsLenUnder(string str, int intLen)
        {
            if (str == null || str.Equals(""))
            {
                return false;
            }
            return (EncSJis.GetByteCount(str)) < intLen;
        }

        /// <summary>文字列桁数チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <param name="intLen">指定された長さ</param>
        /// <return>「True：指定された長さ」「False：指定された長さではない」</return>
        public static bool IsLen(string str, int intLen)
        {
            if (str == null || str.Length == 0)
            {
                return false;
            }
            return str.Length == intLen;
        }

        /// <summary>半角英数字とハイフンチェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：半角英数字とハイフン」「False：それ以外」</return>
        public static bool IsHankakuStrAndHyphen(string str)
        {
            if (str == null || str.Length == 0
                || Regex.IsMatch(str, MATCH_HANKAKU_HYPHEN))
            {
                return true;
            }
            return false;
        }

        /// <summary>全半角英数字チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：全半角英数字」「False：それ以外」</return>
        public static bool IsZenkakuStrAndHankakuStr(string str)
        {
            if (str == null || str.Length == 0
                || Regex.IsMatch(str, MATCH_ZENKAKU_HANKAKU))
            {
                return true;
            }
            return false;
        }

        /// <summary>半角数字チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：半角数字」「False：それ以外」</return>
        public static bool IsHankakuNum(string str)
        {
            if (str == null || str.Length == 0
                || Regex.IsMatch(str, MATCH_HANKAKU))
            {
                return true;
            }
            return false;
        }

        /// <summary>半角数字、カンマ、ハイフンチェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：半角数字、カンマ、ハイフン」「False：それ以外」</return>
        public static bool IsHankakuNumAndCommaAndHyphen(string str)
        {
            if (str == null || str.Length == 0
                || Regex.IsMatch(str, MATCH_HANKAKU_COMMA_HYPHEN))
            {
                return true;
            }
            return false;
        }

        /// <summary>カンマ、およびハイフンの前後が数字チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：半角数字カンマ、およびハイフンの前後が数字」「False：それ以外」</return>
        public static bool IsHankakuNumBeforeOrAfterCommaAndHyphen(string str)
        {
            if (str == null || str.Length == 0)
            {
                return true;
            }
            String[] array = str.Split(new char[2]{',','-'});
            foreach (String i in array)
            {
                if (i.Length == 0 || !IsHankakuNum(i))
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>半角英数字とハイフン、スラッシュチェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：半角英数字、ハイフン、スラッシュ」「False：それ以外」</return>
        public static bool IsHankakuStrAndHyphenAndSlash(string str)
        {
            if (str == null || str.Length == 0
                || Regex.IsMatch(str, MATCH_HANKAKU_HYPHEN_SLASH))
            {
                return true;
            }
            return false;
        }

        /// <summary>半角英数字とアンダーバー、最初文字数値以外チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：半角英数字、アンダーバー、最初文字数値以外」「False：それ以外」</return>
        public static bool IsHankakuStrAndUnderbarAndFirstNotNum(string str)
        {
            if (str == null || str.Length == 0
                || (Regex.IsMatch(str, MATCH_HANKAKU_UNDERBAR) && !IsHankakuNum(str.Substring(0, 1))))
            {
                return true;
            }
            return false;
        }

        /// <summary>半角英数字とハイフン、アンダーバーチェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：半角英数字、ハイフン、アンダーバー」「False：それ以外」</return>
        public static bool IsHankakuStrAndHyphenAndUnderbar(string str)
        {
            if (str == null || str.Length == 0
                || Regex.IsMatch(str, MATCH_HANKAKU_HYPHEN_UNDERBAR))
            {
                return true;
            }
            return false;
        }

        /// <summary>予約語（START）チェック処理</summary>
        /// <param name="str">チェックする文字列</param>
        /// <return>「True：予約語（START）」「False：それ以外」</return>
        public static bool IsHoldStrSTART(string str)
        {
            if (str == null || str.Length == 0)
            {
                return false;
            }
            if (HOLD_START.Equals(str))
            {
                return true;
            }
            return false;
        }

        /// <summary>対象のバイト長度を取得</summary>
        /// <param name="strValue">チェックする文字列</param>
        /// <returns>対象のバイト長度</returns>
        public static int Get_ByteLength(string strValue)
        {
            Encoding unEncoding = Encoding.Default;
            int intLength = 0;
            intLength = unEncoding.GetByteCount(strValue);
            return intLength;
        }

        public static bool isExistGroupId(List<Decimal> loginUserGroupList, List<Decimal> objectUserGroupList)
        {
            foreach (Decimal userGroupId in loginUserGroupList)
            {
                if (objectUserGroupList.Contains(userGroupId)) return true;
            }
            return false;
        }

        #endregion
    }
}
