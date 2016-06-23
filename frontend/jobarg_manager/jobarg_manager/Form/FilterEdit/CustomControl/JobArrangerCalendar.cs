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
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.ComponentModel;
using System.Windows.Controls.Primitives;

namespace jp.co.ftf.jobcontroller.JobController.Form.FilterEdit
{
    /// <summary>
    /// カスタムカレンダーコントロール
    /// </summary>
    [DefaultEvent("DateSelected")]
    public class JobArrangerCalendar : Calendar
    {

        #region Dependency Properties

        // The background brush used for the date highlight.
        public static DependencyProperty DateHighlightBrushProperty = DependencyProperty.Register
             (
                  "DateHighlightBrush",
                  typeof(Brush),
                  typeof(JobArrangerCalendar),
                  new PropertyMetadata(new SolidColorBrush(Colors.Red))
             );

        // The list of dates to be highlighted.
        public static DependencyProperty HighlightedDateTextProperty = DependencyProperty.Register
            (
                "HighlightedDateText",
                typeof(String[]),
                typeof(JobArrangerCalendar),
                new PropertyMetadata()
            );

        // Whether highlights should be shown.
        public static DependencyProperty ShowDateHighlightingProperty = DependencyProperty.Register
             (
                  "ShowDateHighlighting",
                  typeof(bool),
                  typeof(JobArrangerCalendar),
                  new PropertyMetadata(true)
             );

        // Whether tool tips should be shown with highlights.
        public static DependencyProperty ShowHighlightedDateTextProperty = DependencyProperty.Register
             (
                  "ShowHighlightedDateText",
                  typeof(bool),
                  typeof(JobArrangerCalendar),
                  new PropertyMetadata(true)
             );

        #endregion

        #region Constructors

        /// <summary>
        /// Static constructor.
        /// </summary>
        static JobArrangerCalendar()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(JobArrangerCalendar),
                 new FrameworkPropertyMetadata(typeof(JobArrangerCalendar)));
        }

        /// <summary>
        /// Instance constructor.
        /// </summary>
        public JobArrangerCalendar()
        {
            /* We initialize the HighlightedDateText property to an array of 31 strings,
             * since 31 is the maximum number of days in any month. */

            // Initialize HighlightedDateText property
            this.HighlightedDateText = new string[31];
        }

        #endregion

        #region CLR Properties

        /// <summary>
        /// The background brush used for the date highlight.
        /// </summary>
        [Browsable(true)]
        [Category("Highlighting")]
        public Brush DateHighlightBrush
        {
            get { return (Brush)GetValue(DateHighlightBrushProperty); }
            set { SetValue(DateHighlightBrushProperty, value); }
        }

        /// <summary>
        /// The tool tips for highlighted dates.
        /// </summary>
        [Browsable(true)]
        [Category("Highlighting")]
        public String[] HighlightedDateText
        {
            get { return (String[])GetValue(HighlightedDateTextProperty); }
            set { SetValue(HighlightedDateTextProperty, value); }
        }

        /// <summary>
        /// Whether highlights should be shown.
        /// </summary>
        [Browsable(true)]
        [Category("Highlighting")]
        public bool ShowDateHighlighting
        {
            get { return (bool)GetValue(ShowDateHighlightingProperty); }
            set { SetValue(ShowDateHighlightingProperty, value); }
        }

        /// <summary>
        /// Whether tool tips should be shown with highlights.
        /// </summary>
        [Browsable(true)]
        [Category("Highlighting")]
        public bool ShowHighlightedDateText
        {
            get { return (bool)GetValue(ShowHighlightedDateTextProperty); }
            set { SetValue(ShowHighlightedDateTextProperty, value); }
        }

        #endregion

        #region override Methods

        protected override void OnPreviewMouseLeftButtonUp(MouseButtonEventArgs e)
        {
            base.OnPreviewMouseLeftButtonUp(e);

            if (Mouse.Captured is CalendarItem) { Mouse.Capture(null); }

        }

        #endregion
    }
}
