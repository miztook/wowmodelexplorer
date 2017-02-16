using System.Windows;
using System.Windows.Media;

namespace WowTexViewer.Controls.Splash
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
#if WOW50
            _backGroundImage.ImageSource = Application.Current.Resources["WallPaper50"] as ImageSource;
#elif WOW60
            _backGroundImage.ImageSource = Application.Current.Resources["WallPaper60"] as ImageSource;
#else
            _backGroundImage.ImageSource = Application.Current.Resources["WallPaper70"] as ImageSource;
#endif
        }
    }
}
