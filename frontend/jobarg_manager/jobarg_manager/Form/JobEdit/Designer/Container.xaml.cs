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
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Data;
using System.Collections;
using System.Windows.Shapes;
using jp.co.ftf.jobcontroller.Common;
using jp.co.ftf.jobcontroller.DAO;
using jp.co.ftf.jobcontroller.JobController.Form.JobManager;
//*******************************************************************
//                                                                  *
//                                                                  *
//  Copyright (C) 2012 FitechForce, Inc. All Rights Reserved.       *
//                                                                  *
//  * @author DHC 劉 偉 2012/10/04 新規作成<BR>                      *
//                                                                  *
//                                                                  *
//*******************************************************************
namespace jp.co.ftf.jobcontroller.JobController.Form.JobEdit
{
/// <summary>
/// Container.xaml の相互作用ロジック
/// </summary>
public partial class Container : UserControl,IContainer
{
    #region コンストラクタ
    public Container()
    {
        // 初期化 
        InitializeComponent();
    }
    #endregion

    #region フィールド

    /// <summary>マウスの位置</summary>
    private Point mousePosition;

    /// <summary>マウス追従フラグ</summary>
    private bool trackingMouseMove = false;

    /// <summary>選択範囲</summary>
    Rectangle temproaryEllipse;

    /// <summary>DBアクセスインスタンス</summary>
    private DBConnect dbAccess = new DBConnect(LoginSetting.ConnectStr);

    /// <summary>inner_jobnet_id index取得SQL</summary>
    private String GET_INNER_JOBNET_ID_SQL = "select nextid from ja_index_table where count_id=2 for update";

    /// <summary>inner_job_id index取得SQL</summary>
    private String GET_INNER_JOB_ID_SQL_START = "select nextid from ja_index_table where count_id=20 for update";

    /// <summary>inner_flow_id index取得SQL</summary>
    private String GET_INNER_FLOW_ID_SQL1 = "select nextid from ja_index_table where count_id=30 for update";

    /// <summary>画布</summary>
    public Canvas ContainerCanvas
    {
        get
        {
            return cnsDesignerContainer;
        }
    }

    /// <summary>ウィンドウ</summary>
    ContentControl _parantWindow;
    public ContentControl ParantWindow
    {
        get
        {
            return _parantWindow; 
        }
        set
        {
            _parantWindow = value;
        }
    }

    /// <summary>画布内のアイテム</summary>
    private Hashtable _jobItems;
    public Hashtable JobItems
    {
        get
        {
            if (this._jobItems == null)
            {
                this._jobItems = new Hashtable();
            }
            return this._jobItems;
        }
    }

    /// <summary>設定済ジョブＩＤ</summary>
    private Hashtable _setedjobIds;
    public Hashtable SetedJobIds
    {
        get
        {
            if (this._setedjobIds == null)
            {
                this._setedjobIds = new Hashtable();
            }
            return this._setedjobIds;
        }
        set
        {
            _setedjobIds = value;
        }
    }

    /// <summary>画布の空白の場所にクリックフラグ</summary>
    bool _canvasClickFlg = true;
    public bool CanvasClickFlg
    {
        get
        {
            return _canvasClickFlg;
        }
        set
        {
            _canvasClickFlg = value;
        }
    }

    /// <summary>マウスがコンテナ内かどうかのフラグ</summary>
    bool mouseIsInContainer = false;
    public bool MouseIsInContainer { 
        get 
        {
            return mouseIsInContainer; 
        } 
        set 
        { 
            mouseIsInContainer = value;
        } 
    }

    /// <summary> 選択コントローラリスト</summary>
    List<System.Windows.Controls.Control> _currentSelectedControlCollection;
    public List<Control> CurrentSelectedControlCollection
    {
        get
        {
            if (_currentSelectedControlCollection == null)
                _currentSelectedControlCollection = new List<Control>();
            return _currentSelectedControlCollection;
        }
    }

    /// <summary>選択した部品リストの最左位置のX座標</summary>
    private double _leftXOfSelection = -1;
    public double LeftXOfSelection
    {
        get
        {
            return _leftXOfSelection;
        }
        set
        {
            _leftXOfSelection = value;
        }
    }

    /// <summary>選択した部品リストの最上位置のY座標</summary>
    private double _topYOfSelection = -1;
    public double TopYOfSelection
    {
        get
        {
            return _topYOfSelection;
        }
        set
        {
            _topYOfSelection = value;
        }
    }

    /// <summary>マウス選択かどうか。</summary>
    public bool IsMouseSelecting
    {
        get
        {
            return (temproaryEllipse != null);
        }
    }

    /// <summary>ジョブネットID</summary>
    private string _jobnetId;
    public string JobnetId
    {
        get
        {
            return _jobnetId;
        }
        set
        {
            _jobnetId = value;
        }
    }

    /// <summary>更新日</summary>
    private string _updDate;
    public string UpdDate
    {
        get
        {
            return _updDate;
        }
        set
        {
            _updDate = value;
        }
    }

    /// <summary>仮更新日</summary>
    private string _tmpUpdDate;
    public string TmpUpdDate
    {
        get
        {
            return _tmpUpdDate;
        }
        set
        {
            _tmpUpdDate = value;
        }
    }
    /// <summary>ドラッグのジョブネットID</summary>
    private string _dragJobnetId;
    public string DragJobnetId
    {
        get
        {
            return _dragJobnetId;
        }
        set
        {
            _dragJobnetId = value;
        }
    }

    /// <summary>シフトキー押下状態フラグ</summary>
    public bool ShiftKeyIsPress
    {
        get
        {
            return (Keyboard.Modifiers == ModifierKeys.Shift);
        }
    }

    /// <summary>シフトキーで複数選択されたか判定フラグ</summary>
    private bool _isSelectedByShiftKey;
    public bool IsSelectedByShiftKey
    {
        get
        {
            return _isSelectedByShiftKey;
        }
        set
        {
            _isSelectedByShiftKey = value;
        }
    }

    /// <summary>zoom value</summary>
    public double ZoomValue
    {
        get
        {
            return (double)zoomSlider.Value;
        }
    }

    #region データ格納場所

    /// <summary>ジョブネットアイコン設定テーブル</summary>
    public DataTable JobnetControlTable { get; set; }

    /// <summary>ジョブ管理テーブル</summary>
    public DataTable JobControlTable { get; set; }

    /// <summary>フロー管理テーブル</summary>
    public DataTable FlowControlTable { get; set; }

    /// <summary>計算アイコン設定テーブル</summary>
    public DataTable IconCalcTable { get; set; }

    /// <summary>終了アイコン設定テーブル</summary>
    public DataTable IconEndTable { get; set; }

    /// <summary>拡張ジョブアイコン設定テーブル</summary>
    public DataTable IconExtjobTable { get; set; }

    // <summary>条件分岐アイコン設定テーブル</summary>
    public DataTable IconIfTable { get; set; }

    // <summary>情報取得アイコン設定テーブル</summary>
    public DataTable IconInfoTable { get; set; }

    // <summary>ジョブネットアイコン設定テーブル</summary>
    public DataTable IconJobnetTable { get; set; }

    // <summary>ジョブアイコン設定テーブル</summary>
    public DataTable IconJobTable { get; set; }

    // <summary>ジョブコマンド設定テーブル</summary>
    public DataTable JobCommandTable { get; set; }

    // <summary>ジョブ変数設定テーブル</summary>
    public DataTable ValueJobTable { get; set; }

    // <summary>ジョブコントローラ変数設定テーブル</summary>
    public DataTable ValueJobConTable { get; set; }

    // <summary>タスクアイコン設定テーブル</summary>
    public DataTable IconTaskTable { get; set; }

    // <summary>ジョブコントローラ変数アイコン設定テーブル</summary>
    public DataTable IconValueTable { get; set; }

    // <summary>ジョブコントローラ変数定義テーブル</summary>
    public DataTable DefineValueJobconTable { get; set; }

    // <summary>拡張ジョブ定義テーブル</summary>
    public DataTable DefineExtJobTable { get; set; }

    // <summary>ファイル転送アイコン設定テーブル</summary>
    public DataTable IconFcopyTable { get; set; }

    // <summary>ファイル待ち合わせアイコン設定テーブル</summary>
    public DataTable IconFwaitTable { get; set; }

    // <summary>リブートアイコン設定テーブル</summary>
    public DataTable IconRebootTable { get; set; }

    #endregion

    public double getLeftX()
    {
        if (CurrentSelectedControlCollection.Count < 1)
            return -1;

        double leftX = -1;

        // 始め 
        //_leftXOfSelection = ((CommonItem)CurrentSelectedControlCollection[0]).CenterPoint.X;
        for (int i = 0; i < CurrentSelectedControlCollection.Count; i++)
        {
            if (CurrentSelectedControlCollection[i] is CommonItem)
            {
                if (leftX == -1
                    || leftX > ((CommonItem)CurrentSelectedControlCollection[i]).CenterPoint.X)
                    leftX = ((CommonItem)CurrentSelectedControlCollection[i]).CenterPoint.X;
            }
        }

        return leftX;
    }

