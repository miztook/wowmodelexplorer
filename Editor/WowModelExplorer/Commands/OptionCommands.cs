using System;
using System.Collections.Generic;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Threading;
using AvalonDock;
using mywowNet;
using WowModelExplorer;
using WowModelExplorer.Services;
using WowModelExplorer.Views;
using WowModelExplorer.Utility;
using WowModelExplorer.Controls;
using System.Windows;
using Microsoft.Win32;

namespace WowModelExplorer.Commands
{
    public class OptionCommands
    {
        private static RoutedUICommand settingsCommand = new RoutedUICommand();
        private static RoutedUICommand backgroundImageCommand = new RoutedUICommand();
        private static RoutedUICommand backgroundColorCommand = new RoutedUICommand();
        private static RoutedUICommand edgeBorderColorCommand = new RoutedUICommand();
        private static RoutedUICommand editorFontCommand = new RoutedUICommand();
        private static RoutedUICommand editorFontColorCommand = new RoutedUICommand();
        private static RoutedUICommand showOptionCommand = new RoutedUICommand();
        private static RoutedUICommand backMusicCommand = new RoutedUICommand();
        private static RoutedUICommand accelerateCommand = new RoutedUICommand();

        public static RoutedUICommand Settings { get { return settingsCommand; } }
        public static RoutedUICommand BackgroundImage { get { return backgroundImageCommand; } }
        public static RoutedUICommand BackgroundColor { get { return backgroundColorCommand; } }
        public static RoutedUICommand EdgeBorderColor { get { return edgeBorderColorCommand; } }
        public static RoutedUICommand EditorFont { get { return editorFontCommand; } }
        public static RoutedUICommand EditorFontColor { get { return editorFontColorCommand; } }
        public static RoutedUICommand ShowOption { get { return showOptionCommand; } }
        public static RoutedUICommand BackMusic { get { return backMusicCommand; } }
        public static RoutedUICommand Accelerate { get { return accelerateCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get{ return commandBindings; } }

        public OptionCommands()
        {
            commandBindings.Add(new CommandBinding(settingsCommand, EditorSettingsExecuted));
            commandBindings.Add(new CommandBinding(backgroundImageCommand, BackgroundImageExecuted));
            commandBindings.Add(new CommandBinding(backgroundColorCommand, BackgroundColorExecuted));
            commandBindings.Add(new CommandBinding(edgeBorderColorCommand, EdgeBorderColorExecuted));
            commandBindings.Add(new CommandBinding(editorFontCommand, EditorFontExecuted));
            commandBindings.Add(new CommandBinding(editorFontColorCommand, EditorFontColorExecuted));
            commandBindings.Add(new CommandBinding(showOptionCommand, ShowOptionExecuted));
            commandBindings.Add(new CommandBinding(backMusicCommand, BackMusicExecuted));
            commandBindings.Add(new CommandBinding(accelerateCommand, AccelerateExecuted));
        }

        private void EditorSettingsExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EditorSettings? setting = e.Parameter as EditorSettings?;
            if (!setting.HasValue)
                return;

            switch(setting.Value)
            {
                case EditorSettings.LineZTest:
                {
                    Engine.Instance.DrawServices.LineZTestEnable = !Engine.Instance.DrawServices.LineZTestEnable;
                }
                break;
            }
        }

        private void BackgroundImageExecuted(object sender, ExecutedRoutedEventArgs e)
        {
             EditorSetOptions? option = e.Parameter as EditorSetOptions?;
            if (!option.HasValue)
                return;

            switch (option.Value)
            {
                case EditorSetOptions.None:
                    ModelEditorService.Instance.BackImageFileName = "";
                    break;
                case EditorSetOptions.Set:
                    {
                        OpenFileDialog dlg = new OpenFileDialog();
                        dlg.Filter = "jpg|*.jpg|jpeg|*.jpeg|png|*.png";
                        dlg.InitialDirectory = Engine.Instance.GetBaseDirectory();
                        bool? ret = dlg.ShowDialog(ShellService.Instance.MainWindow);
                        if (ret.HasValue && ret.Value)
                        {
                            ModelEditorService.Instance.BackImageFileName = dlg.FileName;
                        }
                    }
                    break;
                default:
                    break;
            }
            Engine.Instance.SceneManager.BackImageFileName = ModelEditorService.Instance.BackImageFileName;
        }

