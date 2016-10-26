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
    /// Interaction logic for NpcContent.xaml
    /// </summary>
    public partial class NpcContent : DockableContent
    {
        public NpcContent()
        {
            InitializeComponent();

            SubscribeEvents();
        }

        private void NpcContent_Loaded(object sender, RoutedEventArgs e)
        {
            if (_rbOld.IsChecked.Value == false && _rbNew.IsChecked.Value == false)
                _rbNew.IsChecked = ModelEditorService.Instance.IsNpcHD;
        }

        private void SubscribeEvents()
        {
            Loaded += new RoutedEventHandler(NpcContent_Loaded);

            _rbNew.Checked += _rbNewModel_Checked;
            _rbNew.Unchecked += _rbNewModel_Unchecked;

            _npcSelect.Npc_Selected += new Npc_SelectedEventHandler(_npcSelect_Npc_Selected);
            _npcPathSelect.Npc_Selected += new Npc_SelectedEventHandler(_npcSelect_Npc_Selected);
            _npcTreeViewSelect.Npc_Selected += new Npc_SelectedEventHandler(_npcSelect_Npc_Selected);
        }

        void _rbNewModel_Checked(object sender, RoutedEventArgs e)
        {
            ModelEditorService.Instance.IsNpcHD = true;

            ChangeNpcHD();
        }

        void _rbNewModel_Unchecked(object sender, RoutedEventArgs e)
        {
            ModelEditorService.Instance.IsNpcHD = false;

            ChangeNpcHD();
        }

        void _npcSelect_Npc_Selected(object sender, Npc npc)
        {
            string path = Engine.Instance.WowDatabase.GetNpcPath(npc.ModelDisplayId, ModelEditorService.Instance.IsNpcHD);
            M2SceneNode node = ModelSceneService.Instance.SetMainM2SceneNode(path, true);

            if (node == null && ModelEditorService.Instance.IsNpcHD)
            {
                string path2 = Engine.Instance.WowDatabase.GetNpcPath(npc.ModelDisplayId, false);
                node = ModelSceneService.Instance.SetMainM2SceneNode(path2, true);
            }

            if (node != null)
            {
                ModelEditorService.Instance.ShowEditPart(node);
                ModelEditorService.Instance.SetWireFrame(node);
                ModelEditorService.Instance.SheathM2NodeWeapon(node);

                node.UpdateNpc(npc.ModelDisplayId);

                //update property
                if (node.Type == M2Type.MT_CHARACTER)
                {
                    ShellService.Instance.PropertyContent.UpdateCharacterNode(node);

                    if (ShellService.Instance.EquipmentsWindow.IsVisible)
                        ShellService.Instance.EquipmentsWindow.Update();

                    if (ShellService.Instance.EquipmentSelectWindow.IsVisible)
                        ShellService.Instance.EquipmentSelectWindow.Update();
                }
                else
                    ShellService.Instance.PropertyContent.UpdateNpcNode(node);
            }

            Player player = Client.Instance.GetPlayer();
            if (EngineService.Instance.IsGameMode && node != null && (node.Type == M2Type.MT_CHARACTER || node.Type == M2Type.MT_CREATRUE))
            {
                player.SetM2AsTarget(node, 15, (float)(Math.PI / 6.0f));
            }
            else
            {
                player.SetM2AsTarget(null, 15, (float)(Math.PI / 6.0f));
            }
        }

        public void Initialize()
        {
            _npcSelect.FillData();
            _npcPathSelect.FillData();
            _npcTreeViewSelect.FillData(_npcPathSelect.Npcs);
        }

        public void ChangeNpcHD()
        {
            //_npcPathSelect.FillData();
        }
    }
}
