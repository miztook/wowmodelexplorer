using System.Windows;
using mywowNet;

namespace WowMapExplorer.Controls.Settings
{
    /// <summary>
    /// Interaction logic for EngineSettings.xaml
    /// </summary>
    public partial class EngineSettings : Window
    {
        public EngineSettings()
        {
            InitializeComponent();
        }

        public void FillData()
        {
            _videoSetting.FillData();
            _advancedSetting.FillData();
        }

        private void btOk_Click(object sender, RoutedEventArgs e)
        {
            _videoSetting.Apply();
            _advancedSetting.Apply();

            this.Close();
        }

    }
}
