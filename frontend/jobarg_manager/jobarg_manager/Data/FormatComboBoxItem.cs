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

namespace jp.co.ftf.jobcontroller.JobController
{
    public class FormatComboBoxItem
    {
        private string m_format = "";
        private string m_view_format = "";

        //コンストラクタ
        public FormatComboBoxItem()
        {
        }
        //コンストラクタ
        public FormatComboBoxItem(string format, string view_format)
        {
            m_format = format;
            m_view_format = view_format;
        }

        //実際の値
        public string Format
        {
            get
            {
                return m_format;
            }
            set
            {
                m_format = value;
            }
        }

        //表示名称
        //(このプロパティはこのサンプルでは使わないのでなくても良い)
        public string ViewFormat
        {
            get
            {
                return m_view_format;
            }
            set
            {
                m_view_format = value;
            }
        }

        //オーバーライドしたメソッド
        //これがコンボボックスに表示される
        public override string ToString()
        {
            return m_view_format;
        }

    }
}

