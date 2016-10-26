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
using WowModelExplorer.Services;
using mywowNet;

namespace WowModelExplorer.Controls
{
    /// <summary>
    /// Interaction logic for M2ArmoryWindow.xaml
    /// </summary>
    public partial class M2ArmoryWindow : Window
    {
        const string filename = @"character.json";

        public M2ArmoryWindow()
        {
            InitializeComponent();
            this.Closing += (sender, e) => { e.Cancel = true; Hide(); };
        }

        SCharacterArmoryInfo charInfo = new SCharacterArmoryInfo();

        public void Update()
        {
            if (mwTool.Instance.ParseCharacterArmoryInfo(filename, out charInfo))
            {
                CharacterSelect charSelect = ShellService.Instance.CharacterContent.CharacterSelect;

                _lblName.Text = charInfo.Name;
                _lblRace.Text = charSelect.GetRaceName(charInfo.Race);
                if (charInfo.Gender != 0)
                    _lblGender.Text = Application.Current.FindResource("female") as string;
                else
                    _lblGender.Text = Application.Current.FindResource("male") as string;
                _lblClass.Text = Engine.Instance.WowDatabase.GetClassName(charInfo.ClassShortName);
            }
            else
            {
                charInfo.Name = "";

                _lblName.Text = charInfo.Name;
                _lblRace.Text = "";
                _lblGender.Text = "";
                _lblClass.Text = "";
            }
        }

        private void btParse_Click(object sender, RoutedEventArgs e)
        {
            if (mwTool.Instance.OpenUrlToJsonFile(tbUrlAddress.Text, filename))
            {
                Update();
            }       
        }

        private void btLoad_Click(object sender, RoutedEventArgs e)
        {
            if (charInfo.Name == "")
                return;

            if (LoadCharacter())
            {
                CharacterSelect charSelect = ShellService.Instance.CharacterContent.CharacterSelect;
                charSelect.UpdateUI(charInfo.Race, charInfo.Gender != 0, charInfo.ClassShortName);
            }
        }

        private void btCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }

        private bool LoadCharacter()
        {
            string shortname = Engine.Instance.WowDatabase.GetRaceName(charInfo.Race);
            WowDatabase _wowDatabase = Engine.Instance.WowDatabase;
            string path = _wowDatabase.GetCharacterPath(shortname, charInfo.Gender != 0, true);
            M2SceneNode node = ModelSceneService.Instance.SetMainM2SceneNode(path, false);

            if (node == null)
            {
                path = _wowDatabase.GetCharacterPath(shortname, charInfo.Gender != 0, false);
                node = ModelSceneService.Instance.SetMainM2SceneNode(path, false);
            }

            if (node != null)
            {
                SCharFeature charFeature = new SCharFeature();
                charFeature.skinColor = charInfo.SkinColor;
                charFeature.hairColor = charInfo.HairColor;
                charFeature.hairStyle = charInfo.HairStyle;
                charFeature.faceType = charInfo.FaceType;
                charFeature.facialHair = charInfo.FacialHair;

                node.M2Instance.CharFeature = charFeature;
            }

            //items
            if (node != null)
            {
                 M2Appearance appearance = node.M2Appearance;
                 if (appearance != null)
                 {
                     appearance.TakeItem(charInfo.Head);
                     appearance.TakeItem(charInfo.Shoulder);
                     appearance.TakeItem(charInfo.Boots);
                     appearance.TakeItem(charInfo.Belt);
                     appearance.TakeItem(charInfo.Shirt);
                     appearance.TakeItem(charInfo.Pants);
                     appearance.TakeItem(charInfo.Chest);
                     appearance.TakeItem(charInfo.Bracers);
                     appearance.TakeItem(charInfo.Gloves);
                     appearance.TakeItem(charInfo.HandRight);
                     appearance.TakeItem(charInfo.HandLeft);
                     appearance.TakeItem(charInfo.Cape);
                     appearance.TakeItem(charInfo.Tabard);
                 }

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

            Player player = Client.Instance.GetPlayer();
            if (EngineService.Instance.IsGameMode && node != null &&
                (node.Type == M2Type.MT_CHARACTER || node.Type == M2Type.MT_CREATRUE))
            {
                player.SetM2AsTarget(node, 15, (float)(Math.PI / 6.0f));
            }
            else
            {
                player.SetM2AsTarget(null, 15, (float)(Math.PI / 6.0f));
            }

            return node != null;
        }
    }
}
