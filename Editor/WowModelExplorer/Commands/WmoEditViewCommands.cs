using System;
using System.Collections.Generic;
using System.Windows.Input;
using AvalonDock;
using mywowNet;
using WowModelExplorer;
using WowModelExplorer.Services;
using WowModelExplorer.Views;
using WowModelExplorer.Controls;

namespace WowModelExplorer.Commands
{
    public class WmoEditViewCommands
    {
        private static RoutedUICommand showEditPartCommand = new RoutedUICommand();
        private static RoutedUICommand wireFrameCommand = new RoutedUICommand();

        public static RoutedUICommand ShowEditPart { get { return showEditPartCommand; } }
        public static RoutedUICommand WireFrame { get { return wireFrameCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public WmoEditViewCommands()
        {
            commandBindings.Add(new CommandBinding(showEditPartCommand, ShowEditPartExecuted, CanWmoExecuted));

            commandBindings.Add(new CommandBinding(wireFrameCommand, WireFrameExecuted, CanWmoExecuted));

        }

        private void ShowEditPartExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            E_WMO_EDIT_PART? part = e.Parameter as E_WMO_EDIT_PART?;
            if (!part.HasValue)
                return;

            bool show = ModelEditorService.Instance.IsShow(part.Value);

            WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
            if (node != null)
            {
                switch (part.Value)
                {
                    case E_WMO_EDIT_PART.EEP_WMO:
                        node.ShowEditPart(part.Value, !show);
                        break;
                    case E_WMO_EDIT_PART.EEP_DOODADS:
                        node.ShowEditPart(part.Value, !show);
                        break;
                }
            }

            ModelEditorService.Instance.SetShow(part.Value, !show);
        }

        private void WireFrameExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            E_OVERRIDE_WIREFRAME? wireframe = e.Parameter as E_OVERRIDE_WIREFRAME?;
            if (!wireframe.HasValue)
                return;

            WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
            if (node != null)
                node.SetWireFrame(wireframe.Value);

            ModelEditorService.Instance.WmoWireFrame = wireframe.Value;
        }

        private void CanWmoExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
            e.CanExecute = (node != null);
        }
    }
}
