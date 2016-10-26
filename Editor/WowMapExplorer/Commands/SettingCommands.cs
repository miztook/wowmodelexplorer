using System;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using mywowNet;
using WowMapExplorer;
using WowMapExplorer.Services;
using WowMapExplorer.Controls.Settings;

namespace WowMapExplorer.Commands
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
                    EngineService.Instance.WheelSpeed = 0.01f;
                    break;
                case NumSetOptions.Low:
                    EngineService.Instance.WheelSpeed = 0.005f;
                    break;
                case NumSetOptions.Lowest:
                    EngineService.Instance.WheelSpeed = 0.001f;
                    break;
                case NumSetOptions.High:
                    EngineService.Instance.WheelSpeed = 0.05f;
                    break;
                case NumSetOptions.Highest:
                    EngineService.Instance.WheelSpeed = 0.1f;
                    break;
            }
        }

        private void gameModeExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EngineService.Instance.IsGameMode = !EngineService.Instance.IsGameMode;
        }

        private void CanEditorModeExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = !EngineService.Instance.IsGameMode;
        }
    
    }
}