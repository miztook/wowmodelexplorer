using System;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using mywowNet;
using WowModelExplorer;
using WowModelExplorer.Services;
using WowModelExplorer.Views;

namespace WowModelExplorer.Commands
{
    public class FileCommands
    {
        private static RoutedUICommand newFileCommand = new RoutedUICommand();
        private static RoutedUICommand openFileCommand = new RoutedUICommand();
        private static RoutedUICommand exitCommand = new RoutedUICommand();

        public static RoutedUICommand NewFile { get { return newFileCommand; } }
        public static RoutedUICommand OpenFile { get { return openFileCommand; } }
        public static RoutedUICommand Exit { get { return exitCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public FileCommands()
        {
            commandBindings.Add(new CommandBinding(newFileCommand, newFileExecuted));
            commandBindings.Add(new CommandBinding(openFileCommand, openFileExecuted));
            commandBindings.Add(new CommandBinding(exitCommand, exitExecuted));
        }

        private void newFileExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            ModelSceneService.Instance.SetMainM2SceneNode("", false);
            ModelSceneService.Instance.SetMainWMOSceneNode("");
        }

        private void openFileExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            OpenTypes? type = e.Parameter as OpenTypes?;
            switch(type.Value)
            {
                case OpenTypes.LoginScreen:
                    {
                        M2SceneNode node = ModelSceneService.Instance.SetMainM2SceneNode(@"interface\Glues\Models\UI_MainMenu_Cataclysm\UI_MainMenu_CataClysm.m2", false);
                        node.BuildVisibleGeosets();
                        ModelEditorService.Instance.SetWireFrame(node);

                        ShellService.Instance.PropertyContent.UpdateNormalM2Node(node);
                    }
                    break;
            }
        }

        private void exitExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            ShellService.Instance.MainWindow.Close();
        }

    }
}
