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
using System.Data;
using System.Collections.ObjectModel;
using System.Data.Odbc;
using System.Security.Cryptography;
using System.Threading;
using System.Globalization;
using jp.co.ftf.jobcontroller.Common;
using jp.co.ftf.jobcontroller.DAO;
using System.IO;
using Microsoft.Win32;


namespace jp.co.ftf.jobcontroller.JobController
{
    /// <summary>
    /// Window1.xaml の相互作用ロジック
    /// </summary>
    public partial class LoginWindow : BaseWindow
    {

        #region フィールド
        private static String DB_CONFIG_FILE = ".." +
                            System.IO.Path.DirectorySeparatorChar +
                            "conf" +
                            System.IO.Path.DirectorySeparatorChar +
                            "jobarg_manager.conf";

        private DataSet ds;
        private List<String> dsnList;
        #endregion

        #region コンストラクター
        public LoginWindow()
        {
            InitializeComponent();
            DataContext = this;
            setContents();

        }
        #endregion

        #region プロパティ
        public ObservableCollection<JobconDBSource> DBInfos { get; set; }
        public String DBConnectStr { get; set; }

        /// <summary>クラス名</summary>
        public override string ClassName
        {
            get
            {
                return "LoginWindow";
            }
        }

        /// <summary>画面ID</summary>
        public override string GamenId
        {
            get
            {
                return Consts.WINDOW_100;
            }
        }

        #endregion

        #region イベント

        //*******************************************************************
        /// <summary>ログインボタンクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        //*******************************************************************
        private void login_buttonClick(object sender, RoutedEventArgs e)
        {
            // 開始ログ
            base.WriteStartLog("login_buttonClick", Consts.PROCESS_010);

            login();

            // 終了ログ
            base.WriteEndLog("login_buttonClick", Consts.PROCESS_010);
        }

        //*******************************************************************
        /// <summary>キャンセルボタンクリック</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        //*******************************************************************
        private void cancel_buttonClick(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        //*******************************************************************
        /// <summary>キー押下</summary>
        /// <param name="sender">源</param>
        /// <param name="e">イベント</param>
        //*******************************************************************
        private void OnKeyDownHandler(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                // 開始ログ
                base.WriteStartLog("OnKeyDownHandler", Consts.PROCESS_010);

                login();

                // 終了ログ
                base.WriteEndLog("OnKeyDownHandler", Consts.PROCESS_010);
            }
        }


        #endregion

        #region private メソッド

        /// <summary>ＤＢ設定情報を読み込み画面にセットする</summary>
        private void setContents()
        {
            try
            {
                System.IO.FileStream fs = new System.IO.FileStream(@DB_CONFIG_FILE, System.IO.FileMode.Open);
                ds = new DataSet("JobconDBInfo");
                try
                {
                    ds.ReadXml(fs, XmlReadMode.InferSchema);
                }
                catch (Exception ex)
                {
                    fs.Close();
                    throw new FileException(Consts.ERROR_LOGIN_004, ex);
                }
                fs.Close();
                DBInfos = new ObservableCollection<JobconDBSource>();
                DataTable dt = new DataTable();
                if (ds.Tables.Count < 1)
                    throw new FileException(Consts.ERROR_LOGIN_004, null);
                dt = ds.Tables[0];
                if (dt.Rows.Count < 1)
                    throw new FileException(Consts.ERROR_LOGIN_004, null);
                int i = 0;
                try
                {
                    foreach (DataRow dr in dt.Rows)
                    {
                        JobconDBSource item = JobconDBSource.Create(dr);
                        if (i == 0)
                        {
                            DBConnectStr = item.ConnnectStr;
                        }
                        DBInfos.Add(item);
                        i++;
                    }
                }
                catch (Exception ex)
                {
                    throw new FileException(Consts.ERROR_LOGIN_004, ex);
                }
            }
            catch (FileNotFoundException ex)
            {
                throw new FileException(Consts.ERROR_LOGIN_003, ex);
            }

            EnumDsn();

        }

        /// <summary>ログイン処理</summary>
        private void login()
        {
            if (!dsnList.Contains<String>(((JobconDBSource)this.comboBox_jobarg.SelectedItem).DBS))
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_LOGIN_005);
                return;
            }
            String connectStr = ((JobconDBSource)this.comboBox_jobarg.SelectedItem).ConnnectStr;
            DBConnect dbConnect;
            bool authSuccess = false;

