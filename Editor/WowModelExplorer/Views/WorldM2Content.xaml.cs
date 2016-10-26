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
using WowModelExplorer.Data;
using WowModelExplorer.Controls;
using WowModelExplorer.Services;
using mywowNet;
using mywowNet.Data;

namespace WowModelExplorer.Views
{
    /// <summary>
    /// Interaction logic for WorldM2Content.xaml
    /// </summary>
    public partial class WorldM2Content : DockableContent
    {
        public WorldM2Content()
        {
            InitializeComponent();
            _worldM2Select.M2Model_Selected += new WorldM2_SelectedEventHandler(_worldM2Select_M2Model_Selected);
            _worldM2TreeViewSelect.M2ModelTreeView_Selected += new WorldM2_TreeViewSelectedEventHandler(_worldM2TreeViewSelect_M2ModelTreeView_Selected);
        }

        void _worldM2TreeViewSelect_M2ModelTreeView_Selected(object sender, string m2filename)
        {
            LoadWorldM2(m2filename);
        }

        void _worldM2Select_M2Model_Selected(object sender, string m2name)
        {
            LoadWorldM2(m2name);
        }

        private void LoadWorldM2(string m2filename)
        {
            M2SceneNode node = ModelSceneService.Instance.SetMainM2SceneNode(m2filename, false);
            if (node != null)
            {
                ModelEditorService.Instance.ShowEditPart(node);
                ModelEditorService.Instance.SetWireFrame(node);

                node.BuildVisibleGeosets();

                //update property
                ShellService.Instance.PropertyContent.UpdateNormalM2Node(node);
            }

            Player player = Client.Instance.GetPlayer();
            player.SetM2AsTarget(null, 15, (float)(Math.PI / 6.0f));
        }

        public void Initialize()
        {
            _worldM2Select.FillData();
            _worldM2TreeViewSelect.FillData();
        }

    }
}