    public double getTopY()
    {
        if (CurrentSelectedControlCollection.Count < 1)
            return -1;

        double topY = -1;

        // 始め 
        //topY = ((CommonItem)CurrentSelectedControlCollection[0]).CenterPoint.Y;
        for (int i = 0; i < CurrentSelectedControlCollection.Count; i++)
        {
            if (CurrentSelectedControlCollection[i] is CommonItem)
            {
                if (topY == -1
                    || topY > ((CommonItem)CurrentSelectedControlCollection[i]).CenterPoint.Y)
                    topY = ((CommonItem)CurrentSelectedControlCollection[i]).CenterPoint.Y;
            }
        }

        return topY;
    }

    #endregion

    #region イベント

    //*******************************************************************
    /// <summary>コンテナをを左クリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void Container_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        if (e.ClickCount == 1)
        {
            // 選択をクリア 
            if (_canvasClickFlg) 
                ClearSelectFlowElement(null);

            FrameworkElement element = sender as FrameworkElement;
            mousePosition = e.GetPosition(element);
            // 一旦削除（区域選択用）

            trackingMouseMove = true;
                
        }
    }

    //*******************************************************************
    /// <summary>コンテナを右クリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void UserControl_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
    {
        this.ContextMenu.Visibility = Visibility.Visible;
    }

    /// <summary>
    /// コンテナ内のマウス移動処理
    /// </summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    public void Container_MouseMove(object sender, MouseEventArgs e)
    {
        // 追従の場合

        if (trackingMouseMove)
        {
            FrameworkElement element = sender as FrameworkElement;
            Point beginPoint = mousePosition;
            Point endPoint = e.GetPosition(element);

            if (temproaryEllipse == null)
            {
                temproaryEllipse = new Rectangle();

                SolidColorBrush brush = new SolidColorBrush();
                brush.Color = Color.FromArgb(255, 234, 213, 2);
                temproaryEllipse.Fill = brush;
                temproaryEllipse.Opacity = 0.2;

                brush = new SolidColorBrush();
                brush.Color = Color.FromArgb(255, 0, 0, 0);
                temproaryEllipse.Stroke = brush;
                temproaryEllipse.StrokeMiterLimit = 2.0;

                cnsDesignerContainer.Children.Add(temproaryEllipse);

            }

            if (endPoint.X >= beginPoint.X)
            {
                if (endPoint.Y >= beginPoint.Y)
                {
                    temproaryEllipse.SetValue(Canvas.TopProperty, beginPoint.Y);
                    temproaryEllipse.SetValue(Canvas.LeftProperty, beginPoint.X);
                }
                else
                {
                    temproaryEllipse.SetValue(Canvas.TopProperty, endPoint.Y);
                    temproaryEllipse.SetValue(Canvas.LeftProperty, beginPoint.X);
                }

            }
            else
            {
                if (endPoint.Y >= beginPoint.Y)
                {
                    temproaryEllipse.SetValue(Canvas.TopProperty, beginPoint.Y);
                    temproaryEllipse.SetValue(Canvas.LeftProperty, endPoint.X);
                }
                else
                {
                    temproaryEllipse.SetValue(Canvas.TopProperty, endPoint.Y);
                    temproaryEllipse.SetValue(Canvas.LeftProperty, endPoint.X);
                }

            }

            temproaryEllipse.Width = Math.Abs(endPoint.X - beginPoint.X);
            temproaryEllipse.Height = Math.Abs(endPoint.Y - beginPoint.Y);
            //スクロール際の場合、スクロールする
            ScrollIfNeeded(e.GetPosition(svContainer));

        }
    }
    protected override void OnPreviewKeyDown(KeyEventArgs e)
    {
        double deltaX = 0;
        double deltaY = 0;
        if (e.Key == Key.Left)
        {
            deltaX = -1;
        }
        if (e.Key == Key.Right)
        {
            deltaX = 1;
        }
        if (e.Key == Key.Up)
        {
            deltaY = -1;
        }
        if (e.Key == Key.Down)
        {
            deltaY = 1;
        }

        if ((deltaX > 0 || deltaX < 0 || deltaY > 0 || deltaY < 0) && IsCanArrowMove(deltaX, deltaY))
        {
            MoveControlCollectionByDisplacement(deltaX, deltaY, null);
            SetControlCollectionItemAndRuleNameControlPosition(null);
            e.Handled = true;
            return;
        }
        if (e.Key == Key.Delete)
        {
            DeleteSelectedControl();
            DeleteSelectedFlow();
            e.Handled = true;
            return;
        }
    }


    //*******************************************************************
    /// <summary>左マウスの解放処理</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void Container_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        trackingMouseMove = false;

        FrameworkElement element = sender as FrameworkElement;
        mousePosition = e.GetPosition(element);

