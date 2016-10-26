using System;
using WowMapExplorer;
using WowMapExplorer.Commands;
using WowMapExplorer.Menus;
using WowMapExplorer.Controls;
using WowMapExplorer.Views;

namespace WowMapExplorer.Services
{
    public class ShellService
    {
        private readonly MainWindow mainWindow;

        private readonly FileCommands fileCommands = new FileCommands();
        private readonly ViewCommands viewCommands = new ViewCommands();
        private readonly OptionCommands optionCommands = new OptionCommands();
        private readonly SettingCommands settingCommands = new SettingCommands();
        private readonly LanguageCommands languageCommands = new LanguageCommands();

        public ShellService(MainWindow window)
        {
            if (instance == null)
            {
                instance = this;

                mainWindow = window;
            }
        }
        private static ShellService instance = null;
        public static ShellService Instance { get { return instance; } }

        public MainWindow MainWindow { get { return mainWindow; } }
        public MapMenu MapMenu { get { return mainWindow._mapMenu; } }
        public MapContent MapContent { get { return mainWindow._mapContent; } }
        //model mode views

        //Commands
        public FileCommands FileCommands { get { return fileCommands; } }
        public ViewCommands ViewCommands { get { return viewCommands; } }
        public OptionCommands OptionCommands { get { return optionCommands; } }
        public SettingCommands SettingCommands { get { return settingCommands; } }
        public LanguageCommands LanguageCommands { get { return languageCommands; } }
    }
}
