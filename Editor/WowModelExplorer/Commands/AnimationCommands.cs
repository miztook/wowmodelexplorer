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
    public class AnimationCommands
    {
        private static RoutedUICommand playCommand = new RoutedUICommand();
        private static RoutedUICommand stepCommand = new RoutedUICommand();
        private static RoutedUICommand pauseCommand = new RoutedUICommand();
        private static RoutedUICommand stopCommand = new RoutedUICommand();
        private static RoutedUICommand setCommand = new RoutedUICommand();

        public static RoutedUICommand Play { get { return playCommand; } }
        public static RoutedUICommand Step { get { return stepCommand; } }
        public static RoutedUICommand Pause { get { return pauseCommand; } }
        public static RoutedUICommand Stop { get { return stopCommand; } }
        public static RoutedUICommand Set { get { return setCommand; } }

        private readonly CommandBindingCollection commandBindings = new CommandBindingCollection();

        public CommandBindingCollection CommandBindings { get { return commandBindings; } }

        public AnimationCommands()
        {
            commandBindings.Add(new CommandBinding(playCommand, playExecuted));
            commandBindings.Add(new CommandBinding(stepCommand, stepExecuted));
            commandBindings.Add(new CommandBinding(pauseCommand, pauseExecuted));
            commandBindings.Add(new CommandBinding(stopCommand, stopExecuted));
            commandBindings.Add(new CommandBinding(setCommand, setExecuted));
        }

         private void playExecuted(object sender, ExecutedRoutedEventArgs e)
         {
             M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
             AnimationContent content = ShellService.Instance.AnimationContent;
             int animIndex = content.GetCurrentAnimaitonIndex();
             if (node == null || animIndex == -1 || Client.Instance.GetPlayer().GetRideNpcId() != 0)
                 return;

             if (animIndex != node.AnimationPlayer.CurrentAnimationIndex ||
                 node.AnimationPlayer.Loop != content._animationSelect._animationToolbar.Loop)
             {
                 if (node.M2Fsm != null)
                 {
                    node.M2Fsm.ResetState();
                 }
                 
                 node.AnimationPlayer.Loop = content._animationSelect._animationToolbar.Loop;

                 node.PlayAnimation((uint)animIndex, node.AnimationPlayer.Loop, 200);
             }

             node.AnimationPlayer.Resume();
         }

         private void stepExecuted(object sender, ExecutedRoutedEventArgs e)
         {
             M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
             AnimationContent content = ShellService.Instance.AnimationContent;
             int animIndex = content.GetCurrentAnimaitonIndex();
             if (node == null || animIndex == -1)
                 return;

             node.AnimationPlayer.Step(25);
         }

         private void pauseExecuted(object sender, ExecutedRoutedEventArgs e)
         {
             M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
             AnimationContent content = ShellService.Instance.AnimationContent;
             int animIndex = content.GetCurrentAnimaitonIndex();
             if (node == null || animIndex == -1)
                 return;

             node.AnimationPlayer.Pause();
         }

         private void stopExecuted(object sender, ExecutedRoutedEventArgs e)
         {
             M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
             AnimationContent content = ShellService.Instance.AnimationContent;
             int animIndex = content.GetCurrentAnimaitonIndex();
             if (node == null || animIndex == -1)
                 return;

             node.AnimationPlayer.Stop();
         }

         private void setExecuted(object sender, ExecutedRoutedEventArgs e)
         {
             AnimationConfigWindow window = new AnimationConfigWindow();
             window.ShowDialog();
         }
    }
}