        if (temproaryEllipse != null)
        {
            double width = temproaryEllipse.Width;
            double height = temproaryEllipse.Height;

            if (width > 10 && height > 10)
            {
                Point p = new Point();
                p.X = (double)temproaryEllipse.GetValue(Canvas.LeftProperty);
                p.Y = (double)temproaryEllipse.GetValue(Canvas.TopProperty);

                //Activity a = null;
                //Rule r = null;
                Label l = null;
                foreach (UIElement uie in cnsDesignerContainer.Children)
                {
                    if (uie is Flow || uie is FlowArc || uie is Rectangle) continue;
                    CommonItem item = (CommonItem)uie;

                    if (p.X < item.CenterPoint.X && item.CenterPoint.X < p.X + width
             && p.Y < item.CenterPoint.Y && item.CenterPoint.Y < p.Y + height)
                    {
                        AddSelectedControl(item);
                        item.IsSelectd = true;
                    }
                }
            }
            cnsDesignerContainer.Children.Remove(temproaryEllipse);

            _leftXOfSelection = getLeftX();
            _topYOfSelection = getTopY();

            temproaryEllipse = null;
        }

    }

    //*******************************************************************
    /// <summary>コンテナに入れる</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void Container_MouseEnter(object sender, MouseEventArgs e)
    {
        mouseIsInContainer = true;
    }

    //*******************************************************************
    /// <summary>コンテナを離れる</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void Container_MouseLeave(object sender, MouseEventArgs e)
    {
        mouseIsInContainer = false;
    }

    #region 部品のドラッグ処理 

    //*******************************************************************
    /// <summary>ジョブをドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void JobSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.JOB;
        DragDrop.DoDragDrop((JobSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>条件をドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void IfSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.IF;
        DragDrop.DoDragDrop((IfSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>並行の開始をドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void ParStartSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.MTS;
        DragDrop.DoDragDrop((MtsSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>並行の終了をドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void ParEndSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.MTE;
        DragDrop.DoDragDrop((MteSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>変数をドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void VarSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.ENV;
        DragDrop.DoDragDrop((EnvSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>拡張ジョブをドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void JobExSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.EXTJOB;
        DragDrop.DoDragDrop((ExtJobSample)sender, data, DragDropEffects.Copy);
    }


    //*******************************************************************
    /// <summary>Startドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void StartSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.START;
        DragDrop.DoDragDrop((StartSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>ENDドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void EndSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.END;
        DragDrop.DoDragDrop((EndSample)sender, data, DragDropEffects.Copy);

    }

    //*******************************************************************
    /// <summary>計算をドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void CalcSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.CAL;
        DragDrop.DoDragDrop((CalSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>Loopをドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void LoopSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.LOOP;
        DragDrop.DoDragDrop((LoopSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>タスクをドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void TaskSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.TASK;
        DragDrop.DoDragDrop((TaskSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>情報をドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void InfoSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.INF;
        DragDrop.DoDragDrop((InfSample)sender, data, DragDropEffects.Copy);
    }

    //added by kim 2012/11/14
    //*******************************************************************
    /// <summary>分岐の終了をドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void IfeSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.IFE;
        DragDrop.DoDragDrop((IfeSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>ファイル転送をドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void FCopySample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.FCOPY;
        DragDrop.DoDragDrop((FCopySample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>ファイル待ち合わせをドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void FWaitSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.FWAIT;
        DragDrop.DoDragDrop((FWaitSample)sender, data, DragDropEffects.Copy);
    }

    //*******************************************************************
    /// <summary>リブートをドラッグ</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    public void RebootSample_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        JobData data = new JobData();
        data.JobType = ElementType.REBOOT;
        DragDrop.DoDragDrop((RebootSample)sender, data, DragDropEffects.Copy);
    }

    //************************************************************************
    /// <summary>ドラッグ部品を受け入れる</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //************************************************************************
    public void Target_Drop(object sender, DragEventArgs e)
    {
        JobData data = (JobData)e.Data.GetData
            ("jp.co.ftf.jobcontroller.JobController.Form.JobEdit.JobData");
        //アイコンの存在チェック 
        if (CheckJobExist(data.JobType))
        {
            CommonDialog.ShowErrorDialog(Consts.ERROR_JOBEDIT_004);
            return;
        }

        CommonItem item = new CommonItem((IContainer)this, data, Consts.EditType.Add);

        ((Canvas)sender).Children.Add(item);
        this.JobItems.Add(item.JobId,item);

        Point p = e.GetPosition(item);

        double x = p.X - item.PicWidth / 2;
        double y = p.Y - item.PicHeight / 2;

        double maxWidth = ContainerCanvas.Width - item.PicWidth;
        double maxHeight = ContainerCanvas.Height - item.PicHeight;

        // コンテナの範囲を越える
        if (x < 0)
            x = 0;
        else if (x > maxWidth)
            x = maxWidth;

        if (y < 0)
            y = 0;
        else if (y > maxHeight)
            y = maxHeight;

        item.SetValue(Canvas.TopProperty, y);
        item.SetValue(Canvas.LeftProperty, x);

        SetItemPosition(item.JobId, x, y);
    }

    #endregion

    #region メニューコンテクストのイベント


    //*******************************************************************
    /// <summary>コンテクストメニューを表示</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void ContextMenu_Open(object sender, RoutedEventArgs e)
    {
        SetContextStatus();
    }

    //*******************************************************************
    /// <summary>直線フローをクリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void MenuitemLine_Click(object sender, RoutedEventArgs e)
    {
        List<System.Windows.Controls.Control> selectItems = CurrentSelectedControlCollection;
        MakeFlow(FlowLineType.Line, (IRoom)selectItems[0], (IRoom)selectItems[1], 0, Consts.EditType.Add);
    }

    //*******************************************************************
    /// <summary>曲線フローをクリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void MenuitemCurve_Click(object sender, RoutedEventArgs e)
    {
        List<System.Windows.Controls.Control> selectItems = CurrentSelectedControlCollection;
        MakeFlow(FlowLineType.Curve, (IRoom)selectItems[0], (IRoom)selectItems[1],0, Consts.EditType.Add);

    }

    //*******************************************************************
    /// <summary>TRUE設定をクリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void MenuitemSetTrue_Click(object sender, RoutedEventArgs e)
    {
        if (CurrentSelectedControlCollection == null
            || CurrentSelectedControlCollection.Count != 1
            || !(CurrentSelectedControlCollection[0] is IFlow))
            return;

        IFlow flow = (IFlow)CurrentSelectedControlCollection[0];
        flow.SetTrue(Consts.EditType.Add);
    }

    //*******************************************************************
    /// <summary>FALSE設定をクリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void MenuitemSetFalse_Click(object sender, RoutedEventArgs e)
    {
        if (CurrentSelectedControlCollection == null
            || CurrentSelectedControlCollection.Count != 1
            || !(CurrentSelectedControlCollection[0] is IFlow))
            return;

        IFlow flow = (IFlow)CurrentSelectedControlCollection[0];
        flow.SetFalse(Consts.EditType.Add);
    }

    //*******************************************************************
    /// <summary>フロー削除をクリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void MenuitemDelFlow_Click(object sender, RoutedEventArgs e)
    {
        DeleteSelectedFlow();
    }

    //*******************************************************************
    /// <summary>削除をクリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void MenuitemlineDel_Click(object sender, RoutedEventArgs e)
    {
        DeleteSelectedControl();
    }

    //*******************************************************************
    /// <summary>設定をクリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void MenuitemlineSet_Click(object sender, RoutedEventArgs e)
    {
        if (_currentSelectedControlCollection.Count == 1)
        {
            ((IRoom)_currentSelectedControlCollection[0]).ShowIconSetting();
        }
    }

    //*******************************************************************
    /// <summary>ジョブ起動をクリック</summary>
    /// <param name="sender">源</param>
    /// <param name="e">マウスイベント</param>
    //*******************************************************************
    private void MenuitemJobRun_Click(object sender, RoutedEventArgs e)
    {
        if (_currentSelectedControlCollection.Count == 1 &&
            ((CommonItem)_currentSelectedControlCollection[0]).ElementType == ElementType.JOB &&
            (SetedJobIds.Contains(((CommonItem)_currentSelectedControlCollection[0]).JobId) || 
            ((CommonItem)_currentSelectedControlCollection[0]).ItemEditType != Consts.EditType.Add)
            )
        {
            JobRun(((CommonItem)_currentSelectedControlCollection[0]).JobId);
        }
    }

    //*******************************************************************
    /// <summary>設定をクリック</summary>
    /// <param name="lineType">線のタイプ</param>
    /// <param name="item1">開始ジョブ</param>
    /// <param name="item2">終了ジョブ</param>
    /// <param name="flowType">フローのタイプ(True、False)</param>
    /// <param name="editType">編集タイプ</param>
    //*******************************************************************
    public void MakeFlow(FlowLineType lineType, IRoom item1, IRoom item2,　int flowType, Consts.EditType editType)
    {
        Point startPos;
        Point endPos;
        IFlow flow = null;
        double flowWith = 0;

        // 直線の場合

        if (FlowLineType.Line.Equals(lineType))
        {
            flow = new Flow();

            List<Point> connectPoints = CommonUtil.GetConnectPoints(item1, item2);

            if (connectPoints == null || connectPoints.Count != 2)
            {
                return;
            }

            startPos = connectPoints[0];
            endPos = connectPoints[1];

            flow.BeginPosition = connectPoints[0];
            flow.EndPosition = connectPoints[1];
        }
        // 曲線の場合

        else if (FlowLineType.Curve.Equals(lineType))
        {
            flow = new FlowArc();

            List<Point> connectPoints = CommonUtil.GetConnectPointsForCurve(item1, item2);

            if (connectPoints == null || connectPoints.Count != 2)
            {
                return;
            }

            flow.BeginPosition = connectPoints[0];
            flow.EndPosition = connectPoints[1];

            // フロー幅 = 半径 
            flowWith = Point.Subtract(connectPoints[0], connectPoints[1]).Length;
            ((FlowArc)flow).arrow.Radius = flowWith * 0.65;
            ((FlowArc)flow).arrow.LineSweepDirection = SweepDirection.Clockwise;
            ((FlowArc)flow).arrow.InitCenterPosition();
        }
        else
            return;

        AddFlow(flow, item1, item2, flowWith, editType);

        item1.AddBeginFlow(flow);
        item2.AddEndFlow(flow);

        flow.BeginItem = item1;
        flow.EndItem = item2;

        // フロータイプをセット 
        if (FlowType.TRUE == (FlowType)flowType)
            flow.SetTrue(editType);
        if (FlowType.FALSE == (FlowType)flowType)
            flow.SetFalse(editType);

        }
    #endregion

    #endregion

    #region publicメッソド 

    //*******************************************************************
    /// <summary>選択モジュールを追加</summary>
    /// <param name="uc">部品</param>
    //*******************************************************************
    public void AddSelectedControl(System.Windows.Controls.Control uc)
    {
        if (!CurrentSelectedControlCollection.Contains(uc))
        {
            _currentSelectedControlCollection.Add(uc);
        }
                
    }

    //*******************************************************************
    /// <summary>フローを追加</summary>
    /// <param name="flow">フロー</param>
    /// <param name="item1">開始ジョブ/param>
    /// <param name="item2">終了ジョブ/param>
    /// <param name="flowWith">曲線幅(半径)/param>
    //*******************************************************************
    public void AddFlow(IFlow flow, IRoom item1, IRoom item2, double flowWith, Consts.EditType editType)
    {
        if (editType == Consts.EditType.READ || Consts.ActionMode.USE == LoginSetting.Mode)
        {
            ((UserControl)flow).IsEnabled = false;
        }
        cnsDesignerContainer.Children.Add((UIElement)flow);
        flow.Container = this;

        // データを追加 
        if (Consts.EditType.Add == editType)
        {
            DataRow row = FlowControlTable.NewRow();
            //ジョブネットID 
            row["jobnet_id"] = _jobnetId;
            // 開始ジョブID 
            row["start_job_id"] = item1.JobId;
            // 終了ジョブID 
            row["end_job_id"] = item2.JobId;
            // 更新日 
            row["update_date"] = _tmpUpdDate;
            // フロータイプ(0：通常（初期値）) 
            row["flow_type"] = 0;
            // フロー幅 

            // 左曲線の場合 
            if (flow.BeginPosition.Y > flow.EndPosition.Y)
                flowWith = 0 - flowWith;

            row["flow_width"] = Convert.ToInt16(flowWith);

            FlowControlTable.Rows.Add(row);
        }
    }

    //*******************************************************************
    /// <summary>特定の部品を選択</summary>
    /// <param name="uc">部品</param>
    /// <param name="isSelected">選択フラグ</param>
    //*******************************************************************
    public void SetWorkFlowElementSelected(System.Windows.Controls.Control uc, bool isSelected)
    {
        if (isSelected)
        {
            AddSelectedControl(uc);
            //_leftXOfSelection = getLeftX();
            //_topYOfSelection = getTopY();
        }   
        else
            RemoveSelectedControl(uc);
        if (!ShiftKeyIsPress)
            ClearSelectFlowElement(uc);

    }

    //*******************************************************************
    /// <summary>特定の部品選択状態をはずす</summary>
    /// <param name="uc">部品</param>
    //*******************************************************************
    public void RemoveSelectedControl(System.Windows.Controls.Control uc)
    {
        if (CurrentSelectedControlCollection.Contains(uc))
        {
            CurrentSelectedControlCollection.Remove(uc);
        }
    }

    //*******************************************************************
    /// <summary>部品選択状態をはずす</summary>
    /// <param name="uc">部品（nullの場合、全部をはずす）</param>
    //*******************************************************************
    public void ClearSelectFlowElement(System.Windows.Controls.Control uc)
    {
        if (CurrentSelectedControlCollection == null || CurrentSelectedControlCollection.Count == 0)
            return;

        int count = CurrentSelectedControlCollection.Count;
        for (int i = 0; i < count; i++)
        {
            if (!(CurrentSelectedControlCollection[i] is IFlow))
                ((IRoom)CurrentSelectedControlCollection[i]).IsSelectd = false;
            else if (CurrentSelectedControlCollection[i] is IFlow)
                ((IFlow)CurrentSelectedControlCollection[i]).IsSelectd = false;
        }

        CurrentSelectedControlCollection.Clear();

        if (uc != null)
        {
            if (!(uc is IFlow))
                ((IRoom)uc).IsSelectd = true;
            else
                ((IFlow)uc).IsSelectd = true;
            AddSelectedControl(uc);
        }
        mouseIsInContainer = true;
        IsSelectedByShiftKey = false;
    }

    //*******************************************************************
    /// <summary>選択した部品の移動処理（uc以外）</summary>
    /// <param name="x">x座標</param>
    /// <param name="y">y座標</param>
    /// <param name="uc">部品</param>
    //*******************************************************************
    public void MoveControlCollectionByDisplacement(double x, double y, UserControl uc)
    {
        if (CurrentSelectedControlCollection == null || CurrentSelectedControlCollection.Count == 0)
            return;

        CommonItem selectItem = uc as CommonItem;

        CommonItem tmpItem = null;
        for (int i = 0; i < CurrentSelectedControlCollection.Count; i++)
        {
            if (CurrentSelectedControlCollection[i] is CommonItem)
            {
                tmpItem = CurrentSelectedControlCollection[i] as CommonItem;
                if (tmpItem == selectItem)
                    continue;
                tmpItem.SetPositionByDisplacement(x, y);
            }
        }
    }

    //*******************************************************************
    /// <summary>選択した部品のフロー名の位置セット（uc以外）</summary>
    /// <param name="uc">部品</param>
    //*******************************************************************
    public void SetControlCollectionItemAndRuleNameControlPosition(UserControl uc)
    {
        if (CurrentSelectedControlCollection == null || CurrentSelectedControlCollection.Count == 0)
            return;

        CommonItem selectItem = uc as CommonItem;

        CommonItem tmpItem = null;
        for (int i = 0; i < CurrentSelectedControlCollection.Count; i++)
        {
            if (CurrentSelectedControlCollection[i] is CommonItem)
            {
                tmpItem = CurrentSelectedControlCollection[i] as CommonItem;
                if (tmpItem == selectItem)
                    continue;

                double x = (double)tmpItem.GetValue(Canvas.LeftProperty);
                double y = (double)tmpItem.GetValue(Canvas.TopProperty);

                SetItemPosition(tmpItem.JobId, x, y);
                // フローのTrue、Falseの位置をセット
                foreach (IFlow flow in tmpItem.BeginFlowList)
                    flow.setRuleNameControlPosition();
                foreach (IFlow flow in tmpItem.EndFlowList)
                    flow.setRuleNameControlPosition();
            }
        }
    }

    // 2012.11.1一旦削除（線の連接点を別のアイコンに変更用）

    ///// <summary>
    ///// 位置によって部品を取得。

    ///// </summary>
    ///// <param name="point">位置</param>
    //public CommonItem GetItemByPoint(Point point)
    //{
    //    for (int i = 0; i < this.JobItems.Count; i++)
    //    {
    //        if (((CommonItem)this.JobItems[i]).PointIsInside(point))
    //            return (CommonItem)this.JobItems[i];
    //    }
    //    return null;
    //}

    //*******************************************************************
    /// <summary>コンテナに部品を含むかどうかの判定</summary>
    /// <param name="uie">部品</param>
    //*******************************************************************
    public bool Contains(UIElement uie)
    {
        return cnsDesignerContainer.Children.Contains(uie);
    }

    //*******************************************************************
    /// <summary>選択部品を削除</summary>
    //*******************************************************************
    public void DeleteSelectedControl()
    {
        //if (_currentSelectedControlCollection == null
        //    || _currentSelectedControlCollection.Count < 1
        //    || !(_currentSelectedControlCollection[0] is IRoom))
        //    return;

        if (_currentSelectedControlCollection == null
            || _currentSelectedControlCollection.Count < 1)
            return;

        for (int i = CurrentSelectedControlCollection.Count - 1; i >= 0; i--) 
        {
            if (CurrentSelectedControlCollection[i] is CommonItem)
            {
                // ジョブを削除 
                CommonItem item = (CommonItem)_currentSelectedControlCollection[i];

                string jobid = item.JobId;

                DataRow[] rows = JobControlTable.Select("job_id='" + jobid + "'");

                // アイコン設定テーブルにデータを削除 
                ElementType type = (ElementType)(Convert.ToInt16(rows[0]["job_type"]));
                DeleteIconSetting(jobid, type);

                // ジョブ管理テーブルから削除 
                if (rows != null && rows.Count() > 0)
                    rows[0].Delete();

                //JobControlTable.AcceptChanges();

                // フローを削除 
                DataRow[] rowsFlow = FlowControlTable.Select("start_job_id='" + jobid + "' or end_job_id='" + jobid + "'");

                foreach (DataRow row in rowsFlow)
                    row.Delete();

                // アイコンを削除 
                ((IRoom)_currentSelectedControlCollection[i]).Delete();
                _currentSelectedControlCollection.Remove(item);

                _jobItems.Remove(jobid);
                SetedJobIds.Remove(jobid);
            }
        }
           
    }

    //*******************************************************************
    /// <summary>選択フローを削除</summary>
    //*******************************************************************
    public void DeleteSelectedFlow()
    {
        //if (CurrentSelectedControlCollection == null
        //    || CurrentSelectedControlCollection.Count != 1
        //    || !(CurrentSelectedControlCollection[0] is IFlow))
        //    return;

        if (CurrentSelectedControlCollection == null
            || CurrentSelectedControlCollection.Count != 1)
            return;
        IFlow flow = (IFlow)_currentSelectedControlCollection[0] ;

        string beginJobId = flow.BeginItem.JobId;
        string endJobId = flow.EndItem.JobId;

        DataRow[] rows = FlowControlTable.Select("start_job_id='" + beginJobId + "' and end_job_id='" + endJobId + "'");

        if(rows != null && rows.Count() > 0)
            rows[0].Delete();

        RemoveFlow(flow);
    }

    //*******************************************************************
    /// <summary>特定の部品を削除</summary>
    /// <param name="uie">部品</param>
    //*******************************************************************
    public void RemoveItem(Control a)
    {
        if (cnsDesignerContainer.Children.Contains(a))
            cnsDesignerContainer.Children.Remove(a);
    }

    //*******************************************************************
    /// <summary>特定のフローを削除</summary>
    /// <param name="a">アイコン</param>
    //*******************************************************************
    public void RemoveFlow(IFlow a)
    {
        if (cnsDesignerContainer.Children.Contains((UIElement)a))
            cnsDesignerContainer.Children.Remove((UIElement)a);
    }

    //*******************************************************************
    /// <summary>ジョブ起動</summary>
    //*******************************************************************
    public void JobRun(Object JobId)
    {        
        DateTime now = DBUtil.GetSysTime();
        String startXPoint = DBUtil.GetParameterVelue("JOBNET_DUMMY_START_X");
        String startYPoint = DBUtil.GetParameterVelue("JOBNET_DUMMY_START_Y");
        String jobXPoint = DBUtil.GetParameterVelue("JOBNET_DUMMY_JOB_X");
        String jobYPoint = DBUtil.GetParameterVelue("JOBNET_DUMMY_JOB_Y");
        String endXPoint = DBUtil.GetParameterVelue("JOBNET_DUMMY_END_X");
        String endYPoint = DBUtil.GetParameterVelue("JOBNET_DUMMY_END_Y");
        dbAccess.CreateSqlConnect();
        dbAccess.BeginTransaction();
        DataTable dt_inner_jobnet = dbAccess.ExecuteQuery(GET_INNER_JOBNET_ID_SQL);
        DataTable dt_inner_job_start = dbAccess.ExecuteQuery(GET_INNER_JOB_ID_SQL_START);
        DataTable dt_inner_flow1 = dbAccess.ExecuteQuery(GET_INNER_FLOW_ID_SQL1);

        String strInnerJobnetNextId = "";
        String strInnerJobNextIdStart = "";
        String strInnerJobNextIdJob = "";
        String strInnerJobNextIdEnd = "";
        String strInnerFlowNextId1 = "";
        String strInnerFlowNextId2 = "";
        String strJobnetNextId = "";

        string strNow = now.ToString("yyyyMMddHHmmss");
        int runType = (int)Consts.RunTypeEnum.Job;
        String jobnetName = ((jp.co.ftf.jobcontroller.JobController.Form.JobEdit.JobEdit)ParantWindow).tbxJobNetId.Text;
        String runJobnetName = jobnetName + "/" + JobId;
        int nameLen = runJobnetName.Length;
        if (nameLen > 64)
        {
            jobnetName = jobnetName.Substring(0, jobnetName.Length + 64 - nameLen);
        }
        runJobnetName = jobnetName + "/" + JobId;

        DataRow[] rowJob = JobControlTable.Select("job_id='" + JobId + "'");
        DataRow[] rowIconJob = IconJobTable.Select("job_id='" + JobId + "'");
        DataRow[] rowJobCommand = JobCommandTable.Select("job_id='" + JobId + "'");
        DataRow[] rowJobValue = ValueJobTable.Select("job_id='" + JobId + "'");
        DataRow[] rowJobconValue = ValueJobConTable.Select("job_id='" + JobId + "'");

        if (dt_inner_jobnet.Rows.Count == 1 && dt_inner_job_start.Rows.Count == 1 && dt_inner_flow1.Rows.Count == 1)
        {
            strInnerJobnetNextId = dt_inner_jobnet.Rows[0]["NEXTID"].ToString();
            strInnerJobNextIdStart = dt_inner_job_start.Rows[0]["NEXTID"].ToString();
            strInnerJobNextIdJob = (Convert.ToDecimal(dt_inner_job_start.Rows[0]["NEXTID"]) + 1).ToString();
            strInnerJobNextIdEnd = (Convert.ToDecimal(dt_inner_job_start.Rows[0]["NEXTID"]) + 2).ToString();
            strInnerFlowNextId1 = dt_inner_flow1.Rows[0]["NEXTID"].ToString();
            strInnerFlowNextId2 = (Convert.ToDecimal(dt_inner_flow1.Rows[0]["NEXTID"]) + 1).ToString();
            strJobnetNextId = "RUN_JOB_" + dt_inner_jobnet.Rows[0]["NEXTID"].ToString();

        }
        else
        {
            dbAccess.TransactionRollback();
            dbAccess.CloseSqlConnect();
            throw new DBException(Consts.SYSERR_004, null);
        }
        String insertRunJobnet = "insert into ja_run_jobnet_table "
                + "(inner_jobnet_id, inner_jobnet_main_id, inner_job_id, update_date, run_type, "
                + "main_flag, status, start_time, end_time, public_flag, jobnet_id, user_name, jobnet_name, memo) "
                + "VALUES (?,?,0,?,?,0,0,0,0,0,?,?,?,null)";
        List<ComSqlParam> insertRunJobnetSqlParams = new List<ComSqlParam>();
        insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
        insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_main_id", strInnerJobnetNextId));
        insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@update_date", strNow));
        insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@run_type", (int)runType));
        insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", strJobnetNextId));
        insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@user_name", LoginSetting.UserName));
        insertRunJobnetSqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_name", runJobnetName));
        dbAccess.ExecuteNonQuery(insertRunJobnet, insertRunJobnetSqlParams);

        String insertRunJobnetSummary = "insert into ja_run_jobnet_summary_table "
                + "(inner_jobnet_id, update_date, run_type, invo_flag,"
                + "start_time, end_time, public_flag, jobnet_id, user_name, jobnet_name, memo) "
                + "VALUES (?,?,?,1,0,0,0,?,?,?,null)";
        List<ComSqlParam> insertRunJobnetSummarySqlParams = new List<ComSqlParam>();
        insertRunJobnetSummarySqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
        insertRunJobnetSummarySqlParams.Add(new ComSqlParam(DbType.String, "@update_date", strNow));
        insertRunJobnetSummarySqlParams.Add(new ComSqlParam(DbType.String, "@run_type", (int)runType));
        insertRunJobnetSummarySqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_id", strJobnetNextId));
        insertRunJobnetSummarySqlParams.Add(new ComSqlParam(DbType.String, "@user_name", LoginSetting.UserName));
        insertRunJobnetSummarySqlParams.Add(new ComSqlParam(DbType.String, "@jobnet_name", runJobnetName));
        dbAccess.ExecuteNonQuery(insertRunJobnetSummary, insertRunJobnetSummarySqlParams);

        String insertRunJobStart = "insert into ja_run_job_table "
                + "(inner_job_id, inner_jobnet_id, inner_jobnet_main_id, job_type, invo_flag,"
                + "boot_count, start_time, end_time, point_x, point_y, job_id) "
                + "VALUES (?,?,?,?,1,0,0,0,?,?,?)";
        List<ComSqlParam> insertRunJobStartSqlParams = new List<ComSqlParam>();
        insertRunJobStartSqlParams.Add(new ComSqlParam(DbType.String, "@inner_job_id", strInnerJobNextIdStart));
        insertRunJobStartSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
        insertRunJobStartSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_main_id", strInnerJobnetNextId));
        insertRunJobStartSqlParams.Add(new ComSqlParam(DbType.String, "@job_type", (int)ElementType.START));
        insertRunJobStartSqlParams.Add(new ComSqlParam(DbType.String, "@point_x", startXPoint));
        insertRunJobStartSqlParams.Add(new ComSqlParam(DbType.String, "@point_y", startYPoint));
        insertRunJobStartSqlParams.Add(new ComSqlParam(DbType.String, "@job_id", "START"));
        dbAccess.ExecuteNonQuery(insertRunJobStart, insertRunJobStartSqlParams);


        String insertRunJobJob = "insert into ja_run_job_table "
                + "(inner_job_id, inner_jobnet_id, inner_jobnet_main_id, job_type, invo_flag,"
                + "boot_count, start_time, end_time, point_x, point_y, job_id, job_name) "
                + "VALUES (?,?,?,?,1,1,0,0,?,?,?,?)";
        List<ComSqlParam> insertRunJobJobSqlParams = new List<ComSqlParam>();
        insertRunJobJobSqlParams.Add(new ComSqlParam(DbType.String, "@inner_job_id", strInnerJobNextIdJob));
        insertRunJobJobSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
        insertRunJobJobSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_main_id", strInnerJobnetNextId));
        insertRunJobJobSqlParams.Add(new ComSqlParam(DbType.String, "@job_type", (int)ElementType.JOB));
        insertRunJobJobSqlParams.Add(new ComSqlParam(DbType.String, "@point_x", jobXPoint));
        insertRunJobJobSqlParams.Add(new ComSqlParam(DbType.String, "@point_y", jobYPoint));
        insertRunJobJobSqlParams.Add(new ComSqlParam(DbType.String, "@job_id", rowIconJob[0]["job_id"]));
        insertRunJobJobSqlParams.Add(new ComSqlParam(DbType.String, "@job_name", rowJob[0]["job_name"]));
        dbAccess.ExecuteNonQuery(insertRunJobJob, insertRunJobJobSqlParams);

        String insertRunJobEnd = "insert into ja_run_job_table "
                + "(inner_job_id, inner_jobnet_id, inner_jobnet_main_id, job_type, invo_flag,"
                + "boot_count, start_time, end_time, point_x, point_y, job_id) "
                + "VALUES (?,?,?,?,1,1,0,0,?,?,?)";
        List<ComSqlParam> insertRunJobEndSqlParams = new List<ComSqlParam>();
        insertRunJobEndSqlParams.Add(new ComSqlParam(DbType.String, "@inner_job_id", strInnerJobNextIdEnd));
        insertRunJobEndSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
        insertRunJobEndSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_main_id", strInnerJobnetNextId));
        insertRunJobEndSqlParams.Add(new ComSqlParam(DbType.String, "@job_type", (int)ElementType.END));
        insertRunJobEndSqlParams.Add(new ComSqlParam(DbType.String, "@point_x", endXPoint));
        insertRunJobEndSqlParams.Add(new ComSqlParam(DbType.String, "@point_y", endYPoint));
        insertRunJobEndSqlParams.Add(new ComSqlParam(DbType.String, "@job_id", "END-1"));
        dbAccess.ExecuteNonQuery(insertRunJobEnd, insertRunJobEndSqlParams);

        String insertRunJobIcon = "insert into ja_run_icon_job_table "
                + "(inner_job_id, inner_jobnet_id, host_flag, stop_flag, command_type, timeout,host_name, stop_code) "
                + "VALUES (?,?,?,?,?,?,?,?)";
        List<ComSqlParam> insertRunJobIconSqlParams = new List<ComSqlParam>();
        insertRunJobIconSqlParams.Add(new ComSqlParam(DbType.String, "@inner_job_id", strInnerJobNextIdJob));
        insertRunJobIconSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
        insertRunJobIconSqlParams.Add(new ComSqlParam(DbType.String, "@host_flag", rowIconJob[0]["host_flag"]));
        insertRunJobIconSqlParams.Add(new ComSqlParam(DbType.String, "@stop_flag", rowIconJob[0]["stop_flag"]));
        insertRunJobIconSqlParams.Add(new ComSqlParam(DbType.String, "@command_type", rowIconJob[0]["command_type"]));
        insertRunJobIconSqlParams.Add(new ComSqlParam(DbType.String, "@timeout", rowIconJob[0]["timeout"]));
        insertRunJobIconSqlParams.Add(new ComSqlParam(DbType.String, "@host_name", rowIconJob[0]["host_name"]));
        insertRunJobIconSqlParams.Add(new ComSqlParam(DbType.String, "@stop_code", rowIconJob[0]["stop_code"]));
        dbAccess.ExecuteNonQuery(insertRunJobIcon, insertRunJobIconSqlParams);

        String insertRunEndIcon = "insert into ja_run_icon_end_table "
                + "(inner_job_id, inner_jobnet_id) "
                + "VALUES (?,?)";
        List<ComSqlParam> insertRunEndIconSqlParams = new List<ComSqlParam>();
        insertRunEndIconSqlParams.Add(new ComSqlParam(DbType.String, "@inner_job_id", strInnerJobNextIdJob));
        insertRunEndIconSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
        dbAccess.ExecuteNonQuery(insertRunEndIcon, insertRunEndIconSqlParams);

        String insertRunFlow1 = "insert into ja_run_flow_table "
                + "(inner_flow_id, inner_jobnet_id, start_inner_job_id, end_inner_job_id) "
                + "VALUES (?,?,?,?)";
        List<ComSqlParam> insertRunFlow1SqlParams = new List<ComSqlParam>();
        insertRunFlow1SqlParams.Add(new ComSqlParam(DbType.String, "@inner_flow_id", strInnerFlowNextId1));
        insertRunFlow1SqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
        insertRunFlow1SqlParams.Add(new ComSqlParam(DbType.String, "@start_inner_job_id", strInnerJobNextIdStart));
        insertRunFlow1SqlParams.Add(new ComSqlParam(DbType.String, "@end_inner_job_id", strInnerJobNextIdJob));
        dbAccess.ExecuteNonQuery(insertRunFlow1, insertRunFlow1SqlParams);

        String insertRunFlow2 = "insert into ja_run_flow_table "
                + "(inner_flow_id, inner_jobnet_id, start_inner_job_id, end_inner_job_id) "
                + "VALUES (?,?,?,?)";
        List<ComSqlParam> insertRunFlow2SqlParams = new List<ComSqlParam>();
        insertRunFlow2SqlParams.Add(new ComSqlParam(DbType.String, "@inner_flow_id", strInnerFlowNextId2));
        insertRunFlow2SqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
        insertRunFlow2SqlParams.Add(new ComSqlParam(DbType.String, "@start_inner_job_id", strInnerJobNextIdJob));
        insertRunFlow2SqlParams.Add(new ComSqlParam(DbType.String, "@end_inner_job_id", strInnerJobNextIdEnd));
        dbAccess.ExecuteNonQuery(insertRunFlow2, insertRunFlow2SqlParams);


        String insertRunJobCommand = "";
        foreach (DataRow row in rowJobCommand)
        {
            insertRunJobCommand = "insert into ja_run_job_command_table "
                + "(inner_job_id, inner_jobnet_id, command_cls, command) "
                + "VALUES (?,?,?,?)";
            List<ComSqlParam> insertRunJobCommandSqlParams = new List<ComSqlParam>();
            insertRunJobCommandSqlParams.Add(new ComSqlParam(DbType.String, "@inner_job_id", strInnerJobNextIdJob));
            insertRunJobCommandSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
            insertRunJobCommandSqlParams.Add(new ComSqlParam(DbType.String, "@command_cls", row["command_cls"]));
            insertRunJobCommandSqlParams.Add(new ComSqlParam(DbType.String, "@command", row["command"]));
            dbAccess.ExecuteNonQuery(insertRunJobCommand, insertRunJobCommandSqlParams);

        }
        String insertRunJobValue = "";
        foreach (DataRow row in rowJobValue)
        {
            insertRunJobValue = "insert into ja_run_value_job_table "
                            + "(inner_job_id, inner_jobnet_id, value_name, value) "
                            + "VALUES (?,?,?,?)";
            List<ComSqlParam> insertRunJobValueSqlParams = new List<ComSqlParam>();
            insertRunJobValueSqlParams.Add(new ComSqlParam(DbType.String, "@inner_job_id", strInnerJobNextIdJob));
            insertRunJobValueSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
            insertRunJobValueSqlParams.Add(new ComSqlParam(DbType.String, "@value_name", row["value_name"]));
            insertRunJobValueSqlParams.Add(new ComSqlParam(DbType.String, "@value", row["value"]));
            dbAccess.ExecuteNonQuery(insertRunJobValue, insertRunJobValueSqlParams);
        }
        String insertRunJobconValue = "";
        foreach (DataRow row in rowJobconValue)
        {
            insertRunJobconValue = "insert into ja_run_value_jobcon_table"
                            + " (inner_job_id, inner_jobnet_id, value_name) "
                            + "VALUES (?,?,?)";
            List<ComSqlParam> insertRunJobconValueSqlParams = new List<ComSqlParam>();
            insertRunJobconValueSqlParams.Add(new ComSqlParam(DbType.String, "@inner_job_id", strInnerJobNextIdJob));
            insertRunJobconValueSqlParams.Add(new ComSqlParam(DbType.String, "@inner_jobnet_id", strInnerJobnetNextId));
            insertRunJobconValueSqlParams.Add(new ComSqlParam(DbType.String, "@value_name", row["value_name"]));
            dbAccess.ExecuteNonQuery(insertRunJobconValue, insertRunJobconValueSqlParams);
        }
        String updateInnerJobnetIndex = "UPDATE ja_index_table SET nextid = nextid + 1 WHERE count_id = 2";
        dbAccess.ExecuteNonQuery(updateInnerJobnetIndex);
        String updateInnerJobIndex = "UPDATE ja_index_table SET nextid = nextid + 3 WHERE count_id = 20";
        dbAccess.ExecuteNonQuery(updateInnerJobIndex);
        String updateInnerFlowIndex = "UPDATE ja_index_table SET nextid = nextid + 2 WHERE count_id = 30";
        dbAccess.ExecuteNonQuery(updateInnerFlowIndex);

        dbAccess.TransactionCommit();
        dbAccess.CloseSqlConnect();

        JobnetExecDetail detail = new JobnetExecDetail(strInnerJobnetNextId, false);
        detail.Topmost = true;
        detail.Show();
    }

    /// <summary>MouseMoveイベントを削除</summary>
    public void RemoveContainerMoveEvent()
    {
        cnsDesignerContainer.MouseMove -= Container_MouseMove;
    }

    #endregion

    #region privateメッソド

    internal void ScrollIfNeeded(Point mouseLocation)
    {
        if (svContainer != null)
        {
            double scrollVerticalOffset = 0.0;
            double scrollHorizontalOffset = 0.0;

            // See if we need to scroll down 
            if (svContainer.ViewportHeight - mouseLocation.Y < 15.0)
            {
                scrollVerticalOffset = 1.0;
            }
            else if (mouseLocation.Y < 15.0)
            {
                scrollVerticalOffset = -1.0;
            }
            if (svContainer.ViewportWidth - mouseLocation.X < 15.0)
            {
                scrollHorizontalOffset = 1.0;
            }
            else if (mouseLocation.X < 15.0)
            {
                scrollHorizontalOffset = -1.0;
            }

            // Scroll the tree down or up 
            if (scrollVerticalOffset != 0.0)
            {
                scrollVerticalOffset += svContainer.VerticalOffset;

                if (scrollVerticalOffset < 0.0)
                {
                    scrollVerticalOffset = 0.0;
                }
                else if (scrollVerticalOffset > svContainer.ScrollableHeight)
                {
                    scrollVerticalOffset = svContainer.ScrollableHeight;
                }

                svContainer.ScrollToVerticalOffset(scrollVerticalOffset);
            }

            if (scrollHorizontalOffset != 0.0)
            {
                scrollHorizontalOffset += svContainer.HorizontalOffset;

                if (scrollHorizontalOffset < 0.0)
                {
                    scrollHorizontalOffset = 0.0;
                }
                else if (scrollHorizontalOffset > svContainer.ScrollableHeight)
                {
                    scrollHorizontalOffset = svContainer.ScrollableHeight;
                }

                svContainer.ScrollToHorizontalOffset(scrollHorizontalOffset);
            }
        }
    }

    #region コンテクストの制限処理

    //*******************************************************************
    /// <summary>コンテクスの利用可能をセット</summary>
    //*******************************************************************
    private void SetContextStatus()
    {
        // フローを利用可能判定 
        if (!IsFlowEnable())
        {
            menuitemLine.IsEnabled = false;
            menuitemCurve.IsEnabled = false;
        }
        else
        {
            menuitemLine.IsEnabled = true;
            menuitemCurve.IsEnabled = true;
        }

        // TRUE設定を利用可能判定 
        if (!IsSetTrueEnable())
            menuitemSetTrue.IsEnabled = false;
        else
            menuitemSetTrue.IsEnabled = true;

        // TRUE設定を利用可能判定 
        if (!IsSetFalseEnable())
            menuitemSetFalse.IsEnabled = false;
        else
            menuitemSetFalse.IsEnabled = true;

        // フロー削除を利用可能判定 
        if (!IsDelFlowEnable())
            menuitemDelFlow.IsEnabled = false;
        else
            menuitemDelFlow.IsEnabled = true;

        // アイコン削除を利用可能判定 
        if (!IsDelIconEnable())
            menuitemDel.IsEnabled = false;
        else
            menuitemDel.IsEnabled = true;

        // アイコン削除を利用可能判定 
        if (!IsSettingEnable())
            menuitemSet.IsEnabled = false;
        else
            menuitemSet.IsEnabled = true;

        // ジョブ起動を利用可能判定 
        if (!IsJobRunEnable())
            menuitemJobRun.IsEnabled = false;
        else
            menuitemJobRun.IsEnabled = true;
    }

    //*******************************************************************
    /// <summary>フローを利用可能判定</summary>
    //*******************************************************************
    private bool IsFlowEnable()
    {
        bool result = true;

        result = IsFlowEnablePositiveDirection();
        if (!result && !IsSelectedByShiftKey)
        {
            result = IsFlowEnableReverseDirection();
        }
        return result;       
    }

    //*******************************************************************
    /// <summary>フローを利用可能判定</summary>
    //*******************************************************************
    private bool IsFlowEnablePositiveDirection()
    {
        // アイコン未選択、または２つ以外のアイコンを選択 
        if (_currentSelectedControlCollection == null
            || _currentSelectedControlCollection.Count != 2
            || !(_currentSelectedControlCollection[0] is IRoom)
            || !(_currentSelectedControlCollection[1] is IRoom))
            return false;

        IElement item1 = ((IRoom)_currentSelectedControlCollection[0]).ContentItem;
        IElement item2 = ((IRoom)_currentSelectedControlCollection[1]).ContentItem;

        // 最初に選択されたアイコンが終了アイコン 
        if (item1 is End)
            return false;

        // 最後に選択されたアイコンが開始アイコン 
        if (item2 is Start)
            return false;

        // すでにフローが設定済みのアイコンを選択。（フロー管理テーブルに同一データが存在する） 
        string job1 = item1.JobId;
        string job2 = item2.JobId;

        DataRow[] existFlows = FlowControlTable.Select
            ("start_job_id ='" + job1 + "' and end_job_id='" + job2 + "'");
        if (existFlows != null && existFlows.Count() > 0)
            return false;

        // 最初に選択されたアイコンが開始アイコンで、


        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (item1 is Start)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最後に選択されたアイコンが終了アイコンで、 
        // フロー管理テーブル（INフロージョブIDをキー）に１件以上のデータが存在する 
        if (item2 is End)
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最後に選択されたアイコンが条件分岐アイコンで、 
        // フロー管理テーブル（INフロージョブIDをキー）に１件以上のデータが存在する 
        if (item2 is If)
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最初に選択されたアイコンが条件分岐アイコンで、 
        // フロー管理テーブル（OUTフロージョブIDをキー）に２件以上のデータが存在する 
        if (item1 is If)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 2)
                return false;
        }

        // 最後に選択されたアイコンが並行処理開始アイコンで、 
        // フロー管理テーブル（INフロージョブIDをキー）に１件以上のデータが存在する 
        if (item2 is Mts)
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最初に選択されたアイコンが並行処理終了アイコンで、 
        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (item1 is Mte)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        //added by kim 2012/11/15
        // 最初に選択されたアイコンが分岐処理終了アイコンで、 
        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (item1 is Ife)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最後に選択されたアイコンがループアイコンで、 
        // フロー管理テーブル（ＩＮフロージョブIDをキー）に２件以上のデータが存在する 
        if (item2 is Loop)
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 2)
                return false;
        }

        // 最初に選択されたアイコンがループアイコンで、 
        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (item1 is Loop)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最後に選択されたアイコンが上記以外のアイコンで、 
        // (開始、終了、条件分岐、分岐終了、並行処理開始、並行処理終了、ループ以外) 
        // フロー管理テーブル（INフロージョブIDをキー）に１件以上のデータが存在する 
        if (!(item2 is Start) && !(item2 is End) && !(item2 is If)
            && !(item2 is Mts) && !(item2 is Mte) && !(item2 is Loop) && !(item2 is Ife))
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最初に選択されたアイコンが上記以外のアイコンで、 
        // (開始、終了、条件分岐、分岐終了、並行処理開始、並行処理終了、ループ以外) 
        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (!(item1 is Start) && !(item1 is End) && !(item1 is If)
            && !(item1 is Mts) && !(item1 is Mte) && !(item1 is Loop) && !(item1 is Ife))
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        return true;
    }

    //*******************************************************************
    /// <summary>フローを利用可能判定</summary>
    //*******************************************************************
    private bool IsFlowEnableReverseDirection()
    {
        // アイコン未選択、または２つ以外のアイコンを選択 
        if (_currentSelectedControlCollection == null
            || _currentSelectedControlCollection.Count != 2
            || !(_currentSelectedControlCollection[0] is IRoom)
            || !(_currentSelectedControlCollection[1] is IRoom))
            return false;

        IElement item1 = ((IRoom)_currentSelectedControlCollection[1]).ContentItem;
        IElement item2 = ((IRoom)_currentSelectedControlCollection[0]).ContentItem;

        // 最初に選択されたアイコンが終了アイコン 
        if (item1 is End)
            return false;

        // 最後に選択されたアイコンが開始アイコン 
        if (item2 is Start)
            return false;

        // すでにフローが設定済みのアイコンを選択。（フロー管理テーブルに同一データが存在する） 
        string job1 = item1.JobId;
        string job2 = item2.JobId;

        DataRow[] existFlows = FlowControlTable.Select
            ("start_job_id ='" + job1 + "' and end_job_id='" + job2 + "'");
        if (existFlows != null && existFlows.Count() > 0)
            return false;

        // 最初に選択されたアイコンが開始アイコンで、


        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (item1 is Start)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最後に選択されたアイコンが終了アイコンで、 
        // フロー管理テーブル（INフロージョブIDをキー）に１件以上のデータが存在する 
        if (item2 is End)
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最後に選択されたアイコンが条件分岐アイコンで、 
        // フロー管理テーブル（INフロージョブIDをキー）に１件以上のデータが存在する 
        if (item2 is If)
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最初に選択されたアイコンが条件分岐アイコンで、 
        // フロー管理テーブル（OUTフロージョブIDをキー）に２件以上のデータが存在する 
        if (item1 is If)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 2)
                return false;
        }

        // 最後に選択されたアイコンが並行処理開始アイコンで、 
        // フロー管理テーブル（INフロージョブIDをキー）に１件以上のデータが存在する 
        if (item2 is Mts)
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最初に選択されたアイコンが並行処理終了アイコンで、 
        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (item1 is Mte)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        //added by kim 2012/11/15
        // 最初に選択されたアイコンが分岐処理終了アイコンで、 
        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (item1 is Ife)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最後に選択されたアイコンがループアイコンで、 
        // フロー管理テーブル（ＩＮフロージョブIDをキー）に２件以上のデータが存在する 
        if (item2 is Loop)
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 2)
                return false;
        }

        // 最初に選択されたアイコンがループアイコンで、 
        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (item1 is Loop)
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最後に選択されたアイコンが上記以外のアイコンで、 
        // (開始、終了、条件分岐、分岐終了、並行処理開始、並行処理終了、ループ以外) 
        // フロー管理テーブル（INフロージョブIDをキー）に１件以上のデータが存在する 
        if (!(item2 is Start) && !(item2 is End) && !(item2 is If)
            && !(item2 is Mts) && !(item2 is Mte) && !(item2 is Loop) && !(item2 is Ife))
        {
            existFlows = FlowControlTable.Select
                ("end_job_id='" + job2 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }

        // 最初に選択されたアイコンが上記以外のアイコンで、 
        // (開始、終了、条件分岐、分岐終了、並行処理開始、並行処理終了、ループ以外) 
        // フロー管理テーブル（OUTフロージョブIDをキー）に１件以上のデータが存在する 
        if (!(item1 is Start) && !(item1 is End) && !(item1 is If)
            && !(item1 is Mts) && !(item1 is Mte) && !(item1 is Loop) && !(item1 is Ife))
        {
            existFlows = FlowControlTable.Select
                ("start_job_id='" + job1 + "'");
            if (existFlows != null && existFlows.Count() >= 1)
                return false;
        }
        _currentSelectedControlCollection.Reverse();
        return true;
    }

    //*******************************************************************
    /// <summary>TRUE設定を利用可能判定</summary>
    //*******************************************************************
    private bool IsSetTrueEnable()
    {
        // 対象：条件分岐アイコンに設定されたOUTフロー 
        if (_currentSelectedControlCollection == null
            || _currentSelectedControlCollection.Count != 1
            || !(_currentSelectedControlCollection[0] is IFlow))
            return false;

        IFlow flow = (IFlow)_currentSelectedControlCollection[0];
        IRoom item1 = flow.BeginItem;
        //IRoom item2 = flow.EndItem;
        string beginJob = item1.JobId;
        //string endJob = item2.JobId;

        // フロー管理テーブルのOUTフロージョブIDをキーにジョブ管理テーブルを検索し、 
        // ジョブタイプが「条件分岐」以外 
        DataRow[] existFlows = JobControlTable.Select
            ("job_id ='" + beginJob + "'");

        if (existFlows != null && existFlows.Count() > 0)
        {
            ElementType type = (ElementType)(existFlows[0]["job_type"]);
            if (ElementType.IF != type)
                return false;
        }

        // フロー管理テーブルのOUTフロージョブIDとフロータイプ「TRUE」をキーに、 
        // フロー管理テーブルのOUTフロージョブIDで検索し、 
        // １件以上のデータが存在する 
        existFlows = FlowControlTable.Select
            ("start_job_id ='" + beginJob + "' and flow_type=1");

        if (existFlows != null && existFlows.Count() >= 1)
        {
            return false;
        }

        return true;
    }

    //*******************************************************************
    /// <summary>FALSE設定を利用可能判定</summary>
    //*******************************************************************
    private bool IsSetFalseEnable()
    {
        // 対象：条件分岐アイコンに設定されたOUTフロー 
        if (_currentSelectedControlCollection == null
            || _currentSelectedControlCollection.Count != 1
            || !(_currentSelectedControlCollection[0] is IFlow))
            return false;

        IFlow flow = (IFlow)_currentSelectedControlCollection[0];
        IRoom item1 = flow.BeginItem;
        //IRoom item2 = flow.EndItem;
        string beginJob = item1.JobId;
        //string endJob = item2.JobId;

        // フロー管理テーブルのOUTフロージョブIDをキーにジョブ管理テーブルを検索し、 
        // ジョブタイプが「条件分岐」以外 
        DataRow[] existFlows = JobControlTable.Select
            ("job_id ='" + beginJob + "'");

        if (existFlows != null && existFlows.Count() > 0)
        {
            ElementType type = (ElementType)(existFlows[0]["job_type"]);
            if (ElementType.IF != type)
                return false;
        }

        // フロー管理テーブルのOUTフロージョブIDとフロータイプ「FALSE」をキーに、 
        // フロー管理テーブルのOUTフロージョブIDで検索し、 
        // １件以上のデータが存在する 
        existFlows = FlowControlTable.Select
            ("start_job_id ='" + beginJob + "' and flow_type=2");

        if (existFlows != null && existFlows.Count() >= 1)
        {
            return false;
        }

        return true;
    }

    //*******************************************************************
    /// <summary>フロー削除を利用可能判定</summary>
    //*******************************************************************
    private bool IsDelFlowEnable()
    {
        // フロー以外が選択された場合 
        if (_currentSelectedControlCollection == null
            || _currentSelectedControlCollection.Count != 1
            || !(_currentSelectedControlCollection[0] is IFlow))
            return false;

        return true;
    }

    //*******************************************************************
    /// <summary>削除を利用可能判定</summary>
    //*******************************************************************
    private bool IsDelIconEnable()
    {
        // アイコン以外が選択された場合 
        if (_currentSelectedControlCollection == null
            || _currentSelectedControlCollection.Count < 1
            || !(_currentSelectedControlCollection[0] is IRoom))
            return false;

        return true;
    }

    //*******************************************************************
    /// <summary>設定を利用可能判定</summary>
    //*******************************************************************
    private bool IsSettingEnable()
    {
        // アイコン以外が選択された場合 
        if (_currentSelectedControlCollection == null
            || _currentSelectedControlCollection.Count != 1
            || !(_currentSelectedControlCollection[0] is IRoom))
            return false;

        return true;
    }

    //*******************************************************************
    /// <summary>ジョブ起動を利用可能判定</summary>
    //*******************************************************************
    private bool IsJobRunEnable()
    {
        // アイコン以外が選択された場合 
        if (_currentSelectedControlCollection == null
            || _currentSelectedControlCollection.Count != 1
            || !(_currentSelectedControlCollection[0] is IRoom)
            || !(((CommonItem)_currentSelectedControlCollection[0]).ElementType == ElementType.JOB)
            || (!SetedJobIds.Contains(((CommonItem)_currentSelectedControlCollection[0]).JobId) && ((CommonItem)_currentSelectedControlCollection[0]).ItemEditType == Consts.EditType.Add))
            return false;

        return true;
    }
    #endregion

    #region DB処理


    //*******************************************************************
    /// <summary>ジョブ位置設定</summary>
    /// <param name="jobId">ジョブID</param>
    /// <param name="x">x座標</param>
    /// <param name="y">y座標</param>
    //******************************************************************* 
    public void SetItemPosition(string jobId, double x, double y)
    {
        DataRow row = JobControlTable.Select("job_id = '" + jobId + "'")[0];
        row["point_x"] = Convert.ToInt32(x);
        row["point_y"] = Convert.ToInt32(y);
            
    }

    //*******************************************************************
    /// <summary>アイコンの存在チェック</summary>
    /// <param name="type">ジョブタイプ</param>
    /// <returns>true:既存 False:存在しない</returns>
    //*******************************************************************
    private bool CheckJobExist(ElementType type)
    {
        // 開始アイコンの場合 
        if (ElementType.START == type)
        {
            DataRow[] rows = JobControlTable.Select("job_type = 0");
            if (rows != null && rows.Count() > 0)
                return true;
        }

        return false;
    }

    //*******************************************************************
    /// <summary>アイコン設定テーブルのデータを削除</summary>
    /// <param name="jobid"></param>
    /// <param name="type"></param>
    //*******************************************************************
    private void DeleteIconSetting(string jobid, ElementType type)
    {
        string where = "job_id='" + jobid + "'";
        DataRow[] rows = null;
        switch (type)
        {
            // 0:開始、6:並行処理開始、7：並行処理終了、8：ループの場合 
            case ElementType.START:
            case ElementType.LOOP:
            case ElementType.MTS:
            case ElementType.MTE:
            //added by kim 2012/11/14
            case ElementType.IFE:
                break;
            // 1:終了の場合 
            case ElementType.END:
                rows = IconEndTable.Select(where);
                break;
            // 2:条件分岐の場合 
            case ElementType.IF:
                rows = IconIfTable.Select(where);
                break;
            // 3:ジョブコントローラ変数の場合 
            case ElementType.ENV:
                rows = IconValueTable.Select(where);
                break;
            // 4:ジョブの場合 
            case ElementType.JOB:
                rows = IconJobTable.Select(where);
                // ジョブコマンド設定テーブル 
                DataRow[] rowsTmp = JobCommandTable.Select(where);
                if(rowsTmp != null)
                {
                    foreach (DataRow row in rowsTmp)
                    row.Delete();
                }
               
                // ジョブ変数設定テーブル 
                rowsTmp = ValueJobTable.Select(where);
                if(rowsTmp != null)
                {
                    foreach (DataRow row in rowsTmp)
                    row.Delete();
                }
                    
                // ジョブコントローラ変数設定テーブル 
                rowsTmp = ValueJobConTable.Select(where);
                if (rowsTmp != null)
                {
                    foreach (DataRow row in rowsTmp)
                        row.Delete();
                }
                    
                break;
            // 5:ジョブネット 
            case ElementType.JOBNET:
                rows = IconJobnetTable.Select(where);
                break;
            // 9：拡張ジョブの場合 
            case ElementType.EXTJOB:
                rows = IconExtjobTable.Select(where);
                break;
            //  10：計算の場合 
            case ElementType.CAL:
                rows = IconCalcTable.Select(where);
                break;
            // 11：タスク場合 
            case ElementType.TASK:
                rows = IconTaskTable.Select(where);
                break;
            // 12：情報取得場合 
            case ElementType.INF:
                rows = IconInfoTable.Select(where);
                break;
            // 14：ファイル転送場合 
            case ElementType.FCOPY:
                rows = IconFcopyTable.Select(where);
                break;
            // 15：ファイル待ち合わせ場合 
            case ElementType.FWAIT:
                rows = IconFwaitTable.Select(where);
                break;
            // 16：リブート場合 
            case ElementType.REBOOT:
                rows = IconRebootTable.Select(where);
                break;
        }
        // 削除 
        if (rows != null)
        {
            foreach (DataRow row in rows)
                row.Delete();
        }
    }

    private bool IsCanArrowMove(double deltaX, double deltaY)
    {
        CommonItem tmpItem = null;
        bool canMove = false;
        for (int i = 0; i < CurrentSelectedControlCollection.Count; i++)
        {
            if (CurrentSelectedControlCollection[i] is CommonItem)
            {
                tmpItem = CurrentSelectedControlCollection[i] as CommonItem;
                double X = (double)tmpItem.GetValue(Canvas.LeftProperty);
                double Y = (double)tmpItem.GetValue(Canvas.TopProperty);
                if ((X + deltaX <= 0) || (X + deltaX >= cnsDesignerContainer.Width)) return false;
                if ((Y + deltaY <= 0) || (Y + deltaY >= cnsDesignerContainer.Height)) return false;
            }
            canMove = true;
        }
        return canMove;
    }

    private bool[] IsNeedScrollMove(double deltaX, double deltaY)
    {
        CommonItem tmpItem = null;
        bool[] need = { false, false };
        for (int i = 0; i < CurrentSelectedControlCollection.Count; i++)
        {
            if (CurrentSelectedControlCollection[i] is CommonItem)
            {
                tmpItem = CurrentSelectedControlCollection[i] as CommonItem;
                double X = (double)tmpItem.GetValue(Canvas.LeftProperty);
                double Y = (double)tmpItem.GetValue(Canvas.TopProperty);
                if (X + deltaX > svContainer.ViewportWidth) need[0] = true;
                if (Y + deltaY > svContainer.ViewportHeight) need[1] = true;
            }
        }
        return need;
    }

    #endregion

    #endregion

    }
}
