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
    public class ModelOperationCommands
    {
        private static RoutedUICommand resetClothesAllCommand = new RoutedUICommand();
        private static RoutedUICommand resetSlotCommand = new RoutedUICommand();
        private static RoutedUICommand resetWeaponAllCommand = new RoutedUICommand();
        private static RoutedUICommand modelColorCommand = new RoutedUICommand();
        private static RoutedUICommand geosetsWindowCommand = new RoutedUICommand();
        private static RoutedUICommand attachmentsWindowCommand = new RoutedUICommand();
        private static RoutedUICommand modelEquipmentsCommand = new RoutedUICommand();
        private static RoutedUICommand modelEquipmentSelectCommand = new RoutedUICommand();
        private static RoutedUICommand characterInfoCommand = new RoutedUICommand();
        private static RoutedUICommand modelArmoryCommand = new RoutedUICommand();
        private static RoutedUICommand exportObjCommand = new RoutedUICommand();
        private static RoutedUICommand exportFbxCommand = new RoutedUICommand();

        public static RoutedUICommand ResetClothesAll { get { return resetClothesAllCommand; } }
        public static RoutedUICommand ResetSlot { get { return resetSlotCommand; } }
        public static RoutedUICommand ResetWeaponAll { get { return resetWeaponAllCommand; } }
        public static RoutedUICommand ModelColor { get { return modelColorCommand; } }
        public static RoutedUICommand GeosetsWindow { get { return geosetsWindowCommand; } }
        public static RoutedUICommand AttachmentsWindow { get { return attachmentsWindowCommand; } }
        public static RoutedUICommand ModelEquipments { get { return modelEquipmentsCommand; } }
        public static RoutedUICommand ModelEquipmentSelect { get { return modelEquipmentSelectCommand; } }
        public static RoutedUICommand CharacterInfo { get { return characterInfoCommand; } }
        public static RoutedUICommand ModelArmory { get { return modelArmoryCommand; } }
        public static RoutedUICommand ExportObj { get { return exportObjCommand; } }
        public static RoutedUICommand ExportFbx { get { return exportFbxCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public ModelOperationCommands()
        {
            commandBindings.Add(new CommandBinding(resetClothesAllCommand, 
                ResetClothesAllExecuted, 
                CanCharacterExecuted));
            commandBindings.Add(new CommandBinding(resetSlotCommand,
                ResetClothesSlotExecuted,
                CanResetSlotExecuted));
            commandBindings.Add(new CommandBinding(resetWeaponAllCommand,
                ResetWeaponAllExecuted,
                CanCharacterExecuted));
            commandBindings.Add(new CommandBinding(modelColorCommand,
                ModelColorExecuted,
                CanModelExecuted));
            commandBindings.Add(new CommandBinding(geosetsWindowCommand, GeosetsWindowExecuted));
            commandBindings.Add(new CommandBinding(attachmentsWindowCommand, AttachmentWindowExecuted));
            commandBindings.Add(new CommandBinding(modelEquipmentsCommand, ModelEquipmentsExecuted));
            commandBindings.Add(new CommandBinding(modelEquipmentSelectCommand, ModelEquipmentSelectExecuted));
            commandBindings.Add(new CommandBinding(characterInfoCommand, CharacterInfoExecuted));
            commandBindings.Add(new CommandBinding(modelArmoryCommand, ModelArmoryExecuted));
            commandBindings.Add(new CommandBinding(exportObjCommand, ExportObjExecuted, CanWorldModelExecuted));
            commandBindings.Add(new CommandBinding(exportFbxCommand, ExportFbxExecuted, CanWorldModelExecuted));
        }

        private void ResetClothesAllExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            node.ResetSlot(E_CHAR_SLOTS.CS_HEAD);
            node.ResetSlot(E_CHAR_SLOTS.CS_SHOULDER);
            node.ResetSlot(E_CHAR_SLOTS.CS_SHIRT);
            node.ResetSlot(E_CHAR_SLOTS.CS_CHEST);
            node.ResetSlot(E_CHAR_SLOTS.CS_BELT);
            node.ResetSlot(E_CHAR_SLOTS.CS_PANTS);
            node.ResetSlot(E_CHAR_SLOTS.CS_BOOTS);
            node.ResetSlot(E_CHAR_SLOTS.CS_BRACERS);
            node.ResetSlot(E_CHAR_SLOTS.CS_GLOVES);
            node.ResetSlot(E_CHAR_SLOTS.CS_CAPE);
            node.ResetSlot(E_CHAR_SLOTS.CS_TABARD);

            node.UpdateCharacter();

            //update property
            ShellService.Instance.PropertyContent.UpdateCharacterNode(node);

            if (ShellService.Instance.EquipmentsWindow.IsVisible)
                ShellService.Instance.EquipmentsWindow.Update();

            if (ShellService.Instance.EquipmentSelectWindow.IsVisible)
                ShellService.Instance.EquipmentSelectWindow.Update();
        }

        private void ResetClothesSlotExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            E_CHAR_SLOTS? slot = e.Parameter as E_CHAR_SLOTS?;
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (slot != null)
            {
                node.ResetSlot(slot.Value);
                if (node.M2Instance.SlotHasModel(slot.Value))
                {
                    if (slot.Value == E_CHAR_SLOTS.CS_HEAD)
                        node.BuildVisibleGeosets();
                    else if (slot.Value == E_CHAR_SLOTS.CS_BELT)
                        node.UpdateCharacter();
                }
                else
                {
                    node.UpdateCharacter();
                }
            }

            //update property
            ShellService.Instance.PropertyContent.UpdateCharacterNode(node);

            if (ShellService.Instance.EquipmentsWindow.IsVisible)
                ShellService.Instance.EquipmentsWindow.Update();

            if (ShellService.Instance.EquipmentSelectWindow.IsVisible)
                ShellService.Instance.EquipmentSelectWindow.Update();
        }

        private void ResetWeaponAllExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            node.ResetSlot(E_CHAR_SLOTS.CS_HAND_LEFT);
            node.ResetSlot(E_CHAR_SLOTS.CS_HAND_RIGHT);

            //update property
            ShellService.Instance.PropertyContent.UpdateCharacterNode(node);

            if (ShellService.Instance.EquipmentsWindow.IsVisible)
                ShellService.Instance.EquipmentsWindow.Update();

            if (ShellService.Instance.EquipmentSelectWindow.IsVisible)
                ShellService.Instance.EquipmentSelectWindow.Update();
        }

        private void ModelColorExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            EditorSetOptions? option = e.Parameter as EditorSetOptions?;
            if (!option.HasValue)
                return;

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;

            switch(option.Value)
            {
                case EditorSetOptions.Set:
                     ColorPickerControls.Dialogs.ColorPickerStandardDialog dialog = new ColorPickerControls.Dialogs.ColorPickerStandardDialog();
                        dialog.colorPickerFull.SelectedColorChanged += (s, param) =>
                        {
                            node.ModelColor = param.Value.ToSColor();
                        };
                        SColor scolor = node.ModelColor;
                        dialog.InitialColor = Color.FromArgb(scolor.A, scolor.R, scolor.G, scolor.B);
                        dialog.Owner = ShellService.Instance.MainWindow;
                        bool? dialogResult = dialog.ShowDialog();
                        if (dialogResult != null && (bool)dialogResult == true)
                        {
                            node.ModelColor = dialog.SelectedColor.ToSColor();
                        }
                        else
                        {
                            node.ModelColor = dialog.InitialColor.ToSColor();
                        }
                    break;
                case EditorSetOptions.Default:
                    node.ModelColor = Colors.White.ToSColor();
                    break;
            }
        }

        private void GeosetsWindowExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            M2GeosetsWindow window = ShellService.Instance.GeosetsWindow;
            if (window.IsVisible)
                window.Hide();
            else
            {
                window.Show();
                window.Update();
            }
        }

        private void AttachmentWindowExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            M2AttachmentsWindow window = ShellService.Instance.AttachmentsWindow;
            if (window.IsVisible)
                window.Hide();
            else
            {
                window.Show();
                window.Update();
            }
        }

        private void ModelEquipmentsExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            M2EquipmentsWindow window = ShellService.Instance.EquipmentsWindow;
            if (window.IsVisible)
                window.Hide();
            else
            {
                window.Update();
                window.Show();
            }
        }

        public void ModelEquipmentSelectExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            M2EquipmentSelectWindow window = ShellService.Instance.EquipmentSelectWindow;
            if (window.IsVisible)
                window.Hide();
            else
            {
                window.Update();
                window.Show();
            }
        }

        private void CharacterInfoExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            M2CharacterInfoWindow window = ShellService.Instance.CharacterInfoWindow;
            if (window.IsVisible)
                window.Hide();
            else
            {
                window.Update();
                window.Show();
            }
        }

        private void ModelArmoryExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            M2ArmoryWindow window = ShellService.Instance.ArmoryWindow;
            if (window.IsVisible)
                window.Hide();
            else
            {
                window.Update();
                window.Show();
            }
        }

        private void CanCharacterExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            e.CanExecute = (node != null && node.Type == M2Type.MT_CHARACTER && !node.IsNpc);
        }

        private void CanModelExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            e.CanExecute = node != null;
        }

        private void CanResetSlotExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            bool slotEmpty = true;
            bool isChar = node != null && node.Type == M2Type.MT_CHARACTER && node.IsNpc == false;
            if (e.Parameter != null && isChar)
            {
                E_CHAR_SLOTS slot = (E_CHAR_SLOTS)e.Parameter;
                slotEmpty = 0 == node.M2Instance.GetSlotItemId(slot);
            }

            e.CanExecute = isChar && !slotEmpty;
        }

        private void CanWorldModelExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            e.CanExecute = false;
            if (node != null && node.Type == M2Type.MT_WORLD)
            {
//                 string strLocale = Engine.Instance.WowEnvironment.GetLocale();
//                 if (strLocale == "" || strLocale.Equals("zhCN", StringComparison.CurrentCultureIgnoreCase))
//                 {
//                     string fullname = node.FileM2.longname;
//                     if (fullname.IndexOf("world/azeroth", StringComparison.CurrentCultureIgnoreCase) != -1)
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

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;

            bool success = mwTool.Instance.ExportM2SceneNodeToOBJ(node, dirPath);

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
