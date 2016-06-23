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
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Data;
using jp.co.ftf.jobcontroller.Common;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2013 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author KIM  2013/09/16 新規作成<BR>                          *
//                                                                  *
//                                                                  *
//*******************************************************************

namespace jp.co.ftf.jobcontroller.JobController
{
    /// <summary>
    /// Attached property provider which adds the read-only attached property
    /// <c>TextBlockService.IsTextTrimmed</c> to the framework's <see cref="TextBlock"/> control.
    /// </summary>
    public class TextBlockService
    {

        #region Attached Property [TextBlockService.IsTextTrimmed]

        /// <summary>
        /// Key returned upon registering the read-only attached property <c>IsTextTrimmed</c>.
        /// </summary>
        public static readonly DependencyPropertyKey IsTextTrimmedKey = DependencyProperty.RegisterAttachedReadOnly(
            "IsTextTrimmed",
            typeof( bool ),
            typeof( TextBlockService ),
            new PropertyMetadata( false ) );    // defaults to false

        /// <summary>
        /// Identifier associated with the read-only attached property <c>IsTextTrimmed</c>.
        /// </summary>
        public static readonly DependencyProperty IsTextTrimmedProperty = IsTextTrimmedKey.DependencyProperty;

        /// <summary>
        /// Returns the current effective value of the IsTextTrimmed attached property.
        /// </summary>
        /// <remarks>Invoked automatically by the framework when databound.</remarks>
        /// <param name="target"><see cref="TextBlock"/> to evaluate</param>
        /// <returns>Effective value of the IsTextTrimmed attached property</returns>
        [AttachedPropertyBrowsableForType( typeof( TextBlock ) )]
        public static Boolean GetIsTextTrimmed( TextBlock target )
        {
            return (Boolean) target.GetValue( IsTextTrimmedProperty );
        }

        #endregion (Attached Property [TextBlockService.IsTextTrimmed])


        /// <summary>
        /// Sets the instance value of read-only dependency property <see cref="IsTextTrimmed"/>.
        /// </summary>
        /// <param name="target">Associated <see cref="TextBlock"/> instance</param>
        /// <param name="value">New value for IsTextTrimmed</param>
        public static void SetIsTextTrimmed( TextBlock target, Boolean value )
        {
            target.SetValue( IsTextTrimmedKey, value );
        }
    }
}
