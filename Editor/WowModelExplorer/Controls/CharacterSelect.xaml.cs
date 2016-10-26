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
using WowModelExplorer.Data;
using mywowNet;
using mywowNet.Data;
using WowModelExplorer.Services;
using System.ComponentModel;

namespace WowModelExplorer.Controls
{
    public delegate void StartOutfitUpdatedHandler(object sender, M2SceneNode node);

    /// <summary>
    /// Interaction logic for CharacterSelect.xaml
    /// </summary>
    public partial class CharacterSelect : UserControl
    {
        private RaceInfo raceInfo;
        private bool female = false;
        private string classShortName;

        public event SelectionChangedEventHandler lbClassSelectionChanged;
        public event StartOutfitUpdatedHandler StartOutfitUpdated;

        public CharacterSelect()
        {
            InitializeComponent();

            lbClassSelectionChanged = new SelectionChangedEventHandler(_lbClasses_SelectionChanged);
            SubscribeEvents();
        }

        public RaceInfo RaceInfo
        {
            get { return raceInfo; }
        }

        public bool Female
        {
            get { return female; }
        }

        public string ClassShortName
        {
            get { return classShortName; }
        }

        private void CharacterSelect_Loaded(object sender, RoutedEventArgs e)
        {
            if (_rbMale.IsChecked.Value == false && _rbFemale.IsChecked.Value == false)
                _rbMale.IsChecked = true;

            if (_rbOld.IsChecked.Value == false && _rbNew.IsChecked.Value == false)
                _rbNew.IsChecked = ModelEditorService.Instance.IsCharacterHD;
        }

        private void SubscribeEvents()
        {
            Loaded += new RoutedEventHandler(CharacterSelect_Loaded);

            _lbAlliance.AddHandler(ListBoxItem.MouseDoubleClickEvent, new MouseButtonEventHandler(_lbAlliance_Selected));
            _lbAlliance.SelectionChanged += (sender, e) => 
                { 
                    if (_lbAlliance.SelectedIndex != -1)
                        _lbHorde.SelectedIndex = -1;
                };
            _lbAlliance.PreviewKeyDown += (sender, e) => { e.Handled = true; };
            _lbAlliance.PreviewKeyUp += (sender, e) => { e.Handled = true; };

            _lbHorde.AddHandler(ListBoxItem.MouseDoubleClickEvent, new MouseButtonEventHandler(_lbHorde_Selected));
            _lbHorde.SelectionChanged += (sender, e) =>
                {
                    if (_lbHorde.SelectedIndex != -1)
                        _lbAlliance.SelectedIndex = -1;
                };
            _lbHorde.PreviewKeyDown += (sender, e) => { e.Handled = true; };
            _lbHorde.PreviewKeyUp += (sender, e) => { e.Handled = true; };

            _rbFemale.Checked += _rbFemale_Checked;
            _rbFemale.Unchecked += _rbFemale_Unchecked;

            _rbNew.Checked += _rbNewModel_Checked;
            _rbNew.Unchecked += _rbNewModel_Unchecked;
        }

        void _lbClasses_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            StartOutfitClass outfit = _lbClasses.SelectedItem as StartOutfitClass;
            if (outfit != null)
            {
                classShortName = outfit.ShortName;

                UseStartOutfit(classShortName);
            }
        }

        void _rbFemale_Checked(object sender, RoutedEventArgs e)
        {
            female = true;

            ChangeCharacter();
        }

        void _rbFemale_Unchecked(object sender, RoutedEventArgs e)
        {
            female = false;

            ChangeCharacter();
        }

        void _rbNewModel_Checked(object sender, RoutedEventArgs e)
        {
            ModelEditorService.Instance.IsCharacterHD = true;

            ChangeCharacter();
        }

        void _rbNewModel_Unchecked(object sender, RoutedEventArgs e)
        {
            ModelEditorService.Instance.IsCharacterHD = false;

            ChangeCharacter();
        }

