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
    public delegate void Wmo_TreeViewSelectedEventHandler(object sender, string wmofilename);
    
    /// <summary>
    /// Interaction logic for WmoTreeViewSelect.xaml
    /// </summary>
    public partial class WmoTreeViewSelect : UserControl
    {
        private ICollectionView m2NamesView;

        public event WorldM2_SelectedEventHandler M2Model_Selected;

        public WmoTreeViewSelect()
        {
            InitializeComponent();

            _TreeView.AddHandler(TreeViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(treeViewItemDbClicked));
            _TreeView.KeyUp += new KeyEventHandler(treeView_KeyUp);

            this.PreviewKeyDown += new KeyEventHandler(filter_PreviewKeyDown);

            _m2ListView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(m2ListViewItemDbClicked));
            _m2ListView.KeyUp += new KeyEventHandler(m2ListView_KeyUp);

            _m2Grid.Visibility = Visibility.Collapsed;
        }

        void filter_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (_m2filter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (!_m2filter.IsKeyboardFocused && e.Key == Key.W)
                e.Handled = true;
        }

        public TreeEntryCollection TreeViewFiles
        {
            get { return this._mainGrid.Resources["_wmoTreeViewCollection"] as TreeEntryCollection; }
        }

        public void FillData()
        {
            GetWmoTreeItemFiles("", new TreeNodeEntry());

            m2NamesView = CollectionViewSource.GetDefaultView(M2Names);
        }

        private void GetWmoTreeItemFiles(string path, TreeNodeEntry node)
        {
            WowEnvironment env = Engine.Instance.WowEnvironment;
            env.ClearOwnCascFiles();
            WowDatabase database = Engine.Instance.WowDatabase;
            for (uint i = 0; i < database.WmoCount; ++i)
            {
                env.AddOwnCascFile(database.GetWMOFileName(i, false));
            }
            env.FinishOwnCascFiles();

            TreeViewFiles.Clear();
            TreeNodeEntry treeNode = TreeViewEntry.TraverseAllUseOwn(path, "*", node);
            for (int i = 0; i < treeNode.NodeEntrys.Count; i++)
            {
                TreeViewFiles.Add(treeNode.NodeEntrys[i]);
            }
        }

        public event Wmo_TreeViewSelectedEventHandler WmoTreeView_Selected;

        private void treeView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadWmo();
        }

        public void treeViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadWmo();
        }

        private void LoadWmo()
        {
            TreeNodeEntry entry = _TreeView.SelectedItem as TreeNodeEntry;
            if (entry != null)
            {
                if (WmoTreeView_Selected != null)
                    WmoTreeView_Selected.Invoke(this, entry.FullName);
            }
        }

        public void UpdateDoodadM2Names(WMOSceneNode node)
        {
            M2Names.Clear();

            if (node == null)
            {
                _m2Grid.Visibility = Visibility.Collapsed;
                return;
            }

            uint num = node.FileWMO.numDoodads;
            for (int i = 0; i < (int)num; ++i)
            {
                string name = node.GetDoodadName(i);
                bool bFind = false;
                for (int k = 0; k < M2Names.Count; ++k)
                {
                    if (M2Names[k].Name == name)
                    {
                        bFind = true;
                        break;
                    }
                }

                if (!bFind && name.Length > 0)
                    M2Names.Add(new Entry() { Name = name, Id = (int)i });
            }

            _m2Grid.Visibility = M2Names.Count > 0 ? Visibility.Visible : Visibility.Collapsed;
        }

        public EntryCollection M2Names
        {
            get { return _m2Grid.Resources["_m2Names"] as EntryCollection; }
        }

        private void m2filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (m2NamesView != null)
                m2NamesView.Filter = new System.Predicate<object>(IsM2Match);
        }

        private bool IsM2Match(object entry)
        {
            Entry i = entry as Entry;
            return -1 != i.Name.IndexOf(_m2filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void m2ListViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadM2Model();
        }

        private void m2ListView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadM2Model();
        }

        private void LoadM2Model()
        {
            Entry entry = _m2ListView.SelectedItem as Entry;
            if (entry != null)
            {
                if (M2Model_Selected != null)
                    M2Model_Selected.Invoke(this, entry.Name);
            }
        }
    }
}
