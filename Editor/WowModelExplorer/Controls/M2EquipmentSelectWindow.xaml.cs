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
using System.Windows.Shapes;
using mywowNet;
using mywowNet.Data;
using System.ComponentModel;
using WowModelExplorer.Data;
using WowModelExplorer.Services;

namespace WowModelExplorer.Controls
{
    /// <summary>
    /// Interaction logic for M2EuipmentSelectWindow.xaml
    /// </summary>
    public partial class M2EquipmentSelectWindow : Window
    {
        public M2EquipmentSelectWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(ModelEquipmentSelect_Loaded);
            this.Closing += (sender, e) => { e.Cancel = true; Hide(); };

            _listView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(listViewItemDbClicked));
            _listView.KeyUp += new KeyEventHandler(listView_KeyUp);
        }

        void ModelEquipmentSelect_Loaded(object sender, RoutedEventArgs e)
        {
            this.Loaded -= ModelEquipmentSelect_Loaded;

            App app = Application.Current as App;
            app.ApplicationLanguageChanged += new Application_LanguageChangedHandler(app_ApplicationLanguageChanged);

            ModelSceneService.Instance.MainM2SceneNodeChanged += new MainM2SceneNode_ChangedHandler(Instance_MainM2SceneNodeChanged);
        }

        public SlotItemCollection SlotItems
        {
            get { return _mainGrid.Resources["_slotItems"] as SlotItemCollection; }
        }

        void Instance_MainM2SceneNodeChanged(object sender, M2SceneNode node)
        {
            Update();
        }

        void app_ApplicationLanguageChanged(object sender, Languages current)
        {
            Update();
        }

        public void Update()
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node == null)
                return;

            SlotItems.GetAllSlotItemsWithSameName(node);
        }

        private void listViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadItem();
        }

        private void listView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadItem();
        }

        private void LoadItem()
        {
            SlotItem item = _listView.SelectedItem as SlotItem;
            if (item == null)
                return;

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null && node.Type == M2Type.MT_CHARACTER && !node.IsNpc)
            {
                M2Appearance appearance = node.M2Appearance;
                if (appearance == null)
                    return;
                appearance.TakeItem(item.ItemId);

                ModelEditorService.Instance.ShowModelPart(node);
                ModelEditorService.Instance.ShowEditPart(node);
                ModelEditorService.Instance.SetWireFrame(node);
                ModelEditorService.Instance.SheathM2NodeWeapon(node);

                int slot = node.M2Instance.GetItemSlot(item.ItemId);

                if (node.M2Instance.SlotHasModel((E_CHAR_SLOTS)slot))
                {
                    if (slot == (int)E_CHAR_SLOTS.CS_HEAD)
                    {
                        node.BuildVisibleGeosets();
                    }
                    else if (slot == (int)E_CHAR_SLOTS.CS_BELT)
                    {
                        node.UpdateCharacter();
                    }
                }
                else
                {
                    node.UpdateCharacter();
                }

                //update property
                ShellService.Instance.PropertyContent.UpdateCharacterNode(node);

                if (ShellService.Instance.EquipmentsWindow.IsVisible)
                    ShellService.Instance.EquipmentsWindow.Update();
            }
        }

        private void btOk_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }
    }
}
