using System;
using System.Windows;
using System.Windows.Controls;
using WowTexViewer.Services;
using AvalonDock;
using mywowNet;

namespace WowTexViewer.Menus
{
    /// <summary>
    /// Interaction logic for MainMenu.xaml
    /// </summary>
    public partial class MainMenu : UserControl
    {
        public MainMenu()
        {
            InitializeComponent();
        }

        private void menuLanguages_SubmenuOpend(object sender, RoutedEventArgs e)
        {
            App app = Application.Current as App;
            menuLanguagesChinese.IsChecked = app.Language == Languages.Chinese;
            menuLanguagesEnglish.IsChecked = app.Language == Languages.English;
        }

        private void menuView_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuViewAllTexture.IsChecked = ShellService.Instance.AllTextureContent.State != DockableContentState.Hidden;
            menuViewMapTexture.IsChecked = ShellService.Instance.MapTexContent.State != DockableContentState.Hidden;
            menuViewMapTextureName.IsChecked = ShellService.Instance.MapTexNameContent.State != DockableContentState.Hidden;
        }

        private void menuEditorOption_SubmenuOpened(object sender, RoutedEventArgs e)
        {

        }

        private void menuTexture_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuTextureOpaque.IsChecked = Engine.Instance.SceneManager.TextureOpaque;
        }

        private void menuOptionShowHide_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            menuOptionShowBaseInfo.IsChecked = ModelEditorService.Instance.IsShow(EditorShowOptions.DebugText);
            menuOptionShowPlane.IsChecked = ModelEditorService.Instance.IsShow(EditorShowOptions.Plane);
         
        }
    }
}
