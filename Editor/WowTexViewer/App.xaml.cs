using System;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;
using System.Windows.Media;
using WowTexViewer.Properties;
using WowTexViewer.Services;
using mywowNet;

namespace WowTexViewer
{
    public delegate void Application_LanguageChangedHandler(object sender, Languages current);

    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        //language
        private Languages language = Languages.Chinese;
        private ResourceDictionary loadedLanguageResourceDictionary;

        private MainWindow window;

        private Collection<ResourceDictionary> MergedDictionaries
        {
            get { return Resources.MergedDictionaries; }
        }

        public event Application_LanguageChangedHandler ApplicationLanguageChanged;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            foreach (ResourceDictionary item in MergedDictionaries)
            {
                if (item.Source != null && item.Source.OriginalString.Contains(@"Language\"))
                    loadedLanguageResourceDictionary = item;
            }

            string lan = Settings.Default.Language;
            switch (lan)
            {
                case "Chinese":
                    {
                        ApplyLanguage(Languages.Chinese);
                        Language = Languages.Chinese;
                    }
                    break;
                case "English":
                    {
                        ApplyLanguage(Languages.English);
                        Language = Languages.English;
                    }
                    break;
            }

            window = new MainWindow();

            // Restore the window size when the values are valid.
            if (Settings.Default.Left >= 0 && Settings.Default.Top >= 0 && Settings.Default.Width > 0 && Settings.Default.Height > 0
                && Settings.Default.Left + Settings.Default.Width <= SystemParameters.VirtualScreenWidth
                && Settings.Default.Top + Settings.Default.Height <= SystemParameters.VirtualScreenHeight)
            {
                window.Left = Settings.Default.Left;
                window.Top = Settings.Default.Top;
                window.Height = Settings.Default.Height;
                window.Width = Settings.Default.Width;
            }
            window.IsMaximized = Settings.Default.IsMaximized;
            Resources["EdgeBorderBrush"] = new SolidColorBrush(Settings.Default.EdgeBorderColor);

            //init
            new EngineService();
            new ShellService(window);
            new ModelSceneService();
            new ModelEditorService();

            //run
            window.Show();

        }

        protected override void OnExit(ExitEventArgs e)
        {
            Settings.Default.Left = window.Left;
            Settings.Default.Top = window.Top;
            Settings.Default.Height = window.Height;
            Settings.Default.Width = window.Width;
            Settings.Default.IsMaximized = window.IsMaximized;
            SolidColorBrush brush = Resources["EdgeBorderBrush"] as SolidColorBrush;
            Settings.Default.EdgeBorderColor = brush.Color;

            switch (Language)
            {
                case Languages.Chinese:
                    Settings.Default.Language = "Chinese";
                    break;
                case Languages.English:
                    Settings.Default.Language = "English";
                    break;
            }

            try
            {
                Settings.Default.Save();
            }
            catch (Exception)
            {
                // When more application instances are closed at the same time then an exception occurs.
            }

            base.OnExit(e);
        }

        public void ApplyLanguage(Languages language)
        {
            Uri objLanguageDictionaryUri;
            switch (language)
            {
                case Languages.Chinese:
                    objLanguageDictionaryUri = new Uri("Resources/Languages/Chinese.xaml", UriKind.Relative);
                    break;
                case Languages.English:
                    objLanguageDictionaryUri = new Uri("Resources/Languages/English.xaml", UriKind.Relative);
                    break;
                default:
                    objLanguageDictionaryUri = new Uri("Resources/Languages/English.xaml", UriKind.Relative);
                    break;
            }

            if (string.IsNullOrEmpty(objLanguageDictionaryUri.OriginalString) == false)
            {
                ResourceDictionary objNewLanguageDictionary =
                    (ResourceDictionary)(LoadComponent(objLanguageDictionaryUri));

                if (objNewLanguageDictionary != null)
                {
                    MergedDictionaries.Remove(loadedLanguageResourceDictionary);
                    loadedLanguageResourceDictionary = objNewLanguageDictionary;
                    MergedDictionaries.Add(objNewLanguageDictionary);
                }
            }
        }

        public Languages Language
        {
            get { return language; }
            set
            {
                if (language != value)
                {
                    language = value;
                    if (ApplicationLanguageChanged != null)
                    {
                        ApplicationLanguageChanged.Invoke(this, language);
                    }
                }
            }
        }

    }
}
