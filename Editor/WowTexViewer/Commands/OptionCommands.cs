using System;
using System.Collections.Generic;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Threading;
using AvalonDock;
using mywowNet;
using WowTexViewer;
using WowTexViewer.Services;
using WowTexViewer.Views;
using WowTexViewer.Controls;
using WowTexViewer.Utility;
using System.Windows;

namespace WowTexViewer.Commands
{
    public class OptionCommands
    {
        private static RoutedUICommand backgroundColorCommand = new RoutedUICommand();
        private static RoutedUICommand edgeBorderColorCommand = new RoutedUICommand();
        private static RoutedUICommand editorFontCommand = new RoutedUICommand();
        private static RoutedUICommand editorFontColorCommand = new RoutedUICommand();
        private static RoutedUICommand showOptionCommand = new RoutedUICommand();

        public static RoutedUICommand BackgroundColor { get { return backgroundColorCommand; } }
        public static RoutedUICommand EdgeBorderColor { get { return edgeBorderColorCommand; } }
        public static RoutedUICommand EditorFont { get { return editorFontCommand; } }
        public static RoutedUICommand EditorFontColor { get { return editorFontColorCommand; } }
        public static RoutedUICommand ShowOption { get { return showOptionCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public OptionCommands()
        {
            commandBindings.Add(new CommandBinding(backgroundColorCommand, BackgroundColorExecuted));
            commandBindings.Add(new CommandBinding(edgeBorderColorCommand, EdgeBorderColorExecuted));
            commandBindings.Add(new CommandBinding(editorFontCommand, EditorFontExecuted));
            commandBindings.Add(new CommandBinding(editorFontColorCommand, EditorFontColorExecuted));
            commandBindings.Add(new CommandBinding(showOptionCommand, ShowOptionExecuted));
        }

        private void BackgroundColorExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EditorSetOptions? option = e.Parameter as EditorSetOptions?;
            if (!option.HasValue)
                return;

            switch (option.Value)
            {
                case EditorSetOptions.Default:
                    Engine.Instance.SceneManager.BackgroundColor = SceneManager.DefaultBackgroundColor;
                    break;
                case EditorSetOptions.Set:
                    {
                        ColorPickerControls.Dialogs.ColorPickerStandardDialog dialog = new ColorPickerControls.Dialogs.ColorPickerStandardDialog();
                        dialog.colorPickerFull.SelectedColorChanged += (s, param) =>
                        {
                            Engine.Instance.SceneManager.BackgroundColor = param.Value.ToSColor();
                        };
                        SColor scolor = Engine.Instance.SceneManager.BackgroundColor;
                        dialog.InitialColor = Color.FromArgb(scolor.A, scolor.R, scolor.G, scolor.B);
                        dialog.Owner = ShellService.Instance.MainWindow;
                        bool? dialogResult = dialog.ShowDialog();
                        if (dialogResult != null && (bool)dialogResult == true)
                        {
                            Engine.Instance.SceneManager.BackgroundColor = dialog.SelectedColor.ToSColor();
                        }
                        else
                        {
                            Engine.Instance.SceneManager.BackgroundColor = dialog.InitialColor.ToSColor();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        private void EdgeBorderColorExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EditorSetOptions? option = e.Parameter as EditorSetOptions?;
            if (!option.HasValue)
                return;

            switch (option.Value)
            {
                case EditorSetOptions.Default:
                    Application.Current.Resources["EdgeBorderBrush"] = new SolidColorBrush(Colors.Gray);
                    break;
                case EditorSetOptions.Set:
                    {
                        ColorPickerControls.Dialogs.ColorPickerStandardDialog dialog = new ColorPickerControls.Dialogs.ColorPickerStandardDialog();
                        SolidColorBrush brush = Application.Current.Resources["EdgeBorderBrush"] as SolidColorBrush;
                        dialog.InitialColor = brush.Color;
                        dialog.Owner = ShellService.Instance.MainWindow;
                        bool? dialogResult = dialog.ShowDialog();
                        if (dialogResult != null && (bool)dialogResult == true)
                        {
                            Application.Current.Resources["EdgeBorderBrush"] = new SolidColorBrush(dialog.SelectedColor);
                        }
                    }
                    break;
            }
        }

        private void EditorFontExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EditorSetOptions? option = e.Parameter as EditorSetOptions?;
            if (!option.HasValue)
                return;

            switch (option.Value)
            {
                case EditorSetOptions.Default:
                    Application.Current.Resources["WindowFontFamily"] = new FontFamily();
                    Application.Current.Resources["WindowFontStyle"] = new FontStyle();
                    Application.Current.Resources["WindowFontWeight"] = new FontWeight();
                    break;
                case EditorSetOptions.Set:
                    {
                        FontDialog.FontDialogBox dialog = new FontDialog.FontDialogBox();

                        FontFamily family = Application.Current.Resources["WindowFontFamily"] as FontFamily;
                        FontStyle? style = Application.Current.Resources["WindowFontStyle"] as FontStyle?;
                        FontWeight? weight = Application.Current.Resources["WindowFontWeight"] as FontWeight?;

                        dialog.FontFamily = family;
                        if (style.HasValue)
                            dialog.FontStyle = style.Value;
                        if (weight.HasValue)
                            dialog.FontWeight = weight.Value;
                        dialog.Owner = ShellService.Instance.MainWindow;
                        bool? dialogResult = dialog.ShowDialog();
                        if (dialogResult != null && (bool)dialogResult == true)
                        {
                            Application.Current.Resources["WindowFontFamily"] = dialog.FontFamily;
                            Application.Current.Resources["WindowFontStyle"] = dialog.FontStyle;
                            Application.Current.Resources["WindowFontWeight"] = dialog.FontWeight;
                        }
                    }
                    break;
            }
        }

        private void EditorFontColorExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EditorSetOptions? option = e.Parameter as EditorSetOptions?;
            if (!option.HasValue)
                return;

            switch (option.Value)
            {
                case EditorSetOptions.Default:
                    Application.Current.Resources["TextBrush"] = new SolidColorBrush(Colors.White);
                    break;
                case EditorSetOptions.Set:
                    {
                        ColorPickerControls.Dialogs.ColorPickerStandardDialog dialog = new ColorPickerControls.Dialogs.ColorPickerStandardDialog();
                        SolidColorBrush brush = Application.Current.Resources["TextBrush"] as SolidColorBrush;
                        dialog.InitialColor = brush.Color;
                        dialog.Owner = ShellService.Instance.MainWindow;
                        bool? dialogResult = dialog.ShowDialog();
                        if (dialogResult != null && (bool)dialogResult == true)
                        {
                            Application.Current.Resources["TextBrush"] = new SolidColorBrush(dialog.SelectedColor);
                        }
                    }
                    break;
            }
        }

        private void ShowOptionExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EditorShowOptions? option = e.Parameter as EditorShowOptions?;
            if (!option.HasValue)
                return;

            bool show = ModelEditorService.Instance.IsShow(option.Value);
            switch (option.Value)
            {
                case EditorShowOptions.DebugText:
                    Engine.Instance.SceneManager.ShowDebug(E_SCENE_DEBUG_PART.ESDP_BASE, !show);
                    break;
                case EditorShowOptions.Plane:
                    ModelSceneService.Instance.GridSceneNode.Visible = !show;
                    break;
            }
            ModelEditorService.Instance.SetShow(option.Value, !show);
        }
    }
}
