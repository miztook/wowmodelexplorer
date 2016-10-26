using System;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using mywowNet;
using WowModelExplorer;
using WowModelExplorer.Services;
using WowModelExplorer.Controls.Settings;

namespace WowModelExplorer.Commands
{
    public class SettingCommands
    {
        private static RoutedUICommand engineSettingCommand = new RoutedUICommand();
        private static RoutedUICommand cameraPositionCommand = new RoutedUICommand();
        private static RoutedUICommand cameraSpeedCommand = new RoutedUICommand();
        private static RoutedUICommand cameraModeCommand = new RoutedUICommand();
        private static RoutedUICommand wheelSpeedCommand = new RoutedUICommand();
        private static RoutedUICommand gameModeCommand = new RoutedUICommand();

        public static RoutedUICommand EngineSetting { get { return engineSettingCommand; } }
        public static RoutedUICommand CameraPosition { get { return cameraPositionCommand; } }
        public static RoutedUICommand CameraSpeed { get { return cameraSpeedCommand; } }
        public static RoutedUICommand CameraMode { get { return cameraModeCommand; } }
        public static RoutedUICommand WheelSpeed { get { return wheelSpeedCommand; } }
        public static RoutedUICommand GameMode { get { return gameModeCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public SettingCommands()
        {
            commandBindings.Add(new CommandBinding(engineSettingCommand, settingExecuted));
            commandBindings.Add(new CommandBinding(cameraPositionCommand, cameraPositionExecuted, CanEditorModeExecuted));
            commandBindings.Add(new CommandBinding(cameraSpeedCommand, cameraSpeedExecuted, CanEditorModeExecuted));
            commandBindings.Add(new CommandBinding(cameraModeCommand, cameraModeExecuted, CanEditorModeExecuted));
            commandBindings.Add(new CommandBinding(wheelSpeedCommand, wheelSpeedExecuted, CanEditorModeExecuted));
            commandBindings.Add(new CommandBinding(gameModeCommand, gameModeExecuted));
        }

        private void settingExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EngineSettings dialog = new EngineSettings();
            dialog.FillData();
            dialog.ShowDialog();
            dialog.Owner = ShellService.Instance.MainWindow;
        }

        private void cameraPositionExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EditorSetOptions? option = e.Parameter as EditorSetOptions?;
            if (!option.HasValue)
                return;

            switch (option.Value)
            {
                case EditorSetOptions.Default:
                    {
                        Camera cam = EngineService.Instance.MainCamera;
                        if (cam != null)
                        {
                            cam.Position = new vector3df(0, 5, -10);
                            cam.Dir = new vector3df(0, -5, 10);
                            cam.RecalculateAll();
                        }
                    }

                    break;
                case EditorSetOptions.Set:
                    break;
            }
        }

        private void cameraSpeedExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            NumSetOptions? option = e.Parameter as NumSetOptions?;
            if (!option.HasValue)
                return;

            switch (option.Value)
            {
                case NumSetOptions.Medium:
                    EngineService.Instance.KeySpeed = 0.3f;
                    break;
                case NumSetOptions.Low:
                    EngineService.Instance.KeySpeed = 0.1f;
                    break;
                case NumSetOptions.Lowest:
                    EngineService.Instance.KeySpeed = 0.05f;
                    break;
                case NumSetOptions.High:
                    EngineService.Instance.KeySpeed = 0.8f;
                    break;
                case NumSetOptions.Highest:
                    EngineService.Instance.KeySpeed = 2.0f;
                    break;
            }

        }

        private void cameraModeExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            CameraModes? mode = e.Parameter as CameraModes?;
            if (!mode.HasValue)
                return;

            EngineService.Instance.CameraMode = mode.Value;
        }

        private void wheelSpeedExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            NumSetOptions? option = e.Parameter as NumSetOptions?;
            if (!option.HasValue)
                return;

            switch (option.Value)
            {
                case NumSetOptions.Off:
                    EngineService.Instance.WheelSpeed = 0.0f;
                    break;
                case NumSetOptions.Medium:
                    EngineService.Instance.WheelSpeed = 0.5f;
                    break;
                case NumSetOptions.Low:
                    EngineService.Instance.WheelSpeed = 0.25f;
                    break;
                case NumSetOptions.Lowest:
                    EngineService.Instance.WheelSpeed = 0.05f;
                    break;
                case NumSetOptions.High:
                    EngineService.Instance.WheelSpeed = 2.5f;
                    break;
                case NumSetOptions.Highest:
                    EngineService.Instance.WheelSpeed = 5.0f;
                    break;
            }
        }

        private void gameModeExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EngineService.Instance.IsGameMode = !EngineService.Instance.IsGameMode;

            Player player = Client.Instance.GetPlayer();
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (EngineService.Instance.IsGameMode && node != null && (node.Type == M2Type.MT_CHARACTER || node.Type == M2Type.MT_CREATRUE))
            {
                
                player.SetM2AsTarget(node, 15, (float)(Math.PI / 6.0f));
            }
            else
            {
                player.SetM2AsTarget(null, 15, (float)(Math.PI / 6.0f));
            }
        }

        private void CanEditorModeExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = !EngineService.Instance.IsGameMode;
        }

    }
}