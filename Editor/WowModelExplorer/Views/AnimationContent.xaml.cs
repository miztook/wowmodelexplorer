using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using WowModelExplorer.Services;
using WowModelExplorer.Commands;
using mywowNet;
using AvalonDock;

namespace WowModelExplorer.Views
{
    /// <summary>
    /// Interaction logic for AnimationContent.xaml
    /// </summary>
    public partial class AnimationContent : DockableContent
    {
        public AnimationContent()
        {
            InitializeComponent();
        }

        public void Initialize()
        {
            ModelSceneService.Instance.MainM2SceneNodeChanged += new MainM2SceneNode_ChangedHandler(Instance_MainM2SceneNodeChanged);

            CommandBindings.AddRange(ShellService.Instance.AnimationCommands.CommandBindings);

            _animationSelect._animationToolbar.playButton.Command = AnimationCommands.Play;
            _animationSelect._animationToolbar.stepButton.Command = AnimationCommands.Step;
            _animationSelect._animationToolbar.pauseButton.Command = AnimationCommands.Pause;
            _animationSelect._animationToolbar.stopButton.Command = AnimationCommands.Stop;
            _animationSelect._animationToolbar.setButton.Command = AnimationCommands.Set;

            _animationSelect._animationToolbar.loopCheckBox.Checked += new RoutedEventHandler(loopCheckBox_Checked);
            _animationSelect._animationToolbar.loopCheckBox.Unchecked += new RoutedEventHandler(loopCheckBox_Unchecked);
        }

        void Instance_MainM2SceneNodeChanged(object sender, M2SceneNode node)
        {
            _animationSelect.ClearAll();
            if (node != null)
            {
                for (uint i = 0; i < node.AnimationCount; ++i)
                {
                    SAnimation? anim = node.GetAnimation(i);
                    if (anim != null)
                    {
                         string name = anim.Value.name;
                         if (!(anim.Value.subIndex == 0 && anim.Value.next == -1))
                                name += "(" + anim.Value.subIndex.ToString() + ")";

                        _animationSelect.AddAnimation(name, anim.Value.length, anim.Value.animIndex);
                    }
                }
                _animationSelect.RecalculateScrollWidth();

                _animationSelect.SelectAnimation(node.AnimationPlayer.CurrentAnimationIndex, node.AnimationPlayer.Loop);
            }
        }

        private void loopCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                node.AnimationPlayer.Loop = true;
            }
        }

        private void loopCheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                node.AnimationPlayer.Loop = false;
            }
        }

        public void UpdateAnimationProgress(bool autoSelectAnimation)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null)
            {
                if (autoSelectAnimation && _animationSelect.GetCurrentAnimationIndex() != node.AnimationPlayer.CurrentAnimationIndex)
                {
                    _animationSelect.SelectAnimation(node.AnimationPlayer.CurrentAnimationIndex, node.AnimationPlayer.Loop);
                }
                
                _animationSelect.UpdateProgress(node.AnimationPlayer.CurrentAnimationIndex, node.AnimationPlayer.CurrentFrame);
            }
        }

        public int GetCurrentAnimaitonIndex()
        {
            return _animationSelect.GetCurrentAnimationIndex();
        }
    }
}
