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
using System.Windows.Data;
using System.Windows;
using jp.co.ftf.jobcontroller.Common;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author KIM  2013/07/23 新規作成<BR>                           *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController
{
    /// <summary>
    /// ジョブ実行結果画面詳細ボタン表示用コンパータ
    /// </summary>
    class DetailVisibilityConverter : IMultiValueConverter

    {
        public object Convert(object[] values,
                            Type targetType,
                            object parameter,
                            System.Globalization.CultureInfo culture)

        {

            if ((values[0] != null && !values[0].Equals(DependencyProperty.UnsetValue) && !values[0].ToString().Trim().Equals("")) 
                || (values[1] != null && !values[1].Equals(DependencyProperty.UnsetValue) && !values[1].ToString().Trim().Equals("")))
			{
                return System.Windows.Visibility.Visible;
			}
			else
			{
                return System.Windows.Visibility.Hidden;
			}
        }


        public object[] ConvertBack(object value,
                                    Type[] targetTypes,
                                    object parameter,
                                    System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

    }
}
