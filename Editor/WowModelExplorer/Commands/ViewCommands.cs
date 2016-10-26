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
            switch(type.Value)
            {         
                case ModelViewTypes.Character:
                    content = ShellService.Instance.CharacterContent;
                    break;
                case ModelViewTypes.Npc:
                    content = ShellService.Instance.NpcContent;
                    break;
                case ModelViewTypes.Ridable:
                    content = ShellService.Instance.RidableContent;
                    break;
                case ModelViewTypes.SpellVisualEffect:
                    content = ShellService.Instance.SpellVisualEffectContent;
                    break;
                case ModelViewTypes.MapModel:
                    content = ShellService.Instance.MapModelContent;
                    break;
                case ModelViewTypes.Wmo:
                    content = ShellService.Instance.WmoContent;
                    break;
                case ModelViewTypes.WorldM2:
                    content = ShellService.Instance.WorldM2Content;
                    break;
                case ModelViewTypes.Clothes:
                    content = ShellService.Instance.ClothesContent;
                    break;
                case ModelViewTypes.Weapon:
                    content = ShellService.Instance.WeaponContent;
                    break;
                case ModelViewTypes.Set:
                    content = ShellService.Instance.SetContent;
                    break;
                case ModelViewTypes.Animation:
                    content = ShellService.Instance.AnimationContent;
                    break;
                case ModelViewTypes.MapModelName:
                    content = ShellService.Instance.MapModelNameContent;
                    break;
                case ModelViewTypes.Property:
                    content = ShellService.Instance.PropertyContent;
                    break;
                case ModelViewTypes.M2State:
                    content = ShellService.Instance.M2StateContent;
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
