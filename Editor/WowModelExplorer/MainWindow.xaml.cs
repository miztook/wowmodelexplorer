using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;
using WowModelExplorer.Controls;
using System.Threading;
using System.Windows.Threading;
using WowModelExplorer.Controls.Splash;
using WowModelExplorer.Services;
using AvalonDock;
using mywowNet;
using WowModelExplorer.Commands;
using WowModelExplorer.Properties;
using WowModelExplorer.Utility;

namespace WowModelExplorer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly DispatcherTimer timer;
        private WindowHost windowhost;
        private bool isKeyFocus = false;
        private bool isBackMode = false;
        private M2GeosetsWindow _geosetsWindow = null;
        private M2AttachmentsWindow _attachmentsWindow = null;
        private WMOGroupsWindow _groupsWindow = null;
        private M2EquipmentsWindow _equipmentsWindow = null;
        private M2EquipmentSelectWindow _equipmentSelectWindow = null;
        private WMOPortalsWindow _portalsWindow = null;
        private M2CharacterInfoWindow _characterInfoWindow = null;
        private M2ArmoryWindow _armoryWindow = null;
        private AccelerateWindow _accelerateWindow = null;

        public M2GeosetsWindow GeosetsWindow { get { return _geosetsWindow; } }
        public M2AttachmentsWindow AttachmentsWindow { get { return _attachmentsWindow; } }
        public WMOGroupsWindow GroupsWindow { get { return _groupsWindow; } }
        public M2EquipmentsWindow EquipmentsWindow { get { return _equipmentsWindow; } }
        public M2EquipmentSelectWindow EquipmentSelectWindow { get { return _equipmentSelectWindow; } }
        public WMOPortalsWindow PortalsWindow { get { return _portalsWindow; } }
        public M2CharacterInfoWindow CharacterInfoWindow { get { return _characterInfoWindow; } }
        public M2ArmoryWindow ArmoryWindow { get { return _armoryWindow; } }
        public AccelerateWindow AccelerateWindow { get { return _accelerateWindow; } }

        public MainWindow()
        {
            InitializeComponent();

            Loaded += new RoutedEventHandler(MainWindow_Loaded);
            Closing += new CancelEventHandler(MainWindow_Closing);
            StateChanged += new EventHandler(MainWindow_StateChanged);
            LostKeyboardFocus += delegate { isKeyFocus = false;  };

            timer = new DispatcherTimer(DispatcherPriority.ApplicationIdle);
            timer.Interval = new System.TimeSpan(0, 0, 0, 0, 1);
            timer.Tick += new EventHandler(timer_Tick);
        }

        public bool IsKeyFocus 
        { 
            get { return isKeyFocus; }
            set { isKeyFocus = value; }
        }

        public bool IsMaximized
        {
            get { return WindowState == WindowState.Maximized; }
            set
            {
                if (value)
                {
                    WindowState = WindowState.Maximized;
                }
                else if (WindowState == WindowState.Maximized)
                {
                    WindowState = WindowState.Normal;
                }
            }
        }

        public bool IsBackMode
        {
            get { return isBackMode;  }
        }

        private void InitCommandBindings()
        {
            CommandBindings.AddRange(ShellService.Instance.FileCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.AnimationCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.ViewCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.ModelOperationCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.ModelEditViewCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.WmoOperationCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.WmoEditViewCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.OptionCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.SettingCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.LanguageCommands.CommandBindings);
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            this.Loaded -= MainWindow_Loaded;

            SplashWindow _splash = new SplashWindow();
            _splash.Show();

            MessageListener.Instance.StartWork(GetStageText(0), new DispatcherOperationCallback(InitStage0));
            Thread.Sleep(1);

            MessageListener.Instance.StartWork(GetStageText(1), new DispatcherOperationCallback(InitStage1));
            Thread.Sleep(1);

            MessageListener.Instance.StartWork(GetStageText(2), new DispatcherOperationCallback(InitStage2));
            Thread.Sleep(1);

            _splash.Close();
        }

        private string GetStageText(int index)
        {
            switch (index)
            {
                case 0:
                    return Application.Current.FindResource("stage1") as string;
                case 1:
                    return Application.Current.FindResource("stage2") as string;
                case 2:
                    return Application.Current.FindResource("stage3") as string;
            }
           return "";
        }

        private object InitStage0(object frame)
        {
            IntPtr hwnd = EngineService.Instance.InitilalizeEngine();
            windowhost = new WindowHost(hwnd);

            ModelSceneService.Instance.Initialize();

            windowhost.WmResize += new ResizeEventHandler(windowHost_WmResize);
            windowhost.WmMouse += new MouseMessageHandler(windowHost_WmMouse);

            ((DispatcherFrame)frame).Continue = false;
            return null;
        }

        private object InitStage1(object frame)
        {
            EngineService.Instance.RetrieveWowData();

            ((DispatcherFrame)frame).Continue = false;
            return null;
        }

        private object InitStage2(object frame)
        {
            InitCommandBindings();

            _characterContent.Initialize();
            _npcContent.Initialize();
            _spellVisualEffectContent.Initialize();
            _mapModelContent.Initialize();
            _wmoContent.Initialize();
            _worldM2Content.Initialize();

            _clothesContent.Initialize();
            _weaponContent.Initialize();
            _setContent.Initialize();
            _ridableContent.Initialize();

            _propertyContent.Initialize();
            _animationContent.Initialize();
            _m2StateContent.Initialize();

            _mapModelContent.Hide();
            _mapModelNameContent.Hide();
            _propertyContent.Hide();

            //model mode
            if (windowhost.HWnd != IntPtr.Zero)
            {
                _controlHostElement.Child = windowhost;
            }

            _leftDockablePane.SetValue(ResizingPanel.ResizeWidthProperty, new GridLength(280));
            _right2DockablePane.SetValue(ResizingPanel.ResizeWidthProperty, new GridLength(250));
            _rightDockablePane.SetValue(ResizingPanel.ResizeWidthProperty, new GridLength(300));
            _bottomPanel.SetValue(ResizingPanel.ResizeHeightProperty, new GridLength(200));
            _bottomDockablePane.SetValue(ResizingPanel.ResizeWidthProperty, new GridLength(1.0f, GridUnitType.Star));
            _bottom2DockablePane.SetValue(ResizingPanel.ResizeWidthProperty, new GridLength(0.2f, GridUnitType.Star));

            //dialogs
            _geosetsWindow = new M2GeosetsWindow();
            _geosetsWindow.Owner = this;
            _attachmentsWindow = new M2AttachmentsWindow();
            _attachmentsWindow.Owner = this;
            _groupsWindow = new WMOGroupsWindow();
            _groupsWindow.Owner = this;
            _equipmentsWindow = new M2EquipmentsWindow();
            _equipmentsWindow.Owner = this;
            _equipmentSelectWindow = new M2EquipmentSelectWindow();
            _equipmentSelectWindow.Owner = this;
            _portalsWindow = new WMOPortalsWindow();
            _portalsWindow.Owner = this;
            _characterInfoWindow = new M2CharacterInfoWindow();
            _characterInfoWindow.Owner = this;
            _armoryWindow = new M2ArmoryWindow();
            _armoryWindow.Owner = this;
            _accelerateWindow = new AccelerateWindow();
            _accelerateWindow.Owner = this;

            _characterContent.Activate();
            _setContent.Activate();

            //set options
            Engine.Instance.SceneManager.BackImageFileName = ModelEditorService.Instance.BackImageFileName;
            Engine.Instance.SceneManager.ShowDebug(E_SCENE_DEBUG_PART.ESDP_BASE, ModelEditorService.Instance.IsShow(EditorShowOptions.DebugText));
            Engine.Instance.SceneManager.ShowDebug(E_SCENE_DEBUG_PART.ESDP_M2_GEOSETS, ModelEditorService.Instance.IsShow(EditorShowOptions.DebugGeosets));
            ModelSceneService.Instance.GridSceneNode.Visible = ModelEditorService.Instance.IsShow(EditorShowOptions.Plane);
            ModelSceneService.Instance.CoordSceneNode.Visible = ModelEditorService.Instance.IsShow(EditorShowOptions.Coord);
            ModelSceneService.Instance.LightCoordSceneNode.Visible = ModelEditorService.Instance.IsShow(EditorShowOptions.LightCoord);

            timer.Start();

            ((DispatcherFrame)frame).Continue = false;
            return null;
        }

        private void MainWindow_Closing(object sender, CancelEventArgs e)
        {
            if (!e.Cancel)
            {
                timer.Stop();
            }

            ModelSceneService.Instance.SetMainM2SceneNode("", false);
            ModelSceneService.Instance.Shutdown();
            EngineService.Instance.Shutdown();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            //update scene
            EngineService.Instance.Tick();

            //update UI
            if (_animationContent.State != DockableContentState.Hidden)
                Dispatcher.CurrentDispatcher.BeginInvoke(DispatcherPriority.Render, new DispatcherOperationCallback(_tick_UI), null);
        }

        private object _tick_UI(object param)
        {
            _animationContent.UpdateAnimationProgress(ModelEditorService.Instance.StateMode);
            return null;
        }

        private void windowHost_WmResize(object sender, uint w, uint h)
        {
            if (w == 0 || h == 0)
                isBackMode = true;
            else
            {
                isBackMode = false;

                EngineService.Instance.OnResize(w, h);
            }
        }

        private void windowHost_WmMouse(object sender, MouseMessage msg, int x, int y, short zDelta)
        {
            if (msg == MouseMessage.MouseLButtonDown || 
                msg == MouseMessage.MouseLButtonUp ||
                msg == MouseMessage.MouseRButtonDown ||
                msg == MouseMessage.MouseRButtonUp)
            {
                if (!isKeyFocus)
                {
                    Keyboard.Focus(this);
                    NativeMethods.SetFocus(windowhost.HWnd);
                    isKeyFocus = true;
                }
            }

            EngineService.Instance.OnMouseMessage(msg, x, y, zDelta);
        }

        private void MainWindow_StateChanged(object sender, EventArgs e)
        {
            timer.IsEnabled = WindowState != WindowState.Minimized;   
        }
    }
}
