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
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Data;
using System.IO;
using jp.co.ftf.jobcontroller.Common;
using jp.co.ftf.jobcontroller.DAO;

namespace jp.co.ftf.jobcontroller.JobController
{
    /// <summary>
    /// ExportWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class ExportWindow : BaseWindow
    {
        #region フィールド

        /// <summary>オブジェクトＩＤ</summary>
        private String _objectId;

        /// <summary>オブジェクト種別</summary>
        private Consts.ObjectEnum _objectType;

        /// <summary>エクスポートデータ</summary>
        private DataRow[] _rows;
        #endregion

        #region コンストラクタ
        public ExportWindow(String objectId, Consts.ObjectEnum objectType, DataRow[] rows)
        {
            InitializeComponent();
            _objectId = objectId;
            _objectType = objectType;
            _rows = rows;
            DataContext = this;
        }
        #endregion

        #region プロパティ

        /// <summary>クラス名</summary>
        public override string ClassName
        {
            get
            {
                return "ExportWindow";
            }
        }

        /// <summary>画面ID</summary>
        public override string GamenId
        {
            get
            {
                return Consts.WINDOW_240;
            }
        }
        #endregion

        #region イベント
        //*******************************************************************
        /// <summary>参照ボタンクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">マウスイベント</param>
        //*******************************************************************
        private void refFile_Click(object sender, EventArgs e)
        {
            Microsoft.Win32.SaveFileDialog saveFileDialog1 = new Microsoft.Win32.SaveFileDialog();

            // 初期表示するディレクトリを設定する
            if (Consts.EXPORT_PATH == null || Consts.EXPORT_PATH.Equals(""))
            {
                saveFileDialog1.InitialDirectory = @"C:\";
            }
            else
            {
                saveFileDialog1.InitialDirectory = Consts.EXPORT_PATH;
            }

            // ファイルのフィルタを設定する
            saveFileDialog1.Filter = "XML File(*.xml)|*.xml";

            // ファイルの種類 の初期設定を 2 番目に設定する (初期値 1)
            saveFileDialog1.FilterIndex = 2;

            // ダイアログボックスを閉じる前に現在のディレクトリを復元する (初期値 false)
            saveFileDialog1.RestoreDirectory = true;

            // 存在しないファイルを指定した場合は、
            // 新しく作成するかどうかの問い合わせを表示する (初期値 false)
            /* added by YAMA 2014/09/22 フォルダ選択ダイアログの操作性向上 */
            //saveFileDialog1.CreatePrompt = true;
            saveFileDialog1.CreatePrompt = false;
            saveFileDialog1.OverwritePrompt = true;

            //ダイアログを表示する
            if (saveFileDialog1.ShowDialog() == true )
            {
                textBox_fileName.Text = saveFileDialog1.FileName;
            }
        }

        //*******************************************************************
        /// <summary>キャンセルボタンクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">マウスイベント</param>
        //*******************************************************************
        private void cancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        //*******************************************************************
        /// <summary>ＯＫボタンクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">マウスイベント</param>
        //*******************************************************************
        private void ok_Click(object sender, EventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("ok_Click", Consts.PROCESS_012);

            // 入力チェック 
            if (!InputCheck())
                return;
            try
            {
                System.IO.StreamWriter file =
                    new System.IO.StreamWriter(textBox_fileName.Text, false);
                DataSet ds = DBUtil.Export(_objectId, _objectType, _rows);
                ds.WriteXml(file);
                file.Close();
                this.Close();

                System.IO.DirectoryInfo dirInfoBar = new System.IO.DirectoryInfo(textBox_fileName.Text);
                System.IO.DirectoryInfo dirInfo = dirInfoBar.Parent;

                Consts.EXPORT_PATH = dirInfo.FullName;
            }
            catch (ArgumentException ex)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_019);
            }
            catch (NotSupportedException ex)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_019);
            }
            catch (DirectoryNotFoundException ex)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_022);
            }
            catch (UnauthorizedAccessException ex)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_023);
            }
            catch (System.IO.IOException ex)
            {
                CommonDialog.ShowErrorDialogFromMessage(ex.Message);
            }
            catch (Exception ex)
            {
                CommonDialog.ShowErrorDialogFromMessage(ex.Message);
            }
            // 終了ログ
            base.WriteEndLog("ok_Click", Consts.PROCESS_012);
        }
        #endregion

        #region privateメソッド
        //*******************************************************************
        /// <summary>入力チェック </summary>
        /// <returns>チェック結果</returns>
        //*******************************************************************
        private bool InputCheck()
        {
            // ジョブネット名が未入力の場合 
            if (CheckUtil.IsNullOrEmpty(textBox_fileName.Text))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_COMMON_014);
                return false;
            }
            return true;
        }
        #endregion
    }
}
