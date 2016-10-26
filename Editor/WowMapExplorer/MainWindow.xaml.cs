using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;
using WowMapExplorer.Controls;
using System.Threading;
using System.Windows.Threading;
using WowMapExplorer.Controls.Splash;
using WowMapExplorer.Services;
using AvalonDock;
using mywowNet;
using WowMapExplorer.Commands;
using WowMapExplorer.Utility;

namespace WowMapExplorer
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

        public MainWindow()
        {
            InitializeComponent();

            Loaded += new RoutedEventHandler(MainWindow_Loaded);
            Closing += new CancelEventHandler(MainWindow_Closing);
            StateChanged += new EventHandler(MainWindow_StateChanged);
            LostKeyboardFocus += delegate { isKeyFocus = false; };

            timer = new DispatcherTimer(DispatcherPriority.ApplicationIdle);
            timer.Interval = new System.TimeSpan(0, 0, 0, 0, 1);
            timer.Tick += new EventHandler(timer_Tick);
        }

        public bool IsKeyFocus { get { return isKeyFocus; } }

        public bool IsBackMode
        {
            get { return isBackMode; }
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

        private void InitCommandBindings()
        {
            CommandBindings.AddRange(ShellService.Instance.FileCommands.CommandBindings);

            CommandBindings.AddRange(ShellService.Instance.ViewCommands.CommandBindings);

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

            MapSceneService.Instance.Initialize();

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

            _mapContent.Initialize();

            if (windowhost.HWnd != IntPtr.Zero)
            {
                _controlHostElement.Child = windowhost;
            }

            _leftDockablePane.SetValue(ResizingPanel.ResizeWidthProperty, new GridLength(200));

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

            MapSceneService.Instance.Shutdown();
            EngineService.Instance.Shutdown();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            //update scene
            EngineService.Instance.Tick();

            //update UI
        }

        private object _tick_UI(object param)
        {
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
