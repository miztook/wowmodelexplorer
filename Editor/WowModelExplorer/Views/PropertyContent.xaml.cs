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
using WowModelExplorer.Services;
using WowModelExplorer.Props;
using System.Windows.Controls.WpfPropertyGrid;
using System.ComponentModel;
using mywowNet;
using AvalonDock;

namespace WowModelExplorer.Views
{
    /// <summary>
    /// Interaction logic for PropertyContent.xaml
    /// </summary>
    public partial class PropertyContent : DockableContent
    {
        public PropertyContent()
        {
            InitializeComponent();
        }

        public void Initialize()
        {
            App app = Application.Current as App;
            app.ApplicationLanguageChanged += new Application_LanguageChangedHandler(app_ApplicationLanguageChanged);
        }

        void app_ApplicationLanguageChanged(object sender, Languages current)
        {
            UpdateProperties();
        }

        CharacterProperty characterProperty = new CharacterProperty();
        CharacterPropertyEnglish characterPropertyEnglish = new CharacterPropertyEnglish();
        NpcProperty npcProperty = new NpcProperty();
        NpcPropertyEnglish npcPropertyEnglish = new NpcPropertyEnglish();
        NormalM2Property normalM2Property = new NormalM2Property();
        NormalM2PropertyEnglish normalM2PropertyEnglish = new NormalM2PropertyEnglish();
        WmoProperty wmoProperty = new WmoProperty();
        WmoPropertyEnglish wmoPropertyEnglish = new WmoPropertyEnglish();

        public void UpdateProperties()
        {
            M2SceneNode m2Node = ModelSceneService.Instance.MainM2SceneNode;
            WMOSceneNode wmoNode = ModelSceneService.Instance.MainWMOSceneNode;

            _propertyGrid.SelectedObject = null;

            if (m2Node != null)
            {
                if (m2Node.Type == M2Type.MT_CHARACTER)
                {
                    UpdateCharacterNode(m2Node);
                } 
                else if(m2Node.Type == M2Type.MT_CREATRUE)
                {
                    UpdateNpcNode(m2Node);
                }
                else
                {
                    UpdateNormalM2Node(m2Node);
                }
            }
            else if (wmoNode != null)
            {
                UpdateWmoNode(wmoNode);
            }
        }

        public void UpdateCharacterNode(M2SceneNode node)
        {
            _propertyGrid.SelectedObject = null;

            App app = Application.Current as App;
            if (app.Language == Languages.Chinese)
            {
                characterProperty.FromM2SceneNode(node);
                _propertyGrid.SelectedObject = characterProperty;

                CategoryItem item = _propertyGrid.Categories[CharacterProperty.cateHead];
                if (item != null)
                    item.IsBrowsable = node.HeadSceneNode != null;
                item = _propertyGrid.Categories[CharacterProperty.cateLeftShoulder];
                if (item != null)
                    item.IsBrowsable = node.LeftShoulderSceneNode != null;
                item = _propertyGrid.Categories[CharacterProperty.cateRightShoulder];
                if (item != null)
                    item.IsBrowsable = node.RightShoulderSceneNode != null;
                item = _propertyGrid.Categories[CharacterProperty.cateLeftHand];
                if (item != null)
                    item.IsBrowsable = node.LeftHandSceneNode != null;
                item = _propertyGrid.Categories[CharacterProperty.cateRightHand];
                if (item != null)
                    item.IsBrowsable = node.RightHandSceneNode != null;    
            }
            else
            {
                characterPropertyEnglish.FromM2SceneNode(node);
                _propertyGrid.SelectedObject = characterPropertyEnglish;

                CategoryItem item = _propertyGrid.Categories[CharacterPropertyEnglish.cateHead];
                if (item != null)
                    item.IsBrowsable = node.HeadSceneNode != null;
                item = _propertyGrid.Categories[CharacterPropertyEnglish.cateLeftShoulder];
                if (item != null)
                    item.IsBrowsable = node.LeftShoulderSceneNode != null;
                item = _propertyGrid.Categories[CharacterPropertyEnglish.cateRightShoulder];
                if (item != null)
                    item.IsBrowsable = node.RightShoulderSceneNode != null;
                item = _propertyGrid.Categories[CharacterPropertyEnglish.cateLeftHand];
                if (item != null)
                    item.IsBrowsable = node.LeftHandSceneNode != null;
                item = _propertyGrid.Categories[CharacterPropertyEnglish.cateRightHand];
                if (item != null)
                    item.IsBrowsable = node.RightHandSceneNode != null;    
            }
     
        }

        public void UpdateNpcNode(M2SceneNode node)
        {
            _propertyGrid.SelectedObject = null;

            App app = Application.Current as App;
            if (app.Language == Languages.Chinese)
            {
                npcProperty.FromM2SceneNode(node);
                _propertyGrid.SelectedObject = npcProperty;
            }
            else
            {
                npcPropertyEnglish.FromM2SceneNode(node);
                _propertyGrid.SelectedObject = npcPropertyEnglish;
            }
        }

        public void UpdateNormalM2Node(M2SceneNode node)
        {
            _propertyGrid.SelectedObject = null;

            App app = Application.Current as App;
            if (app.Language == Languages.Chinese)
            {
                normalM2Property.FromM2SceneNode(node);
                _propertyGrid.SelectedObject = normalM2Property;
            }
            else
            {
                normalM2PropertyEnglish.FromM2SceneNode(node);
                _propertyGrid.SelectedObject = normalM2PropertyEnglish;
            }
        }

        public void UpdateWmoNode(WMOSceneNode node)
        {
            _propertyGrid.SelectedObject = null;

            App app = Application.Current as App;
            if (app.Language == Languages.Chinese)
            {
                wmoProperty.FromWMOSceneNode(node);
                _propertyGrid.SelectedObject = wmoProperty;
            }
            else
            {
                wmoPropertyEnglish.FromWMOSceneNode(node);
                _propertyGrid.SelectedObject = wmoPropertyEnglish;
            }
        }
    }
}
