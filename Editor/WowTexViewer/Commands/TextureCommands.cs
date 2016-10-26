using System;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using mywowNet;
using WowTexViewer;
using WowTexViewer.Services;
using WowTexViewer.Views;
using WowTexViewer.Utility;
using Microsoft.Win32;

namespace WowTexViewer.Commands
{
    public class TextureCommands
    {
        private static RoutedUICommand textureOpaqueCommand = new RoutedUICommand();
        private static RoutedUICommand exportAsTgaCommand = new RoutedUICommand();
        private static RoutedUICommand exportAsTgaNoAlphaCommand = new RoutedUICommand();

        public static RoutedUICommand TextureOpaque { get { return textureOpaqueCommand; } }
        public static RoutedUICommand ExportAsTga { get { return exportAsTgaCommand; } }
        public static RoutedUICommand ExportAsTgaNoAlpha { get { return exportAsTgaNoAlphaCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public TextureCommands()
        {
            commandBindings.Add(new CommandBinding(textureOpaqueCommand, textureOpaqueExecuted));
            commandBindings.Add(new CommandBinding(exportAsTgaCommand, exportAsTgaExecuted, CanTextureExecuted));
            commandBindings.Add(new CommandBinding(exportAsTgaNoAlphaCommand, exportAsTgaNoAlphaExecuted, CanTextureExecuted));
        }

        private void textureOpaqueExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Engine.Instance.SceneManager.TextureOpaque = !Engine.Instance.SceneManager.TextureOpaque;
        }

        private void exportAsTgaExecuted(object sender, ExecutedRoutedEventArgs e)
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
            bool success = mwTool.Instance.ExportBlpAsTgaDir(
                ModelSceneService.Instance.CurrentTextureName,
                dirPath,
                true);

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

        private void exportAsTgaNoAlphaExecuted(object sender, ExecutedRoutedEventArgs e)
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
            bool success = mwTool.Instance.ExportBlpAsTgaDir(
                ModelSceneService.Instance.CurrentTextureName,
                dirPath,
                false);

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

        private void CanTextureExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = ModelSceneService.Instance.CurrentTextureName.Length > 0;
        }
    }
}
