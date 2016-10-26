using System;
using System.Windows.Input;
using AvalonDock;
using WowModelExplorer.Controls;
using WowModelExplorer.Data;
using mywowNet;
using mywowNet.Data;
using WowModelExplorer.Services;

namespace WowModelExplorer.Views
{
    /// <summary>
    /// Interaction logic for MapM2Content.xaml
    /// </summary>
    public partial class MapModelNameContent : DockableContent
    {
        public MapModelNameContent()
        {
            InitializeComponent();

            _mapModelNames.MapM2_Selected += new MapM2_SelectedEventHandler(_mapM2Names_MapM2_Selected);
            _mapModelNames.MapLocalWMO_Selected += new MapWMO_SelectedEventHandler(_mapM2Names_MapLocalWMO_Selected);
        }

        void _mapM2Names_MapLocalWMO_Selected(object sender, Entry entry)
        {
            FileADT adt = ModelSceneService.Instance.CurrentADT;
            if (adt == null)
                return;
            string path = adt.GetWMOFileName((uint)entry.Id, false);
            WMOSceneNode node = ModelSceneService.Instance.SetMainWMOSceneNode(path);
            ModelEditorService.Instance.SetWireFrame(node);

            //update property
            if(node != null)
                ShellService.Instance.PropertyContent.UpdateWmoNode(node);
        }

        void _mapM2Names_MapM2_Selected(object sender, Entry entry)
        {
            FileADT adt = ModelSceneService.Instance.CurrentADT;
            if (adt == null)
                return;
            string path = adt.GetM2FileName((uint)entry.Id, false);
            M2SceneNode node = ModelSceneService.Instance.SetMainM2SceneNode(path, false);
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

        public void FillADTData(FileADT fileadt)
        {
            _mapModelNames.FillADTData(fileadt);
        }

    }
}