        private void BackgroundColorExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EditorSetOptions? option = e.Parameter as EditorSetOptions?;
            if (!option.HasValue)
                return;

            switch(option.Value)
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
                case EditorShowOptions.DebugGeosets:
                    Engine.Instance.SceneManager.ShowDebug(E_SCENE_DEBUG_PART.ESDP_M2_GEOSETS, !show);
                    break;
                case EditorShowOptions.Plane:
                    ModelSceneService.Instance.GridSceneNode.Visible = !show;
                    break;
                case EditorShowOptions.Coord:
                    ModelSceneService.Instance.CoordSceneNode.Visible = !show;
                    break;
                case EditorShowOptions.LightCoord:
                    ModelSceneService.Instance.LightCoordSceneNode.Visible = !show;
                    break;
            }
            ModelEditorService.Instance.SetShow(option.Value, !show);
        }

        private void AccelerateExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            AccelerateWindow window = ShellService.Instance.AccelerateWindow;
            if (window.IsVisible)
                window.Hide();
            else
            {
                window.Show();
            }
        }

        private void BackMusicExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            BackMusicFiles? file = e.Parameter as BackMusicFiles?;
            if (!file.HasValue)
                return;

            switch(file.Value)
            {
                case BackMusicFiles.None:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if( index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }
                        ModelEditorService.Instance.BackMusic = BackMusicFiles.None;
                    }
                    break;
                case BackMusicFiles.MainTitle:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        uint newIndex;
                        if (Engine.Instance.AudioPlayer.LoadMp3(@"Sound\music\GlueScreenMusic\wow_main_theme.mp3", out newIndex))
                        {
                            Engine.Instance.AudioPlayer.PlayMp3(newIndex, true);
                            ModelEditorService.Instance.MusicIndex = (int)newIndex;
                            ModelEditorService.Instance.BackMusic = BackMusicFiles.MainTitle;
                        }  
                    }
                    break;
                case BackMusicFiles.WotLK:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        uint newIndex;
                        if (Engine.Instance.AudioPlayer.LoadMp3(@"Sound\music\GlueScreenMusic\WotLK_main_title.mp3", out newIndex))
                        {
                            Engine.Instance.AudioPlayer.PlayMp3(newIndex, true);
                            ModelEditorService.Instance.MusicIndex = (int)newIndex;
                            ModelEditorService.Instance.BackMusic = BackMusicFiles.WotLK;
                        }                      
                    }
                    break;
                case BackMusicFiles.Cataclysm:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        uint newIndex;
                        if (Engine.Instance.AudioPlayer.LoadMp3(@"Sound\music\cataclysm\MUS_Shattering_UU01.mp3", out newIndex))
                        {
                            Engine.Instance.AudioPlayer.PlayMp3(newIndex, true);
                            ModelEditorService.Instance.MusicIndex = (int)newIndex;
                            ModelEditorService.Instance.BackMusic = BackMusicFiles.Cataclysm;
                        }                   
                    }
                    break;
                case BackMusicFiles.Pandaria:
                     {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        uint newIndex;
                        if (Engine.Instance.AudioPlayer.LoadMp3(@"Sound\music\Pandaria\MUS_50_HeartofPandaria_01.mp3", out newIndex))
                        {
                            Engine.Instance.AudioPlayer.PlayMp3(newIndex, true);
                            ModelEditorService.Instance.MusicIndex = (int)newIndex;
                            ModelEditorService.Instance.BackMusic = BackMusicFiles.Pandaria;
                        }                   
                    }
                    break;
                case BackMusicFiles.Draenor:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        uint newIndex;
                        if (Engine.Instance.AudioPlayer.LoadMp3(@"Sound\music\Draenor\MUS_60_ASiegeofWorlds_MainTitle.mp3", out newIndex))
                        {
                            Engine.Instance.AudioPlayer.PlayMp3(newIndex, true);
                            ModelEditorService.Instance.MusicIndex = (int)newIndex;
                            ModelEditorService.Instance.BackMusic = BackMusicFiles.Draenor;
                        }
                    }
                    break;
                case BackMusicFiles.HighBorne:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        uint newIndex;
                        if (Engine.Instance.AudioPlayer.LoadMp3(@"Sound\music\GlueScreenMusic\BCCredits_Lament_of_the_Highborne.mp3", out newIndex))
                        {
                            Engine.Instance.AudioPlayer.PlayMp3(newIndex, true);
                            ModelEditorService.Instance.MusicIndex = (int)newIndex;
                            ModelEditorService.Instance.BackMusic = BackMusicFiles.HighBorne;
                        }                   
                    }
                    break;
                case BackMusicFiles.Stormwind:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        musicIndex = 0;
                        PlayMusicStormwind();
                        ModelEditorService.Instance.BackMusic = BackMusicFiles.Stormwind;
                    }
                    break;
                case BackMusicFiles.Darnassus:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        musicIndex = 0;
                        PlayMusicDarnassus();
                        ModelEditorService.Instance.BackMusic = BackMusicFiles.Darnassus;
                    }
                    break;
                case BackMusicFiles.Ironforge:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        musicIndex = 0;
                        PlayMusicIronForge();
                        ModelEditorService.Instance.BackMusic = BackMusicFiles.Ironforge;
                    }
                    break;
                case BackMusicFiles.Orgrimmar:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        musicIndex = 0;
                        PlayMusicOrgrimmar();
                        ModelEditorService.Instance.BackMusic = BackMusicFiles.Orgrimmar;
                    }
                    break;
                case BackMusicFiles.Undercity:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        musicIndex = 0;
                        PlayMusicUnderCity();
                        ModelEditorService.Instance.BackMusic = BackMusicFiles.Undercity;
                    }
                    break;
                case BackMusicFiles.Thunderbluff:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        musicIndex = 0;
                        PlayMusicThunderBluff();
                        ModelEditorService.Instance.BackMusic = BackMusicFiles.Thunderbluff;
                    }
                    break;
                case BackMusicFiles.Silvermoon:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        musicIndex = 0;
                        PlayMusicSilvermoon();
                        ModelEditorService.Instance.BackMusic = BackMusicFiles.Silvermoon;
                    }
                    break;
                case BackMusicFiles.Garrosh:
                    {
                        int index = ModelEditorService.Instance.MusicIndex;
                        if (index != -1)
                        {
                            Engine.Instance.AudioPlayer.FadeoutMp3((uint)index, 5000);
                        }

                        musicIndex = 0;
                        PlayMusicGarrosh();
                        ModelEditorService.Instance.BackMusic = BackMusicFiles.Garrosh;
                    }
                    break;
            }
        }

        private static int musicIndex = 0;

        private static readonly string[] stormwindFiles = new string[]
        {
            @"Sound\music\CityMusic\Stormwind\stormwind_intro-moment.mp3",
            @"Sound\music\CityMusic\Stormwind\stormwind01-moment.mp3",
            @"Sound\music\CityMusic\Stormwind\stormwind02-moment.mp3",
            @"Sound\music\CityMusic\Stormwind\stormwind03-moment.mp3",
            @"Sound\music\cataclysm\MUS_Stormwind_GU01.mp3",
            @"Sound\music\cataclysm\MUS_Stormwind_GU02.mp3",
            @"Sound\music\cataclysm\MUS_Stormwind_GU03.mp3",
        };

        private static readonly string[] darnassusFiles = new string[]
        {
            @"Sound\music\cataclysm\MUS_Darnassus_GI01.mp3",
            @"Sound\music\cataclysm\MUS_Darnassus_GI02.mp3",
        };

        private static readonly string[] ironforgeFiles = new string[]
        {
            @"Sound\music\CityMusic\Ironforge\IronForge Intro.mp3",
            @"Sound\music\cataclysm\MUS_DarkIronforge_GU01.mp3",
            @"Sound\music\cataclysm\MUS_DarkIronforge_GU02.mp3",
            @"Sound\music\cataclysm\MUS_DarkIronforge_GU03.mp3",
            @"Sound\music\cataclysm\MUS_DarkIronforge_GU04.mp3",
            @"Sound\music\cataclysm\MUS_DarkIronforge_GU05.mp3",
            @"Sound\music\cataclysm\MUS_DarkIronforge_GU06.mp3",
            @"Sound\music\cataclysm\MUS_DarkIronforge_GU07.mp3",
        };

        private static readonly string[] orgrimmarFiles = new string[]
        {
            @"Sound\music\CityMusic\Orgrimmar\orgrimmar_intro-moment.mp3",
            @"Sound\music\cataclysm\MUS_Orgrimmar_GU01.mp3",
            @"Sound\music\cataclysm\MUS_OrgrimmarLegacy_GU01.mp3",
        };

        private static readonly string[] undeadFiles = new string[]
        {
            @"Sound\music\CityMusic\Undercity\undercityintro-moment.mp3",
            @"Sound\music\cataclysm\MUS_Undead_UU01.mp3",
            @"Sound\music\cataclysm\MUS_Undead_UU02.mp3",
            @"Sound\music\cataclysm\MUS_Undead_UU03.mp3",
            @"Sound\music\cataclysm\MUS_Undead_UU04.mp3",
            @"Sound\music\cataclysm\MUS_Undead_UU05.mp3",
        };

        private static readonly string[] thunderbluffFiles = new string[]
        {
            @"Sound\music\CityMusic\Thunderbluff\Thunderbluff Intro.mp3",
            @"Sound\music\cataclysm\MUS_ThunderBluff_GU01.mp3",
            @"Sound\music\cataclysm\MUS_ThunderBluff_GU02.mp3",
            @"Sound\music\cataclysm\MUS_ThunderBluff_GU03.mp3",
        };

        private static readonly string[] silvermoonFiles = new string[]
        {
            @"Sound\music\ZoneMusic\Eversong\ES_SilvermoonIntro01.mp3",
            @"Sound\music\ZoneMusic\Sunwell\SW_MagistersTerraceWalkUni02.mp3",
            @"Sound\music\ZoneMusic\Sunwell\SW_MagistersTerraceWalkUni03.mp3",
        };

        private static readonly string[] garroshFiles = new string[]
        {
            @"Sound\music\Pandaria\MUS_51_GarroshTheme_B_01.mp3",
            @"Sound\music\Pandaria\MUS_51_GarroshTheme_B_02.mp3",
            @"Sound\music\Pandaria\MUS_51_GarroshTheme_A_Hero_01.mp3",
            @"Sound\music\Pandaria\MUS_51_GarroshTheme_B_Hero_01.mp3",
        };

        private static string GetMusicFile(BackMusicFiles music)
        {
            switch(music)
            {
                case BackMusicFiles.Stormwind:
                    {
                        int n = musicIndex % (stormwindFiles.GetLength(0));
                        musicIndex = n + 1;
                        return stormwindFiles[n];
                    }
                case BackMusicFiles.Darnassus:
                    {
                        int n = musicIndex % (darnassusFiles.GetLength(0));
                        musicIndex = n + 1;
                        return darnassusFiles[n];
                    }
                case BackMusicFiles.Ironforge:
                    {
                        int n = musicIndex % (ironforgeFiles.GetLength(0));
                        musicIndex = n + 1;
                        return ironforgeFiles[n];
                    }
                case BackMusicFiles.Orgrimmar:
                    {
                        int n = musicIndex % (orgrimmarFiles.GetLength(0));
                        musicIndex = n + 1;
                        return orgrimmarFiles[n];
                    }
                case BackMusicFiles.Undercity:
                    {
                        int n = musicIndex % (undeadFiles.GetLength(0));
                        musicIndex = n + 1;
                        return undeadFiles[n];
                    }
                case BackMusicFiles.Thunderbluff:
                    {
                        int n = musicIndex % (thunderbluffFiles.GetLength(0));
                        musicIndex = n + 1;
                        return thunderbluffFiles[n];
                    }
                case BackMusicFiles.Silvermoon:
                    {
                        int n = musicIndex % (silvermoonFiles.GetLength(0));
                        musicIndex = n + 1;
                        return silvermoonFiles[n];
                    }
                case BackMusicFiles.Garrosh:
                    {
                        int n = musicIndex % (garroshFiles.GetLength(0));
                        musicIndex = n + 1;
                        return garroshFiles[n];
                    }
            }
            return "";
        }

        public static FnSoundCallback playMusicStormwind = new FnSoundCallback(PlayMusicStormwind);
        public static FnSoundCallback playMusicDarnassus = new FnSoundCallback(PlayMusicDarnassus);
        public static FnSoundCallback playMusicIronForge = new FnSoundCallback(PlayMusicIronForge);
        public static FnSoundCallback playMusicOrgrimmar = new FnSoundCallback(PlayMusicOrgrimmar);
        public static FnSoundCallback playMusicUnderCity = new FnSoundCallback(PlayMusicUnderCity);
        public static FnSoundCallback playMusicThunderBluff = new FnSoundCallback(PlayMusicThunderBluff);
        public static FnSoundCallback playMusicSilverMoon = new FnSoundCallback(PlayMusicSilvermoon);
        public static FnSoundCallback playMusicGarrosh = new FnSoundCallback(PlayMusicGarrosh);

        private static void PlayMusicStormwind()
        {
            uint newIndex;
            if (Engine.Instance.AudioPlayer.LoadMp3(GetMusicFile(BackMusicFiles.Stormwind), out newIndex))
            {
                Engine.Instance.AudioPlayer.PlayMp3Callback(newIndex, playMusicStormwind);
                ModelEditorService.Instance.MusicIndex = (int)newIndex;
            }           
        }

        private static void PlayMusicDarnassus()
        {
            uint newIndex;
            if (Engine.Instance.AudioPlayer.LoadMp3(GetMusicFile(BackMusicFiles.Darnassus), out newIndex))
            {
                Engine.Instance.AudioPlayer.PlayMp3Callback(newIndex, playMusicDarnassus);
                ModelEditorService.Instance.MusicIndex = (int)newIndex;
            }
        }

        private static void PlayMusicIronForge()
        {
            uint newIndex;
            if (Engine.Instance.AudioPlayer.LoadMp3(GetMusicFile(BackMusicFiles.Ironforge), out newIndex))
            {
                Engine.Instance.AudioPlayer.PlayMp3Callback(newIndex, playMusicIronForge);
                ModelEditorService.Instance.MusicIndex = (int)newIndex;
            }
        }

        private static void PlayMusicOrgrimmar()
        {
            uint newIndex;
            if (Engine.Instance.AudioPlayer.LoadMp3(GetMusicFile(BackMusicFiles.Orgrimmar), out newIndex))
            {
                Engine.Instance.AudioPlayer.PlayMp3Callback(newIndex, playMusicOrgrimmar);
                ModelEditorService.Instance.MusicIndex = (int)newIndex;
            }
        }

        private static void PlayMusicUnderCity()
        {
            uint newIndex;
            if (Engine.Instance.AudioPlayer.LoadMp3(GetMusicFile(BackMusicFiles.Undercity), out newIndex))
            {
                Engine.Instance.AudioPlayer.PlayMp3Callback(newIndex, playMusicUnderCity);
                ModelEditorService.Instance.MusicIndex = (int)newIndex;
            }
        }

        private static void PlayMusicThunderBluff()
        {
            uint newIndex;
            if (Engine.Instance.AudioPlayer.LoadMp3(GetMusicFile(BackMusicFiles.Thunderbluff), out newIndex))
            {
                Engine.Instance.AudioPlayer.PlayMp3Callback(newIndex, playMusicThunderBluff);
                ModelEditorService.Instance.MusicIndex = (int)newIndex;
            }
        }

        private static void PlayMusicSilvermoon()
        {
            uint newIndex;
            if (Engine.Instance.AudioPlayer.LoadMp3(GetMusicFile(BackMusicFiles.Silvermoon), out newIndex))
            {
                Engine.Instance.AudioPlayer.PlayMp3Callback(newIndex, playMusicSilverMoon);
                ModelEditorService.Instance.MusicIndex = (int)newIndex;
            }
        }

        private static void PlayMusicGarrosh()
        {
            uint newIndex;
            if (Engine.Instance.AudioPlayer.LoadMp3(GetMusicFile(BackMusicFiles.Garrosh), out newIndex))
            {
                Engine.Instance.AudioPlayer.PlayMp3Callback(newIndex, playMusicGarrosh);
                ModelEditorService.Instance.MusicIndex = (int)newIndex;
            }
        }
    }
}
