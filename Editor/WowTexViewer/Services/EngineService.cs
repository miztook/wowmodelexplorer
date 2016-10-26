using System;
using System.Windows;
using mywowNet;
using System.Diagnostics;
using WowTexViewer.Controls;
using WowTexViewer.Utility;

namespace WowTexViewer.Services
{
    public class EngineService
    {
        public EngineService()
        {
            if (instance == null)
            {
                instance = this;
            }
        }

        private static EngineService instance = null;
        public static EngineService Instance { get { return instance; } }

        Camera _mainCamera;
        public Camera MainCamera
        {
            get { return _mainCamera; }
        }

        public IntPtr InitilalizeEngine()
        {
            new Engine();

            Engine.Instance.LoadSetting();

            SVideoSetting videoSetting = Engine.Instance.VideoSetting;
            SAdvancedSetting advancedSetting = Engine.Instance.AdvancedSetting;

            E_DRIVER_TYPE driverType = advancedSetting.driverType;
            if (!Engine.Instance.IsAboveVista && driverType == E_DRIVER_TYPE.Direct3D11)
            {
                advancedSetting.driverType = E_DRIVER_TYPE.Direct3D9;
            }

            bool success = Engine.Instance.Init(driverType, 0, false, videoSetting.vsync, videoSetting.antialias, true);
            Debug.Assert(success);
            if (!success && driverType == E_DRIVER_TYPE.Direct3D11)
            {
                driverType = E_DRIVER_TYPE.Direct3D9;
                success = Engine.Instance.Init(driverType, 0, false, videoSetting.vsync, videoSetting.antialias, true);
                Debug.Assert(success);
            }

            Engine.Instance.VideoSetting = videoSetting;
            Engine.Instance.AdvancedSetting = advancedSetting;

            new mwTool();

            _mainCamera = Engine.Instance.SceneManager.AddCamera(new vector3df(0, 50, 0), new vector3df(0, 0, 0), new vector3df(0, 0, 1), 0.5f, 2500.0f, (float)Math.PI / 4.0f);

            return Engine.Instance.HWnd;
        }

        public void RetrieveWowData()
        {
            Engine.Instance.WowDatabase.BuildTextures();
            Engine.Instance.WowDatabase.BuildMaps();
        }

        public void Tick()
        {
            SceneManager smgr = Engine.Instance.SceneManager;

            if (smgr.BeginFrame(ShellService.Instance.MainWindow.IsActive))
            {
                smgr.DrawAll(!ShellService.Instance.MainWindow.IsBackMode);

                smgr.EndFrame();
            }
        }

        public void Shutdown()
        {
            mwTool.Instance.Dispose();

            Engine.Instance.SaveSetting();

            Engine.Instance.Dispose();
        }

        public void OnResize(uint width, uint height)
        {
            Engine.Instance.Driver.SetDisplayMode(width, height);

            Engine.Instance.FontServices.OnWindowSizeChanged(width, height);

            Engine.Instance.SceneManager.OnWindowSizeChanged(width, height);

            Camera cam = Engine.Instance.SceneManager.ActiveCamera;
            if (cam != null)
                cam.RecalculateAll();
        }

        public void OnMouseMessage(MouseMessage message, int x, int y, short zDelta)
        {

        }
    }
}