            dbConnect = new DBConnect(connectStr);
            try
            {
                dbConnect.CreateSqlConnect();
            }
            catch (Exception e)
            {
                CommonDialog.ShowErrorDialog(Consts.ERROR_LOGIN_006);
                return;
            }
            authSuccess = auth(dbConnect);
            if (authSuccess)
            {
                if (!CheckUserStatus(dbConnect))
                {
                    CommonDialog.ShowErrorDialog(Consts.ERROR_LOGIN_002);
                    dbConnect.CloseSqlConnect();
                    return;
                }
                if (!LoginSetting.Lang.StartsWith("ja_"))
                {
                    Thread.CurrentThread.CurrentUICulture = new CultureInfo("en-US", false);
                }
                JobArrangerWindow jobArrangerWindow = new JobArrangerWindow();
                jobArrangerWindow.Show();
                this.Close();
            }
            else
            {
                dbConnect.CloseSqlConnect();
                CommonDialog.ShowErrorDialog(Consts.ERROR_LOGIN_001);
            }
        }

        /// <summary>認証処理</summary>
        private bool auth(DBConnect dbConnect)
        {
            UsersDAO dao = new UsersDAO(dbConnect);
            DataTable dt = dao.GetEntityByNameAndPass(textBox_user.Text, GetMD5String(passBox_pass.Password));
            if (dt.Rows.Count == 1)
            {
                DataRow row0 = dt.Rows[0];
                LoginSetting.UserID = Convert.ToDecimal(row0["userid"]);
                LoginSetting.UserName = (String)row0["alias"];
                LoginSetting.Authority = (Consts.AuthorityEnum)row0["type"];
                LoginSetting.Mode = Consts.ActionMode.DEVELOP;
                if (LoginSetting.Authority == Consts.AuthorityEnum.GENERAL)
                {
                    LoginSetting.Mode = Consts.ActionMode.USE;
                }
                LoginSetting.ConnectStr = ((JobconDBSource)comboBox_jobarg.SelectedItem).ConnnectStr;
                LoginSetting.GroupList = DBUtil.GetGroupIDListByAlias(LoginSetting.UserName);
                LoginSetting.JobconName = ((JobconDBSource)comboBox_jobarg.SelectedItem).JobconName;

                LoginSetting.DBType = ((JobconDBSource)comboBox_jobarg.SelectedItem).DBType;

                //added by YAMA 2014/02/26
                string wkLang = (String)row0["lang"];
                LoginSetting.Lang = wkLang.ToLower();

                LoginSetting.HealthCheckFlag = ((JobconDBSource)comboBox_jobarg.SelectedItem).HealthCheckFlag;
                LoginSetting.HealthCheckInterval = ((JobconDBSource)comboBox_jobarg.SelectedItem).HealthCheckInterval;

                //added by YAMA 2014/03/03
                LoginSetting.JaZabbixVersion = ((JobconDBSource)comboBox_jobarg.SelectedItem).JaZabbixVersion;

                // added by YAMA 2014/10/20    マネージャ内部時刻同期
                LoginSetting.ManagerTimeSync = DBUtil.GetManagerTimeSync();

                // added by YAMA 2014/10/30    グループ所属無しユーザーでのマネージャ動作
                if (LoginSetting.GroupList.Count == 0)
                {
                    LoginSetting.BelongToUsrgrpFlag = false;
                }
                else
                {
                    LoginSetting.BelongToUsrgrpFlag = true;
                }

                return true;
            }
            return false;
        }

        private bool CheckUserStatus(DBConnect dbConnect)
        {
            if (LoginSetting.Authority == Consts.AuthorityEnum.SUPER)
            {
                return true;
            }
            String checkSql = "select users.userid,usrgrp.users_status from users,users_groups,usrgrp where users.alias='"
                                    + LoginSetting.UserName + "' and users.userid=users_groups.userid and users_groups.usrgrpid=usrgrp.usrgrpid and usrgrp.users_status=1";
            DataTable dt = dbConnect.ExecuteQuery(checkSql);
            if (dt.Rows.Count > 0)
                return false;
            return true;
        }

        /// <summary>パスワードの暗号化</summary>       
        private string GetMD5String(string str)
        {
            MD5CryptoServiceProvider md5 = new MD5CryptoServiceProvider();
            Byte[] bytes = md5.ComputeHash(Encoding.ASCII.GetBytes(str));
            StringBuilder sb = new StringBuilder();
            foreach (byte b in bytes)
            {
                sb.Append(b.ToString("x2"));
            }
            return sb.ToString();
        }

        /// <summary>ODBCのDSN一覧を取得</summary>   
        private void EnumDsn()
        {
            dsnList = new List<string>();
            dsnList.AddRange(EnumDsn(Registry.CurrentUser));
            dsnList.AddRange(EnumDsn(Registry.LocalMachine));

        }
        /// <summary>特定レジストリーキーのODBCのDSN一覧を取得</summary> 
        private IEnumerable<string> EnumDsn(RegistryKey rootKey)
        {
            RegistryKey regKey = rootKey.OpenSubKey(@"Software\ODBC\ODBC.INI\ODBC Data Sources");
            if (regKey != null)
            {
                foreach (string name in regKey.GetValueNames())
                {
                    string value = regKey.GetValue(name, "").ToString();
                    yield return name;
                }
            }
        }

        #endregion
    }
}