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
using WowModelExplorer.Services;
using WowModelExplorer.Controls;
using WowModelExplorer.Data;
using mywowNet;
using mywowNet.Data;

namespace WowModelExplorer.Views
{
    /// <summary>
    /// Interaction logic for WeaponContent.xaml
    /// </summary>
    public partial class WeaponContent : DockableContent
    {
        public WeaponContent()
        {
            InitializeComponent();

            SubscribeEvents();
        }

        private void SubscribeEvents()
        {
            _itemsDagger.ItemSelected += new Item_SelectedEventHandler(WeaponContent_ItemSelected);
            _itemsShield.ItemSelected += new Item_SelectedEventHandler(WeaponContent_ItemSelected);
            _itemsBow.ItemSelected += new Item_SelectedEventHandler(WeaponContent_ItemSelected);
            _items2Handed.ItemSelected += new Item_SelectedEventHandler(WeaponContent_ItemSelected);
            _itemsLeftHanded.ItemSelected += new Item_SelectedEventHandler(WeaponContent_ItemSelected);
            _itemsRightHanded.ItemSelected += new Item_SelectedEventHandler(WeaponContent_ItemSelected);
            _itemsOffHand.ItemSelected += new Item_SelectedEventHandler(WeaponContent_ItemSelected);
            _itemsGun.ItemSelected += new Item_SelectedEventHandler(WeaponContent_ItemSelected);
        }

        public void Initialize()
        {
            _itemsDagger.FillData(E_ITEM_TYPE.IT_DAGGER);
            _itemsShield.FillData(E_ITEM_TYPE.IT_SHIELD);
            _itemsBow.FillData(E_ITEM_TYPE.IT_BOW);
            _items2Handed.FillData(E_ITEM_TYPE.IT_2HANDED);
            _itemsLeftHanded.FillData(E_ITEM_TYPE.IT_LEFTHANDED);
            _itemsRightHanded.FillData(E_ITEM_TYPE.IT_RIGHTHANDED);
            _itemsOffHand.FillData(E_ITEM_TYPE.IT_OFFHAND);
            _itemsGun.FillData(E_ITEM_TYPE.IT_GUN);
        }

        private void WeaponContent_ItemSelected(object sender, Item item)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null && node.Type == M2Type.MT_CHARACTER && !node.IsNpc)
            {
                M2Appearance appearance = node.M2Appearance;
                if (appearance == null)
                    return;
                appearance.TakeItem(item.Id);
                ModelEditorService.Instance.ShowModelPart(node);
                ModelEditorService.Instance.ShowEditPart(node);
                ModelEditorService.Instance.SetWireFrame(node);
                ModelEditorService.Instance.SheathM2NodeWeapon(node);

                //update property
                ShellService.Instance.PropertyContent.UpdateCharacterNode(node);

                if (ShellService.Instance.EquipmentsWindow.IsVisible)
                    ShellService.Instance.EquipmentsWindow.Update();

                if (ShellService.Instance.EquipmentSelectWindow.IsVisible)
                    ShellService.Instance.EquipmentSelectWindow.Update();
            }
        }
    }
}
