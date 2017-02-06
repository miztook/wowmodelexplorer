using System.Windows;
using System.Windows.Media;

namespace WowModelExplorer.Controls.Splash
{
    public partial class SplashWindow : Window
    {
        public SplashWindow()
        {
            InitializeComponent();

            this.Loaded += new RoutedEventHandler(SplashWindow_Loaded);
        }

        void SplashWindow_Loaded(object sender, RoutedEventArgs e)
        {
            _backGroundImage.ImageSource = Application.Current.Resources["WallPaper60"] as ImageSource;
        }
    }
}
