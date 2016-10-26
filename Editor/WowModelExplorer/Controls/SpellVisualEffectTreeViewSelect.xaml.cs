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
using System.ComponentModel;
using mywowNet.Data;
using WowModelExplorer.Services;
using WowModelExplorer.Data;
using mywowNet;

namespace WowModelExplorer.Controls
{
    public delegate void SpellVisualEffect_TreeViewSelectedEventHandler(object sender, TreeNodeEntry spellEffect);
    
    /// <summary>
    /// Interaction logic for SpellVisualEffectTreeViewSelect.xaml
    /// </summary>
    public partial class SpellVisualEffectTreeViewSelect : UserControl
    {
        private ICollectionView spellVisualEffectsView;

        public SpellVisualEffectTreeViewSelect()
        {
            InitializeComponent();

            _TreeView.AddHandler(TreeViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(treeViewItemDbClicked));
            _TreeView.KeyUp += new KeyEventHandler(treeView_KeyUp);
        }

        public TreeEntryCollection TreeViewFiles
        {
            get { return this._mainGrid.Resources["_spellVisualEffectTreeViewCollection"] as TreeEntryCollection; }
        }

        public void FillData(SpellVisualEffectCollection spellVisualEffects)
        {
            GetSpellTreeItemFiles(spellVisualEffects, "", new TreeNodeEntry());
            spellVisualEffectsView = CollectionViewSource.GetDefaultView(TreeViewFiles);

            _TreeView.ExpandAll();
        }

        private void GetSpellTreeItemFiles(SpellVisualEffectCollection spellVisualEffects, string path, TreeNodeEntry node)
        {
            WowEnvironment env = Engine.Instance.WowEnvironment;
            env.ClearOwnCascFiles();
            foreach(SpellVisualEffect spell in spellVisualEffects)
            {
                string s = Engine.Instance.WowDatabase.GetSpellVisualEffectPath(spell.Id);
                env.AddOwnCascFile(s);
            }
            env.FinishOwnCascFiles();

            TreeViewFiles.Clear();
            TreeNodeEntry treeNode = TreeViewEntry.TraverseAllUseOwn(path, "*", node);
            for (int i = 0; i < treeNode.NodeEntrys.Count; i++)
            {
                TreeViewFiles.Add(treeNode.NodeEntrys[i]);
            }
        }

        public event SpellVisualEffect_TreeViewSelectedEventHandler SpellVisualEffectTreeView_Selected;

        private void treeView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadSpellVisualEffect();
        }

        public void treeViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadSpellVisualEffect();
        }

        private void LoadSpellVisualEffect()
        {
            TreeNodeEntry entry = _TreeView.SelectedItem as TreeNodeEntry;
            if (entry != null)
            {
                if (SpellVisualEffectTreeView_Selected != null)
                    SpellVisualEffectTreeView_Selected.Invoke(this, entry);
            }
        }
    }
}
