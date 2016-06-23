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

namespace jp.co.ftf.jobcontroller.DAO
{
    public class ImportDoubleRelationCheck
    {
        private string _tableName;
        private string[] _keys;
        private string _refTableName;
        private string[] _refKeys;

        public string TableName
        {
            get { return _tableName; }
            set { _tableName = value; }
        }
        public string[] Keys
        {
            get { return _keys; }
            set { _keys = value; }
        }
        public string RefTableName
        {
            get { return _refTableName; }
            set { _refTableName = value; }
        }
        public string[] RefKeys
        {
            get { return _refKeys; }
            set { _refKeys = value; }
        }

        public ImportDoubleRelationCheck(String tableName, String[] keys, String refTableName, String[] refKeys)
        {
            _tableName = tableName;
            _keys = keys;
            _refTableName = refTableName;
            _refKeys = refKeys;
        }
    }
}

