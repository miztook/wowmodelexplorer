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
using AvalonDock;
using WowModelExplorer.Controls;
using WowModelExplorer.Data;
using WowModelExplorer.Services;
using mywowNet;
using mywowNet.Data;

namespace WowModelExplorer.Views
{
    /// <summary>
    /// Interaction logic for M2StateContent.xaml
    /// </summary>
    public partial class M2StateContent : DockableContent
    {
        public M2StateContent()
        {
            InitializeComponent();
        }

        public void Initialize()
        {
             _stateSelect.Initialize();

             ModelSceneService.Instance.MainM2SceneNodeChanged += new MainM2SceneNode_ChangedHandler(Instance_MainM2SceneNodeChanged);

             _stateSelect.State_Selected += new M2State_SelectedEventHandler(_stateSelect_State_Selected);
        }

        void Instance_MainM2SceneNodeChanged(object sender, M2SceneNode node)
        {
            _stateSelect.FillData(node);
        }

        void _stateSelect_State_Selected(object sender, M2State state)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if ( node != null && node.M2Fsm != null)
            {
                node.M2Fsm.ChangeState(state.State);
            }
        }

    }
}
