using System;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using WowTexViewer.Services;

namespace WowTexViewer.Commands
{
    public class FileCommands
    {
        private static RoutedUICommand newFileCommand = new RoutedUICommand();
        private static RoutedUICommand exitCommand = new RoutedUICommand();

        public static RoutedUICommand NewFile { get { return newFileCommand; } }
        public static RoutedUICommand Exit { get { return exitCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public FileCommands()
        {
            commandBindings.Add(new CommandBinding(newFileCommand, newFileExecuted));
            commandBindings.Add(new CommandBinding(exitCommand, exitExecuted));
        }

        private void newFileExecuted(object sender, ExecutedRoutedEventArgs e)
        {
        }

        private void exitExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            ShellService.Instance.MainWindow.Close();
        }
    }
}
