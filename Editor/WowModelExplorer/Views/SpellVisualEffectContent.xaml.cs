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
    /// Interaction logic for SpellVisualEffectContent.xaml
    /// </summary>
    public partial class SpellVisualEffectContent : DockableContent
    {
        public SpellVisualEffectContent()
        {
            InitializeComponent();

            _spellVisualEffectSelect.SpellVisualEffect_Selected += new SpellVisualEffect_SelectedEventHandler(_spellVisualEffectSelect_SpellVisualEffect_Selected);
            _spellVisualEffectTreeViewSelect.SpellVisualEffectTreeView_Selected += new SpellVisualEffect_TreeViewSelectedEventHandler(_spellVisualEffectTreeViewSelect_SpellVisualEffectTreeView_Selected);
        }

        public void Initialize()
        {
            _spellVisualEffectSelect.FillData();
            SpellVisualEffectCollection spells = _spellVisualEffectSelect.SpellVisualEffects;
            _spellVisualEffectTreeViewSelect.FillData(spells);
        }

        void _spellVisualEffectSelect_SpellVisualEffect_Selected(object sender, SpellVisualEffect visual)
        {
            LoadEffectVisual(visual.Id);
        }

        private void LoadEffectVisual(int visualId)
        {
            string path = Engine.Instance.WowDatabase.GetSpellVisualEffectPath(visualId);
            LoadEffectVisual(path);
        }

        void _spellVisualEffectTreeViewSelect_SpellVisualEffectTreeView_Selected(object sender, TreeNodeEntry spellEffect)
        {
            LoadEffectVisual(spellEffect.FullName);
        }

        private void LoadEffectVisual(string path)
        {
            M2SceneNode node = ModelSceneService.Instance.SetMainM2SceneNode(path, true);
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