        private void _lbAlliance_Selected(object sender, MouseButtonEventArgs e)
        {
            if (_lbAlliance.SelectedIndex != -1)
            {
                raceInfo = _lbAlliance.SelectedItem as RaceInfo;

                ChangeCharacter();
            }
        }

        private void _lbHorde_Selected(object sender, MouseButtonEventArgs e)
        {
            if (_lbHorde.SelectedIndex != -1)
            {
                raceInfo = _lbHorde.SelectedItem as RaceInfo;

                ChangeCharacter();
            }
        }

        private void ChangeCharacter()
        {
            if (raceInfo == null)
                return;

            WowDatabase _wowDatabase = Engine.Instance.WowDatabase;
            string path = _wowDatabase.GetCharacterPath(raceInfo.ShortName, female, ModelEditorService.Instance.IsCharacterHD);
            M2SceneNode node = ModelSceneService.Instance.SetMainM2SceneNode(path, false);

            if (node == null && ModelEditorService.Instance.IsCharacterHD)
            {
                path = _wowDatabase.GetCharacterPath(raceInfo.ShortName, female, false);
                node = ModelSceneService.Instance.SetMainM2SceneNode(path, false);
            }
             UpdateMaxCharFeature();
             UpdateCurrentCharFeature();
             UpdateStartOutfitClasses();
             SelectClass(classShortName, true);

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

        private void UpdateStartOutfitClasses()
        {
            _lbClasses.SelectionChanged -= lbClassSelectionChanged;

            StartOutfitClasses.GetStartOutfitClasses(raceInfo.RaceId, female);

            _lbClasses.SelectionChanged += lbClassSelectionChanged;
        }

        private StartOutfitClass SelectClass(string shortname, bool updateModel)
        {
            if (!updateModel)
                _lbClasses.SelectionChanged -= lbClassSelectionChanged;

            foreach (object item in _lbClasses.Items)
            {
                StartOutfitClass outfit = item as StartOutfitClass;
                if (outfit.ShortName == shortname)
                {
                    _lbClasses.SelectedItem = item;

                    if (!updateModel)
                        _lbClasses.SelectionChanged += lbClassSelectionChanged;

                    return outfit;
                }
            }

            _lbClasses.SelectedIndex = 0;

            if (!updateModel)
                _lbClasses.SelectionChanged += lbClassSelectionChanged;

            return _lbClasses.SelectedItem as StartOutfitClass;
        }

        private void UpdateMaxCharFeature()
        {
            MaxCharFeature.GetMaxCharFeature(raceInfo.RaceId, female, ModelEditorService.Instance.IsCharacterHD);
        }

        private void UpdateCurrentCharFeature()
        {
            CurrentCharFeature.PropertyChanged -= new PropertyChangedEventHandler(CurrentCharFeature_PropertyChanged);

            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null && node.Type == M2Type.MT_CHARACTER && !node.IsNpc)
            {
                CurrentCharFeature.FromSCharFeature(node.M2Instance.CharFeature);
            }

            CurrentCharFeature.PropertyChanged += new PropertyChangedEventHandler(CurrentCharFeature_PropertyChanged);
        }

