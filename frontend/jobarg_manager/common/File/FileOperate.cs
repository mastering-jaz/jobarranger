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
using System.IO;
using System.Data;

//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 郭 暁宇 2012/10/15 新規作成<BR>                    *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.Common
{
    /// <summary>
    /// ファイルの操作クラス.
    /// </summary>
    public class FileOperate
    {
        #region フィールド

        /// <summary>ファイルの区切り</summary>
        private static char _seperator = ',';

        #endregion

        #region プロパティ

        public static char FileSeperator
        {
            get
            {
                return _seperator;
            }
            set
            {
                _seperator = value;
            }
        }

        #endregion

        #region publicメソッド

        /// <summary>ファイルの作成</summary>
        /// <param name="strFilePath">ファイルパス</param>
        public static void CreateFile(string strFilePath)
        {
            FileStream fs = null;

            try
            {
                if (!System.IO.File.Exists(strFilePath))
                {
                    fs = System.IO.File.Create(strFilePath);
                }
            }
            catch (Exception ex)
            {
                throw new FileException(Consts.SYSERR_002,ex);
            }
            finally
            {
                if (fs != null)
                {
                    fs.Close();
                    fs.Dispose();
                    fs = null;  
                }
            }
        }

        /// <summary>ファイルのコピー</summary>
        /// <param name="sourceFileName">既存のファイル</param>
        /// <param name="destFileName">新しいファイル</param>
        public static void CopyFile(string sourceFileName, string destFileName)
        {
            try
            {
                System.IO.File.Copy(sourceFileName, destFileName,true);
            }
            catch (Exception ex)
            {
                throw new FileException(Consts.SYSERR_002,ex);
            }
        }

        /// <summary>ファイルの削除</summary>
        /// <param name="strFilePath">ファイルパス</param>
        public static void DeleteFile(string strFilePath)
        {
            try
            {
                if (System.IO.File.Exists(strFilePath))
                {
                    System.IO.File.Delete(strFilePath);
                }               
            }
            catch (Exception ex)
            {
                throw new FileException(Consts.SYSERR_002,ex);
            }
        }

        /// <summary>ファイルの書き込み</summary>
        /// <param name="dt">取り込みのDataTable</param>
        /// <param name="strFilePath">ファイルパス</param>
        /// <param name="isHasHead">ヘッダがあるかどうか</param>
        public static void Export(DataTable dt, string strFilePath, bool isHasHead)
        {
            StringBuilder builder = new StringBuilder();
            StreamWriter sw = null;

            try
            {
               DeleteFile(strFilePath);

               if (dt == null || dt.Rows.Count == 0)
               {
                   return;
               }

               if (isHasHead)
               {
                   for (int i = 0; i < dt.Columns.Count; i++)
                   {
                       if(i!=0)
                       {
                          builder.Append(_seperator);                                                
                       }
                       builder.Append(dt.Columns[i].ColumnName);                    
                   }
                   builder.AppendLine();
               }

                foreach(DataRow dr in dt.Rows)
                {
                    for (int j = 0; j < dt.Columns.Count; j++)
                    {
                        if (j != 0)
                        {
                            builder.Append(_seperator);   
                        }
                        builder.Append(dr[dt.Columns[j]]);   
                    }
                    builder.AppendLine();
                }
                sw = new StreamWriter(strFilePath, false, 
                                      Encoding.GetEncoding("Shift_JIS"));
                sw.Write(builder.ToString());
            }       
            catch(Exception ex)
            {
                throw new FileException(Consts.SYSERR_002,ex);
            }
            finally
            {
                if (sw != null)
                {
                    sw.Close();
                    sw.Dispose();
                    sw = null;
                }
            }

        }

        /// <summary>ファイルの読み込み</summary>
        /// <param name="strFilePath">ファイルパス</param>
        /// <param name="isHasHead">ヘッダがあるかどうか</param>
        /// <param name="columnName">列の名前</param>
        /// <return>取り込みのDataTable</return>
        public static DataTable Import(string strFilePath, bool isHasHead, 
                                       string[] columnName)
        {
            StreamReader sr = null;
            DataTable dt = new DataTable();

            try
            {
                if (!System.IO.File.Exists(strFilePath))
                {
                    throw new FileException(Consts.SYSERR_002,null);
                }

                sr = new StreamReader(strFilePath, Encoding.GetEncoding("Shift_JIS"));

                string[] arrData = sr.ReadLine().Split(FileSeperator);

                for (int i = 0; i < columnName.Length; i++)
                {
                    dt.Columns.Add(columnName[i].ToString());
                }

                DataRow dr;

                if (!isHasHead)
                {
                    dr = dt.NewRow();
                    dr.ItemArray = arrData;
                    dt.Rows.Add(dr);
                }

                while (sr.Peek() >= 0)
                {
                    arrData = sr.ReadLine().Split(FileSeperator);

                    dr = dt.NewRow();
                    dr.ItemArray = arrData;
                    dt.Rows.Add(dr);
                }
            }
            catch (Exception ex)
            {
                throw new FileException(Consts.SYSERR_002,ex);
            }
            finally
            {
                if (sr != null)
                {
                    sr.Close();
                    sr.Dispose();
                    sr = null;
                }
            }
            return dt;
        }

        #endregion
    }
}
