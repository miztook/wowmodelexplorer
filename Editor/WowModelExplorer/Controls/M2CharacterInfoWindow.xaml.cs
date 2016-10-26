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
using WowModelExplorer.Views;
using System.IO;
using Microsoft.Win32;

namespace WowModelExplorer.Controls
{
    /// <summary>
    /// Interaction logic for M2CharacterInfo.xaml
    /// </summary>
    public partial class M2CharacterInfoWindow : Window
    {
        private uint raceId;
        private bool female;
        private string classShortName;
        private SCharFeature charFeature;

        public M2CharacterInfoWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(M2CharacterInfo_Loaded);
            this.Closing += (sender, e) => { e.Cancel = true; Hide(); };
        }

        void M2CharacterInfo_Loaded(object sender, RoutedEventArgs e)
        {
            this.Loaded -= M2CharacterInfo_Loaded;

            App app = Application.Current as App;
            app.ApplicationLanguageChanged += new Application_LanguageChangedHandler(app_ApplicationLanguageChanged);
        }

        void app_ApplicationLanguageChanged(object sender, Languages current)
        {
            Update();
        }

        public void Update()
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node == null || node.Type != M2Type.MT_CHARACTER || node.IsNpc)
            {
                _lblRace.Text = "";
                _lblGender.Text = "";
                _lblClass.Text = "";
                _lblSkinColor.Text = "";
                _lblFaceType.Text = "";
                _lblHairType.Text = "";
                _lblFacialHair.Text = "";
                _lblHairColor.Text = "";
                return;
            }

            CharacterSelect charSelect = ShellService.Instance.CharacterContent.CharacterSelect;
            raceId = charSelect.RaceInfo.RaceId;
            _lblRace.Text = charSelect.GetRaceName(raceId);
            female = charSelect.Female;
            if (female)
                _lblGender.Text = Application.Current.FindResource("female") as string;
            else
                _lblGender.Text = Application.Current.FindResource("male") as string;
            classShortName = charSelect.ClassShortName;
            _lblClass.Text = Engine.Instance.WowDatabase.GetClassName(classShortName);
            CharFeature feature = charSelect.CurrentCharFeature;

            feature.ToSCharFeature(out charFeature);

            _lblSkinColor.Text = charFeature.skinColor.ToString();
            _lblFaceType.Text = charFeature.faceType.ToString();
            _lblHairType.Text = charFeature.hairStyle.ToString();
            _lblFacialHair.Text = charFeature.facialHair.ToString();
            _lblHairColor.Text = charFeature.hairColor.ToString();
        }

        private void btImport_Click(object sender, RoutedEventArgs e)
        {

            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "text|*.txt|All Files|*.*";
            dlg.InitialDirectory = Engine.Instance.GetBaseDirectory();
            bool? ret = dlg.ShowDialog(ShellService.Instance.MainWindow);
            if (ret.HasValue && ret.Value)
            {
                StreamReader reader = new StreamReader(dlg.FileName);

                string txt;
                txt = reader.ReadLine();
                raceId = UInt32.Parse(txt);

                txt = reader.ReadLine();
                female = Int32.Parse(txt) != 0;

                txt = reader.ReadLine();
                classShortName = txt;

                txt = reader.ReadLine();
                charFeature.skinColor = UInt32.Parse(txt);

                txt = reader.ReadLine();
                charFeature.faceType = UInt32.Parse(txt);

                txt = reader.ReadLine();
                charFeature.hairStyle = UInt32.Parse(txt);

                txt = reader.ReadLine();
                charFeature.facialHair = UInt32.Parse(txt);

                txt = reader.ReadLine();
                charFeature.hairColor = UInt32.Parse(txt);

                reader.Dispose();

                if (LoadCharacter())
                {
                    CharacterSelect charSelect = ShellService.Instance.CharacterContent.CharacterSelect;
                    charSelect.UpdateUI(raceId, female, classShortName);

                    charSelect.UseStartOutfit(classShortName);

                    Update();
                }    
            }
        }

        private void btExport_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "text|*.txt|All Files|*.*";
            dlg.InitialDirectory = Engine.Instance.GetBaseDirectory();
            bool? ret = dlg.ShowDialog(ShellService.Instance.MainWindow);
            if (ret.HasValue && ret.Value)
            {
                StreamWriter writer = new StreamWriter(dlg.FileName);

                writer.WriteLine(raceId);
                writer.WriteLine(female ? 1 : 0);
                writer.WriteLine(classShortName);
                writer.WriteLine(charFeature.skinColor);
                writer.WriteLine(charFeature.faceType);
                writer.WriteLine(charFeature.hairStyle);
                writer.WriteLine(charFeature.facialHair);
                writer.WriteLine(charFeature.hairColor);

                writer.Dispose();
            }
        }

        private void btCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }

        private bool LoadCharacter()
        {
            string shortname = Engine.Instance.WowDatabase.GetRaceName(raceId);
            WowDatabase _wowDatabase = Engine.Instance.WowDatabase;
            string path = _wowDatabase.GetCharacterPath(shortname, female, true);
            M2SceneNode node = ModelSceneService.Instance.SetMainM2SceneNode(path, false);

            if (node == null)
            {
                path = _wowDatabase.GetCharacterPath(shortname, female, true);
                node = ModelSceneService.Instance.SetMainM2SceneNode(path, false);
            }

            if (node != null)
                node.M2Instance.CharFeature = charFeature;

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
