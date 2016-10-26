using System;
using WowTexViewer;
using WowTexViewer.Commands;
using WowTexViewer.Menus;
using WowTexViewer.Views;

namespace WowTexViewer.Services
{
    public class ShellService
    {
        private readonly MainWindow mainWindow;

        private readonly FileCommands fileCommands = new FileCommands();
        private readonly ViewCommands viewCommands = new ViewCommands();
        private readonly TextureCommands textureCommands = new TextureCommands();
        private readonly OptionCommands optionCommands = new OptionCommands();
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

        public MainMenu MainMenu { get { return mainWindow._modelMenu; } }
        public AllTextureContent AllTextureContent { get { return mainWindow._allTextureContent; } }
        public MapTexContent MapTexContent { get { return mainWindow._mapTexContent; } }
        public MapTexNameContent MapTexNameContent { get { return mainWindow._mapTexNameContent; } }

        //Commands
        public FileCommands FileCommands { get { return fileCommands; } }
        public ViewCommands ViewCommands { get { return viewCommands; } }
        public TextureCommands TextureCommands { get { return textureCommands; } }
        public OptionCommands OptionCommandds { get { return optionCommands; } }
        public LanguageCommands LuaguageCommands { get { return languageCommands; } }
    }
}
