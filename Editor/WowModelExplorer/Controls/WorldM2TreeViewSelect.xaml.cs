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
    public delegate void WorldM2_TreeViewSelectedEventHandler(object sender, string m2filename);

    /// <summary>
    /// Interaction logic for WorldM2TreeViewSelect.xaml
    /// </summary>
    public partial class WorldM2TreeViewSelect : UserControl
    {
        public event WorldM2_TreeViewSelectedEventHandler M2ModelTreeView_Selected;

        public WorldM2TreeViewSelect()
        {
            InitializeComponent();

            _TreeView.AddHandler(TreeViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(treeViewItemDbClicked));
            _TreeView.KeyUp += new KeyEventHandler(treeView_KeyUp);
        }

        public TreeEntryCollection TreeViewFiles
        {
            get { return this._mainGrid.Resources["_m2TreeViewCollection"] as TreeEntryCollection; }
        }

        public void FillData()
        {
            GetWorldM2TreeItemFiles("", new TreeNodeEntry());
        }

        private void GetWorldM2TreeItemFiles(string path, TreeNodeEntry node)
        {
            WowEnvironment env = Engine.Instance.WowEnvironment;
            env.ClearOwnCascFiles();
            WowDatabase database = Engine.Instance.WowDatabase;
            for (uint i = 0; i < database.WorldModelCount; ++i)
            {
                env.AddOwnCascFile(database.GetWorldModelName(i, false));
            }
            env.FinishOwnCascFiles();

            TreeViewFiles.Clear();
            TreeNodeEntry treeNode = TreeViewEntry.TraverseAllUseOwn(path, "*", node);
            for (int i = 0; i < treeNode.NodeEntrys.Count; i++)
            {
                TreeViewFiles.Add(treeNode.NodeEntrys[i]);
            }
        }

        private void treeView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadM2Model();
        }

        public void treeViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadM2Model();
        }

        private void LoadM2Model()
        {
            TreeNodeEntry entry = _TreeView.SelectedItem as TreeNodeEntry;
            if (entry != null)
            {
                if (M2ModelTreeView_Selected != null)
                    M2ModelTreeView_Selected.Invoke(this, entry.FullName);
            }
        }
    }
}
