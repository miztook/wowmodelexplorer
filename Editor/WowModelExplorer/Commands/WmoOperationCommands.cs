using System;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using mywowNet;
using System.Windows.Media;
using WowModelExplorer;
using WowModelExplorer.Services;
using WowModelExplorer.Views;
using WowModelExplorer.Utility;
using WowModelExplorer.Controls;
using System.Windows;

namespace WowModelExplorer.Commands
{
    public class WmoOperationCommands
    {
        private static RoutedUICommand groupsWindowCommand = new RoutedUICommand();
        private static RoutedUICommand portalsWindowCommand = new RoutedUICommand();
        private static RoutedUICommand exportObjCommand = new RoutedUICommand();
        private static RoutedUICommand exportFbxCommand = new RoutedUICommand();

        public static RoutedUICommand GroupsWindow { get { return groupsWindowCommand; } }
        public static RoutedUICommand PortalsWindow { get { return portalsWindowCommand; } }
        public static RoutedUICommand ExportObj { get { return exportObjCommand; } }
        public static RoutedUICommand ExportFbx { get { return exportFbxCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public WmoOperationCommands()
        {
            commandBindings.Add(new CommandBinding(groupsWindowCommand, GroupsWindowExecuted));
            commandBindings.Add(new CommandBinding(portalsWindowCommand, PortalsWindowExecuted));
            commandBindings.Add(new CommandBinding(exportObjCommand, ExportObjExecuted, CanWmoExecuted));
            commandBindings.Add(new CommandBinding(exportFbxCommand, ExportFbxExecuted, CanWmoExecuted));
        }

        private void GroupsWindowExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            WMOGroupsWindow window = ShellService.Instance.GroupsWindow;
            if (window.IsVisible)
                window.Hide();
            else
            {
                window.Show();
                window.Update();
            }
        }

        private void PortalsWindowExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            WMOPortalsWindow window = ShellService.Instance.PortalsWindow;
            if (window.IsVisible)
                window.Hide();
            else
            {
                window.Show();
                window.Update();
            }
        }

        private void CanWmoExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
            e.CanExecute = false;
            if (node != null)
            {
//                 string strLocale = Engine.Instance.WowEnvironment.GetLocale();
//                 if (strLocale == "" || strLocale.Equals("zhCN", StringComparison.CurrentCultureIgnoreCase))
//                 {
//                     string fullname = node.FileWMO.longname;
//                     if (fullname.IndexOf("world/wmo/blacktemple", StringComparison.CurrentCultureIgnoreCase) != -1 ||
//                         fullname.IndexOf("world/wmo/darkmoonfaire", StringComparison.CurrentCultureIgnoreCase) != -1)
//                         e.CanExecute = true;
//                 }
//                 else 
                {
                    e.CanExecute = true;
                }
            }
        }

        private void ExportObjExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            FolderBrowserDialog.FolderBrowserDialog dialog = new FolderBrowserDialog.FolderBrowserDialog
            {
                ShowEditBox = true,
                BrowseShares = true
            };

            dialog.RootType = FolderBrowserDialog.RootType.Path;
            //dialog.RootPath = Engine.Instance.GetBaseDirectory();

            if (dialog.ShowDialog() != true)
                return;

            string dirPath = dialog.SelectedPath;

            WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;

            bool success = mwTool.Instance.ExportWMOSceneNodeToOBJ(node, dirPath);  

            if (success)
            {
                NativeMethods.ShellExecute(
                    IntPtr.Zero,
                    "open",
                    "Explorer.exe",
                    dirPath,
                    "",
                    NativeMethods.ShowCommands.SW_NORMAL);
            }
        }

        private void ExportFbxExecuted(object sender, ExecutedRoutedEventArgs e)
        {
          
        }
    }
}
