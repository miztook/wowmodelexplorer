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
    public delegate void Npc_TreeViewSelectedEventHandler(object sender, string npcfilename);

    /// <summary>
    /// Interaction logic for NpcTreeViewSelect.xaml
    /// </summary>
    public partial class NpcTreeViewSelect : UserControl
    {
        Dictionary<string, List<Npc>> m_NpcNameMap = new Dictionary<string,List<Npc>>();
        private ICollectionView npcsView;

        public NpcTreeViewSelect()
        {
            InitializeComponent();

            _TreeView.AddHandler(TreeViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(treeViewItemDbClicked));
            _TreeView.KeyUp += new KeyEventHandler(treeView_KeyUp);

            _listView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(npcListViewItemDbClicked));
            _listView.KeyUp += new KeyEventHandler(npcListView_KeyUp);
        }

        public TreeEntryCollection TreeViewFiles
        {
            get { return this._treeGrid.Resources["_npcTreeViewCollection"] as TreeEntryCollection; }
        }

        public NpcCollection Npcs
        {
            get { return _npcGrid.Resources["_npcs"] as NpcCollection; }
        } 

        public void FillData(NpcCollection npcCollection)
        {
            npcsView = CollectionViewSource.GetDefaultView(Npcs);

            GetNpcTreeItemFiles(npcCollection, "", new TreeNodeEntry());

            //build string to npclist map
            m_NpcNameMap.Clear();
            foreach (Npc npc in npcCollection)
            {
                List<Npc> listNpc;
                if (!m_NpcNameMap.TryGetValue(npc.Path, out listNpc))
                    m_NpcNameMap[npc.Path] = new List<Npc>();

                m_NpcNameMap[npc.Path].Add(npc);
            }
        }

        private void GetNpcTreeItemFiles(NpcCollection npcCollection, string path, TreeNodeEntry node)
        {
            WowEnvironment env = Engine.Instance.WowEnvironment;
            env.ClearOwnCascFiles();
            foreach (Npc npc in npcCollection)
            {
                env.AddOwnCascFile(npc.Path);
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
                LoadNpcCollection();
        }

        public void treeViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadNpcCollection();
        }

        private void LoadNpcCollection()
        {
            TreeNodeEntry entry = _TreeView.SelectedItem as TreeNodeEntry;
            if (entry != null)
            {
                Npcs.Clear();
                List<Npc> listNpc = new List<Npc>();
                if (m_NpcNameMap.TryGetValue(entry.FullName, out listNpc))
                {
                    foreach (Npc npc in listNpc)
                    {
                        Npcs.Add(npc);
                    }
                }

                if (Npcs.Count > 0)
                {
                   _listView.SelectedItem = Npcs[0];
                   LoadNpc();
                }
            }
        }

        bool nameAscending = true;
        bool idAscending = true;
        bool typeAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (npcsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            npcsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Name":
                    {
                        npcsView.SortDescriptions.Add(new SortDescription("Name",
                                                nameAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        nameAscending = !nameAscending;
                    }
                    break;
                case "Id":
                    {
                        npcsView.SortDescriptions.Add(new SortDescription("Id",
                                               idAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        idAscending = !idAscending;
                    }
                    break;
                case "Type":
                    {
                        npcsView.SortDescriptions.Add(new SortDescription("Type",
                                               typeAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        typeAscending = !typeAscending;
                    }
                    break;
            }
        }

        public event Npc_SelectedEventHandler Npc_Selected;

        private void npcListViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadNpc();
        }

        private void npcListView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadNpc();
        }

        private void LoadNpc()
        {
            Npc npc = _listView.SelectedItem as Npc;
            if (npc != null)
            {
                if (Npc_Selected != null)
                    Npc_Selected.Invoke(this, npc);
            }
        }

    }
}
