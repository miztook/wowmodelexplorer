using System;
using System.Windows;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using mywowNet;
using WowModelExplorer;
using WowModelExplorer.Services;
using WowModelExplorer.Views;

namespace WowModelExplorer.Commands
{
    public class LanguageCommands
    {
        private static RoutedUICommand setLanguageCommand = new RoutedUICommand();

        public static RoutedUICommand SetLanguage { get { return setLanguageCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public LanguageCommands()
        {
            commandBindings.Add(new CommandBinding(setLanguageCommand, setLanguageExecuted));
        }

        private void setLanguageExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            App app = Application.Current as App;
            Languages? lan = e.Parameter as Languages?;
            if (lan.HasValue)
            {
                if (app.Language != lan.Value)
                {
                    app.ApplyLanguage(lan.Value);
                    app.Language = lan.Value;
                }
            }
        }
    }
}
