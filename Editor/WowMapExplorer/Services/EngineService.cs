using System;
using mywowNet;
using System.Diagnostics;
using WowMapExplorer.Controls;
using WowMapExplorer.Utility;

namespace WowMapExplorer.Services
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

        CameraModes cameraMode = CameraModes.Maya;
        public CameraModes CameraMode
        {
            get { return cameraMode; }
            set { cameraMode = value; }
        }
        float keySpeed = 0.3f;
        public float KeySpeed
        {
            get { return keySpeed; }
            set { keySpeed = value; }
        }

        float wheelSpeed = 0.01f;
        public float WheelSpeed
        {
            get { return wheelSpeed; }
            set { wheelSpeed = value; }
        }

        bool isGameMode = false;
        public bool IsGameMode
        {
            get { return isGameMode; }
            set
            {
                isGameMode = value;
            }
        }

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

            new Client();

            _mainCamera = Engine.Instance.SceneManager.AddCamera(new vector3df(0, 5, -10), new vector3df(0, 0, 0), new vector3df(0, 1, 0), 0.5f, 2500.0f, (float)Math.PI / 4.0f);

            return Engine.Instance.HWnd;;
        }

        public void RetrieveWowData()
        {
            Engine.Instance.WowDatabase.BuildItems();
            Engine.Instance.WowDatabase.BuildNpcs("npcs.csv");
            Engine.Instance.WowDatabase.BuildStartOutfit();
            Engine.Instance.WowDatabase.BuildMaps();
        }

        public void Tick()
        {
            SceneManager smgr = Engine.Instance.SceneManager;

            if (ShellService.Instance.MainWindow.IsActive &&
                ShellService.Instance.MainWindow.IsKeyFocus &&
                !IsGameMode)
            {
                Camera.SKeyControl keycontrol = new Camera.SKeyControl()
                {
                    front = NativeMethods.IsKeyPressed('W'),
                    back = NativeMethods.IsKeyPressed('S'),
                    left = NativeMethods.IsKeyPressed('A'),
                    right = NativeMethods.IsKeyPressed('D'),
                    up = NativeMethods.IsKeyPressed(NativeConstants.VK_SPACE),
                    down = false
                };
                MainCamera.OnKeyMove(KeySpeed, keycontrol);
            }

            if (smgr.BeginFrame(ShellService.Instance.MainWindow.IsActive))
            {
                smgr.DrawAll(!ShellService.Instance.MainWindow.IsBackMode);

                smgr.EndFrame();

                Client.Instance.Tick(smgr.TimeSinceLastFrame);
            }
        }

        public void Shutdown()
        {
            Client.Instance.Dispose();

            Engine.Instance.SaveSetting();

            Engine.Instance.Dispose();
        }

        public void OnResize(uint width, uint height)
        {
            Engine.Instance.Driver.SetDisplayMode(width, height);

            Engine.Instance.SceneManager.OnWindowSizeChanged(width, height);

            Camera cam = Engine.Instance.SceneManager.ActiveCamera;
            if (cam != null)
                cam.RecalculateAll();
        }

        float lastX = 0;
        float lastY = 0;
        bool LCapture = false;
        bool RCapture = false;
        public void OnMouseMessage(MouseMessage message, int x, int y, short zDelta)
        {
            switch (message)
            {
                case MouseMessage.MouseLButtonDown:
                    if (!RCapture)
                    {
                        lastX = x;
                        lastY = y;
                        LCapture = true;
                    }
                    break;
                case MouseMessage.MouseLButtonUp:
                    if (LCapture)
                    {
                        lastX = lastY = 0;
                        LCapture = false;
                    }
                    RCapture = false;
                    break;
                case MouseMessage.MouseRButtonDown:
                    if (!LCapture)
                    {
                        lastX = x;
                        lastY = y;
                        RCapture = true;
                    }
                    break;
                case MouseMessage.MouseRButtonUp:
                    if (RCapture)
                    {
                        lastX = lastY = 0;
                        RCapture = false;
                    }
                    LCapture = false;
                    break;
                case MouseMessage.MouseMove:
                    {
                        if (LCapture || RCapture)
                        {
                            float deltaX = x - lastX;
                            float deltaY = y - lastY;

                            bool bLight = MapEditorService.Instance.IsShow(EditorShowOptions.LightCoord) && NativeMethods.IsKeyPressed('L');

                            if (bLight)
                            {
                                CoordSceneNode lightCoord = MapSceneService.Instance.LightCoordSceneNode;
                                lightCoord.PitchYawFPS(deltaY * 0.2f, deltaX * 0.2f);
                                Engine.Instance.SceneEnvironment.LightDir = lightCoord.GetAxisDir(E_AXIS.ESA_Z);
                            }
                            else
                            {
                                if (IsGameMode)
                                {
                                    Client.Instance.OnMouseMove(deltaY * 0.3f, deltaX * 0.3f);
                                }
                                else
                                {
                                    switch (CameraMode)
                                    {
                                        case CameraModes.Maya:
                                            {
                                                if (LCapture)
                                                    MainCamera.PitchYawMaya(deltaY * 0.3f, deltaX * 0.3f);
                                                else
                                                    MainCamera.MoveOffsetMaya(deltaX * 0.02f, deltaY * 0.02f);
                                            }
                                            break;
                                        case CameraModes.FPS:
                                            {
                                                MainCamera.PitchYawFPS(deltaY * 0.2f, deltaX * 0.2f);
                                            }
                                            break;
                                    }
                                }
                            }

                            lastX += deltaX;
                            lastY += deltaY;
                        }

                    }
                    break;
                case MouseMessage.MouseWheel:
                    {
                        if (ShellService.Instance.MainWindow.IsKeyFocus && WheelSpeed > 0.0f)
                        {
                            if (IsGameMode)
                            {
                                Client.Instance.OnMouseWheel(zDelta);
                            }
                            else
                            {
                                Camera.SKeyControl keycontrol = new Camera.SKeyControl()
                                {
                                    front = zDelta > 0,
                                    back = zDelta < 0,
                                    left = false,
                                    right = false,
                                    up = false,
                                    down = false
                                };
                                MainCamera.OnKeyMove(WheelSpeed * Math.Abs(zDelta), keycontrol);
                            }

                        }
                    }
                    break;
            }
        }

    }
}
