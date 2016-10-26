using System;
using System.Collections.Generic;
using System.Linq;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using WowModelExplorer.Data;
using WowModelExplorer.Services;
using mywowNet.Data;

namespace WowModelExplorer.Controls
{
    /// <summary>
    /// Interaction logic for NpcNameSelect.xaml
    /// </summary>
    public partial class NpcPathSelect : UserControl
    {
        private ICollectionView npcsView;

        public NpcPathSelect()
        {
            InitializeComponent();
            _listView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(listViewItemDbClicked));
            _listView.KeyUp += new KeyEventHandler(listView_KeyUp);

            this.PreviewKeyDown += new KeyEventHandler(filter_PreviewKeyDown);
        }

        void filter_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (filter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (!filter.IsKeyboardFocused && e.Key == Key.W)
                e.Handled = true;
        }

        public void FillData()
        {
            Npcs.GetAllNpcs(ModelEditorService.Instance.IsNpcHD, true);
            npcsView = CollectionViewSource.GetDefaultView(Npcs);
        }

        public NpcCollection Npcs
        {
            get { return _mainGrid.Resources["_npcs"] as NpcCollection; }
        }

        private bool IsMatch(object npc)
        {
            Npc i = npc as Npc;
            return -1 != i.Path.IndexOf(filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            FilterNpc();
        }

        private void FilterNpc()
        {
            if (npcsView == null)
                return;

            npcsView.Filter = new System.Predicate<object>(IsMatch);
        }

        bool pathAscending = true;
        bool idAscending = true;
        bool modelIdAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (npcsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            npcsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Path":
                    {
                        npcsView.SortDescriptions.Add(new SortDescription("Path",
                                                pathAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        pathAscending = !pathAscending;
                    }
                    break;
                case "ModelDisplayId":
                    {
                        npcsView.SortDescriptions.Add(new SortDescription("ModelDisplayId",
                                               idAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        idAscending = !idAscending;
                    }
                    break;
                case "ModelId":
                    {
                        npcsView.SortDescriptions.Add(new SortDescription("ModelId",
                                               modelIdAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        modelIdAscending = !modelIdAscending;
                    }
                    break;
            }

        }

        public event Npc_SelectedEventHandler Npc_Selected;

        private void listViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadNpc();
        }

        private void listView_KeyUp(object sender, KeyEventArgs e)
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
