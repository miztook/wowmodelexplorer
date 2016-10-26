using System;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using mywowNet;
using WowTexViewer;
using WowTexViewer.Services;
using WowTexViewer.Views;

namespace WowTexViewer.Commands
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
            ModelViewTypes? type = e.Parameter as ModelViewTypes?;
            DockableContent content = null;

            switch (type.Value)
            {
                case ModelViewTypes.AllTexture:
                    content = ShellService.Instance.AllTextureContent;
                    break;
                case ModelViewTypes.MapTexture:
                    content = ShellService.Instance.MapTexContent;
                    break;
                case ModelViewTypes.MapTextureName:
                    content = ShellService.Instance.MapTexNameContent;
                    break;
            }

            if (content != null)
            {
                if (content.State != DockableContentState.Hidden)
                    content.Hide();
                else
                    content.Show();
            }
        }
    }
}
