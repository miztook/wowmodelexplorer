using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using WowMapExplorer.Services;
using AvalonDock;

namespace WowMapExplorer.Menus
{
    /// <summary>
    /// Interaction logic for MapMenu.xaml
    /// </summary>
    public partial class MapMenu : UserControl
    {
        public MapMenu()
        {
            InitializeComponent();
        }

        private void menuEngineOption_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuSettingGameMode.IsChecked = EngineService.Instance.IsGameMode;
        }

        private void menuOptionShowHide_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuOptionShowBaseInfo.IsChecked = MapEditorService.Instance.IsShow(EditorShowOptions.DebugText);
            menuOptionShowCoord.IsChecked = MapEditorService.Instance.IsShow(EditorShowOptions.Coord);
        }

        private void menuSettingCameraSpeed_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuSettingCameraSpeedLowest.IsChecked = EngineService.Instance.KeySpeed == 0.05f;
            menuSettingCameraSpeedLow.IsChecked = EngineService.Instance.KeySpeed == 0.1f;
            menuSettingCameraSpeedMedium.IsChecked = EngineService.Instance.KeySpeed == 0.3f;
            menuSettingCameraSpeedHigh.IsChecked = EngineService.Instance.KeySpeed == 0.8f;
            menuSettingCameraSpeedHighest.IsChecked = EngineService.Instance.KeySpeed == 2.0f;
        }

        private void menuSettingCameraModes_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuSettingCameraModesFPS.IsChecked = EngineService.Instance.CameraMode == CameraModes.FPS;
            menuSettingCameraModesMaya.IsChecked = EngineService.Instance.CameraMode == CameraModes.Maya;
        }

        private void menuSettingWheelSpeed_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuSettingWheelSpeedOff.IsChecked = EngineService.Instance.WheelSpeed == 0.0f;
            menuSettingWheelSpeedLowest.IsChecked = EngineService.Instance.WheelSpeed == 0.01f;
            menuSettingWheelSpeedLow.IsChecked = EngineService.Instance.WheelSpeed == 0.05f;
            menuSettingWheelSpeedMedium.IsChecked = EngineService.Instance.WheelSpeed == 0.1f;
            menuSettingWheelSpeedHigh.IsChecked = EngineService.Instance.WheelSpeed == 0.5f;
            menuSettingWheelSpeedHighest.IsChecked = EngineService.Instance.WheelSpeed == 1.0f;
        }

        private void menuLanguages_SubmenuOpend(object sender, RoutedEventArgs e)
        {
            App app = Application.Current as App;
            menuLanguagesChinese.IsChecked = app.Language == Languages.Chinese;
            menuLanguagesEnglish.IsChecked = app.Language == Languages.English;
        }

        private void menuView_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuViewMap.IsChecked = ShellService.Instance.MapContent.State != DockableContentState.Hidden;        
        }

    }
}