        public void UseStartOutfit(string shortname)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null && node.Type == M2Type.MT_CHARACTER && !node.IsNpc)
            {
                M2Appearance appearance = node.M2Appearance;
                if (appearance == null)
                    return;

                int startOutfitId = GetStartOutfitId(shortname);
                appearance.LoadStartOutfitId(startOutfitId, shortname == "DEATHKNIGHT");
 
                 ModelEditorService.Instance.ShowModelPart(node);
                 ModelEditorService.Instance.ShowEditPart(node);
                 ModelEditorService.Instance.SetWireFrame(node);
                 ModelEditorService.Instance.SheathM2NodeWeapon(node);

                 node.UpdateCharacter();

                if (StartOutfitUpdated != null)
                {
                    StartOutfitUpdated.Invoke(this, node);
                }
            }
        }

        void CurrentCharFeature_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (node != null && node.Type == M2Type.MT_CHARACTER && !node.IsNpc)
            {
                SCharFeature schar = new SCharFeature();
                schar.skinColor = CurrentCharFeature.SkinColor;
                schar.faceType = CurrentCharFeature.FaceType;
                schar.hairStyle = CurrentCharFeature.HairStyle;
                schar.hairColor = CurrentCharFeature.HairColor;
                schar.facialHair = CurrentCharFeature.FacialHair;

                node.M2Instance.CharFeature = schar;

                ModelEditorService.Instance.ShowModelPart(node);
                ModelEditorService.Instance.ShowEditPart(node);
                ModelEditorService.Instance.SetWireFrame(node);
                ModelEditorService.Instance.SheathM2NodeWeapon(node);

                node.UpdateCharacter();
            }
        }

        public RaceInfoCollection AllianceRaceInfos 
        {
            get { return _mainGrid.Resources["_allianceRaces"] as RaceInfoCollection;  }
        }

        public RaceInfoCollection HordeRaceInfos
        {
            get { return _mainGrid.Resources["_hordeRaces"] as RaceInfoCollection; }
        }

        public StartOutfitClassCollection StartOutfitClasses
        {
            get { return _mainGrid.Resources["_startOutfitClasses"] as StartOutfitClassCollection;  }
        }

        public CharFeature MaxCharFeature
        {
            get { return _mainGrid.Resources["_maxCharFeature"] as CharFeature; }
        }

        public CharFeature CurrentCharFeature
        {
            get { return _mainGrid.Resources["_currentCharFeature"] as CharFeature; }
        }

        public string GetRaceName(uint raceid)
        {
            foreach (var v in AllianceRaceInfos)
            {
                if (v.RaceId == raceid)
                    return v.Name;
            }

            foreach (var v in HordeRaceInfos)
            {
                if (v.RaceId == raceid)
                    return v.Name;
            }

            return "";
        }

        public int GetStartOutfitId(string shortname)
        {
            foreach (var v in StartOutfitClasses)
            {
                if (v.ShortName == shortname)
                    return v.Id;
            }
            return 0;
        }

        public bool UpdateUI(uint raceid, bool female, string classname)
        {
            foreach (object item in _lbAlliance.Items)
            {
                RaceInfo r = item as RaceInfo;
                if (r.RaceId == raceid)
                {
                    this.raceInfo = r;
                    _lbAlliance.SelectedItem = item;
                    break;
                }
            }

            foreach (object item in _lbHorde.Items)
            {
                RaceInfo r = item as RaceInfo;
                if (r.RaceId == raceid)
                {
                    this.raceInfo = r;
                    _lbHorde.SelectedItem = item;
                    break;
                }
            }

            this.female = female;

            _rbFemale.Checked -= _rbFemale_Checked;
            _rbFemale.Unchecked -= _rbFemale_Unchecked;
            _rbFemale.IsChecked = this.female;
            _rbMale.IsChecked = !this.female;
            _rbFemale.Checked += _rbFemale_Checked;
            _rbFemale.Unchecked += _rbFemale_Unchecked;

            _rbNew.Checked -= _rbNewModel_Checked;
            _rbNew.Unchecked -= _rbNewModel_Unchecked;
            _rbOld.IsChecked = !ModelEditorService.Instance.IsCharacterHD;
            _rbNew.IsChecked = ModelEditorService.Instance.IsCharacterHD;
            _rbNew.Checked += _rbNewModel_Checked;
            _rbNew.Unchecked += _rbNewModel_Unchecked;

            classShortName = classname;

            UpdateMaxCharFeature();
            UpdateCurrentCharFeature();
            UpdateStartOutfitClasses();
            SelectClass(classShortName, false);

            return true;
        }
    }
}
