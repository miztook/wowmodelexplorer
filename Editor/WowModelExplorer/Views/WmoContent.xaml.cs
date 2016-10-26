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
    /// Interaction logic for WmoContent.xaml
    /// </summary>
    public partial class WmoContent : DockableContent
    {
        public WmoContent()
        {
            InitializeComponent();
            _wmoSelect.MapLocalWMO_Selected += new MapWMO_SelectedEventHandler(_wmoSelect_MapLocalWMO_Selected);
            _wmoTreeViewSelect.WmoTreeView_Selected += new Wmo_TreeViewSelectedEventHandler(_wmoTreeViewSelect_WmoTreeView_Selected);

            _wmoSelect.M2Model_Selected += new WorldM2_SelectedEventHandler(_wmoSelect_M2Model_Selected);
            _wmoTreeViewSelect.M2Model_Selected += new WorldM2_SelectedEventHandler(_wmoTreeViewSelect_M2Model_Selected);
         }

        void _wmoSelect_M2Model_Selected(object sender, string m2name)
        {
            LoadWorldM2(m2name);
        }

        void _wmoTreeViewSelect_M2Model_Selected(object sender, string m2name)
        {
            LoadWorldM2(m2name);
        }

        void _wmoSelect_MapLocalWMO_Selected(object sender, Entry entry)
        {
            WowDatabase database = Engine.Instance.WowDatabase;
            string path = database.GetWMOFileName((uint)entry.Id, false);
            WMOSceneNode node = ModelSceneService.Instance.SetMainWMOSceneNode(path);
            ModelEditorService.Instance.SetWireFrame(node);

            //update doodad names
            _wmoSelect.UpdateDoodadM2Names(node);

            //update property
            if (node != null)
                ShellService.Instance.PropertyContent.UpdateWmoNode(node);
        }

        void _wmoTreeViewSelect_WmoTreeView_Selected(object sender, string wmofilename)
        {
            WowDatabase database = Engine.Instance.WowDatabase;
            string path = wmofilename;
            WMOSceneNode node = ModelSceneService.Instance.SetMainWMOSceneNode(path);
            ModelEditorService.Instance.SetWireFrame(node);

            //update doodad names
            _wmoTreeViewSelect.UpdateDoodadM2Names(node);

            //update property
            if (node != null)
                ShellService.Instance.PropertyContent.UpdateWmoNode(node);
        }

        public void Initialize()
        {
            _wmoSelect.FillData();
            _wmoTreeViewSelect.FillData();
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
    }
}
