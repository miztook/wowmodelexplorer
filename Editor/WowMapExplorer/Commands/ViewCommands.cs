using System;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using mywowNet;
using WowMapExplorer;
using WowMapExplorer.Services;

namespace WowMapExplorer.Commands
{
    public class ViewCommands
    {
        private static RoutedUICommand viewCommand = new RoutedUICommand();
        public static RoutedUICommand View { get { return viewCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public ViewCommands()
        {
            commandBindings.Add(new CommandBinding(viewCommand, viewExecuted));
        }

        private void viewExecuted(object sender, ExecutedRoutedEventArgs e)
        {

        }

    }
}
