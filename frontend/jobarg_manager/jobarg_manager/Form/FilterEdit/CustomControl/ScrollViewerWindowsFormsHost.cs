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
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.Integration;
using System.Windows.Media;
using System.Windows.Forms;


namespace jp.co.ftf.jobcontroller.JobController.Form.FilterEdit
{
    public class ScrollViewerWindowsFormsHost : WindowsFormsHost
    {
        private PresentationSource _presentationSource;


        public ScrollViewerWindowsFormsHost()
        {
            PresentationSource.AddSourceChangedHandler(this, SourceChangedEventHandler);
        }

        protected override void OnWindowPositionChanged(Rect rcBoundingBox)
        {
            base.OnWindowPositionChanged(rcBoundingBox);

            if (ParentScrollViewer == null)
                return;

            GeneralTransform tr = RootVisual.TransformToDescendant(ParentScrollViewer);
            var scrollRect = new Rect(new Size(ParentScrollViewer.ViewportWidth, ParentScrollViewer.ViewportHeight));

            var intersect = Rect.Intersect(scrollRect, tr.TransformBounds(rcBoundingBox));
            if (!intersect.IsEmpty)
            {
                tr = ParentScrollViewer.TransformToDescendant(this);
                intersect = tr.TransformBounds(intersect);
            }
            else
                intersect = new Rect();

            int x1 = (int)Math.Round(intersect.Left);
            int y1 = (int)Math.Round(intersect.Top);
            int x2 = (int)Math.Round(intersect.Right);
            int y2 = (int)Math.Round(intersect.Bottom);

            SetRegion(x1, y1, x2, y2);
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            if (disposing)
                PresentationSource.RemoveSourceChangedHandler(this, SourceChangedEventHandler);
        }

        private void SourceChangedEventHandler(Object sender, SourceChangedEventArgs e)
        {
            ParentScrollViewer = FindParentScrollViewer();
        }

        private ScrollViewer FindParentScrollViewer()
        {
            DependencyObject vParent = this;
            ScrollViewer parentScroll = null;
            while (vParent != null)
            {
                parentScroll = vParent as ScrollViewer;
                if (parentScroll != null)
                    break;

                vParent = LogicalTreeHelper.GetParent(vParent);
            }
            return parentScroll;
        }

        private void SetRegion(int x1, int y1, int x2, int y2)
        {
            SetWindowRgn(Handle, CreateRectRgn(x1, y1, x2, y2), true);
        }

        Visual RootVisual
        {
            get
            {
                if (_presentationSource == null)
                    _presentationSource = PresentationSource.FromVisual(this);

                return _presentationSource.RootVisual;
            }
        }

        ScrollViewer ParentScrollViewer { get; set; }

        [DllImport("User32.dll", SetLastError = true)]
        static extern int SetWindowRgn(IntPtr hWnd, IntPtr hRgn, bool bRedraw);

        [DllImport("gdi32.dll")]
        static extern IntPtr CreateRectRgn(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect);
    }
}
