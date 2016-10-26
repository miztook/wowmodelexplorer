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
    public class ModelEditViewCommands
    {
        private static RoutedUICommand showEditPartCommand = new RoutedUICommand();
        private static RoutedUICommand showModelPartCommand = new RoutedUICommand();
        private static RoutedUICommand wireFrameCommand = new RoutedUICommand();
        private static RoutedUICommand stateModeCommand = new RoutedUICommand();
        private static RoutedUICommand sheathWeaponCommand = new RoutedUICommand();

        public static RoutedUICommand ShowEditPart { get { return showEditPartCommand; } }
        public static RoutedUICommand ShowModelPart { get { return showModelPartCommand; } }
        public static RoutedUICommand WireFrame { get { return wireFrameCommand; } }
        public static RoutedUICommand StateMode { get { return stateModeCommand; } }
        public static RoutedUICommand SheathWeapon { get { return sheathWeaponCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public ModelEditViewCommands()
        {
            commandBindings.Add(new CommandBinding(showEditPartCommand, ShowEditPartExecuted, CanM2Executed));

            commandBindings.Add(new CommandBinding(showModelPartCommand, ShowModelPartExecuted, CanCharacterExecuted));

            commandBindings.Add(new CommandBinding(wireFrameCommand, WireFrameExecuted, CanM2Executed));

            commandBindings.Add(new CommandBinding(stateModeCommand, StateModeExecuted));

            commandBindings.Add(new CommandBinding(sheathWeaponCommand, SheathWeaponExecuted));
        }

        private void ShowEditPartExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            E_MODEL_EDIT_PART? part = e.Parameter as E_MODEL_EDIT_PART?;
            if (!part.HasValue)
                return;

            bool show = ModelEditorService.Instance.IsShow(part.Value);

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                switch (part.Value)
                {
                    case E_MODEL_EDIT_PART.EEP_MODEL:
                        node.ShowEditPart(part.Value, !show);
                        break;
                    case E_MODEL_EDIT_PART.EEP_PARTICLES:
                        node.ShowEditPart(part.Value, !show);
                        break;
                    case E_MODEL_EDIT_PART.EEP_BOUNDINGBOX:
                        node.ShowEditPart(part.Value, !show);
                        break;
                    case E_MODEL_EDIT_PART.EEP_BONES:
                        node.ShowEditPart(part.Value, !show);
                        break;
                    case E_MODEL_EDIT_PART.EEP_BOUNDINGAABOX:
                        node.ShowEditPart(part.Value, !show);
                        break;
                    case E_MODEL_EDIT_PART.EEP_COLLISIONAABOX:
                        node.ShowEditPart(part.Value, !show);
                        break;
                }
            }

            ModelEditorService.Instance.SetShow(part.Value, !show);
        }

        private void ShowModelPartExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            E_MODEL_PART? part = e.Parameter as E_MODEL_PART?;
            if (!part.HasValue)
                return;

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            bool show = ModelEditorService.Instance.IsShow(part.Value);

            node.ShowModelPart(part.Value, !show);
            if (part.Value == E_MODEL_PART.EMP_ALL || part.Value == E_MODEL_PART.EMP_CAPE)
                node.BuildVisibleGeosets();
            else if (part.Value == E_MODEL_PART.EMP_HEAD)
                node.BuildVisibleGeosets();

            ModelEditorService.Instance.SetShow(part.Value, !show);
        }

        private void WireFrameExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            E_OVERRIDE_WIREFRAME? wireframe = e.Parameter as E_OVERRIDE_WIREFRAME?;
            if (!wireframe.HasValue)
                return;

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
                node.SetWireFrame(wireframe.Value);

            ModelEditorService.Instance.M2WireFrame = wireframe.Value;
        }

        private void StateModeExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            ModelEditorService.Instance.StateMode = !ModelEditorService.Instance.StateMode;
        }

        private void SheathWeaponExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            ModelEditorService.Instance.SheathWeapon = !ModelEditorService.Instance.SheathWeapon;

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                ModelEditorService.Instance.SheathM2NodeWeapon(node);
            }
        }

        private void CanM2Executed(object sender, CanExecuteRoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            e.CanExecute = (node != null);
        }

        private void CanCharacterExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            e.CanExecute = (node != null && node.Type == M2Type.MT_CHARACTER && !node.IsNpc);
        }
    }
}
