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
    /// Interaction logic for SetsContent.xaml
    /// </summary>
    public partial class SetContent : DockableContent
    {
        public SetContent()
        {
            InitializeComponent();

            _setSelect.Set_Selected += new Set_SelectedEventHandler(_setSelect_Set_Selected);
        }

        void _setSelect_Set_Selected(object sender, Set set)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null && node.Type == M2Type.MT_CHARACTER && !node.IsNpc)
            {
                M2Appearance appearance = node.M2Appearance;
                if (appearance == null)
                    return;

                appearance.LoadSet(set.Id);
                
                ModelEditorService.Instance.ShowModelPart(node);
                ModelEditorService.Instance.ShowEditPart(node);
                ModelEditorService.Instance.SetWireFrame(node);
                ModelEditorService.Instance.SheathM2NodeWeapon(node);

                node.UpdateCharacter();

                //update property
                ShellService.Instance.PropertyContent.UpdateCharacterNode(node);

                if (ShellService.Instance.EquipmentsWindow.IsVisible)
                    ShellService.Instance.EquipmentsWindow.Update();

                if (ShellService.Instance.EquipmentSelectWindow.IsVisible)
                    ShellService.Instance.EquipmentSelectWindow.Update();
            }
        }

        public void Initialize()
        {
            _setSelect.FillData();
        }
    }
}
