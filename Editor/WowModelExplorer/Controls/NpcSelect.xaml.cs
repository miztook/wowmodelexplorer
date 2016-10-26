using System;
using System.Collections.Generic;
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
    public delegate void Npc_SelectedEventHandler(object sender, Npc npc);

    /// <summary>
    /// Interaction logic for NpcSelect.xaml
    /// </summary>
    public partial class NpcSelect : UserControl
    {
        private Dictionary<string, bool> visibleEntries = new Dictionary<string, bool>();
        private ICollectionView npcsView;

        public NpcSelect()
        {
            InitializeComponent();

            _listView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(listViewItemDbClicked));
            _listView.KeyUp += new KeyEventHandler(listView_KeyUp);

            _typeContextMenu.Closed += new RoutedEventHandler(_typeContextMenu_Closed);
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
            Npcs.GetAllNpcs(false, false);
            npcsView = CollectionViewSource.GetDefaultView(Npcs);

            visibleEntries.Clear();
            foreach (Npc npc in Npcs)
            {
                if (!visibleEntries.ContainsKey(npc.Type))
                {
                    visibleEntries[npc.Type] = true;
                }
            }
            BuildTypeContextMenu();
        }

        public NpcCollection Npcs
        {
            get { return _mainGrid.Resources["_npcs"] as NpcCollection; }
        }

        private bool IsMatch(object npc)
        {
            Npc i = npc as Npc;
            if (!visibleEntries[i.Type])
                return false;

            return -1 != i.Name.IndexOf(filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            FilterNpc();
        }

        private void _typeContextMenu_Closed(object sender, RoutedEventArgs e)
        {
            FilterNpc();
        }

        private void FilterNpc()
        {
            if (npcsView == null)
                return;

            npcsView.Filter = new System.Predicate<object>(IsMatch);
        }

        bool nameAscending = true;
        bool idAscending = true;
        bool typeAscending = true;
        bool modelIdAscending = true;
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
                case "Type":
                    {
                        npcsView.SortDescriptions.Add(new SortDescription("Type",
                                               typeAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        typeAscending = !typeAscending;
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


        List<MenuItem> menuItems = new List<MenuItem>();
        public void BuildTypeContextMenu()
        {
            _typeContextMenu.Items.Clear();

            //
            {
                MenuItem menuItemAll = new MenuItem() { Tag = "All" };
                menuItemAll.SetResourceReference(MenuItem.HeaderProperty, "allSelect");
                menuItemAll.Click += new RoutedEventHandler(menuItem_Click);
                _typeContextMenu.Items.Add(menuItemAll);

                MenuItem menuItemNone = new MenuItem() { Tag = "None" };
                menuItemNone.SetResourceReference(MenuItem.HeaderProperty, "noneSelect");
                menuItemNone.Click += new RoutedEventHandler(menuItem_Click);
                _typeContextMenu.Items.Add(menuItemNone);
            }

            foreach (var en in visibleEntries)
            {
                MenuItem menuItem = new MenuItem() { Header = en.Key, IsChecked = en.Value };
                menuItem.Click += new RoutedEventHandler(menuItem_Click);
                _typeContextMenu.Items.Add(menuItem);

                menuItems.Add(menuItem);
            }
        }

        void menuItem_Click(object sender, RoutedEventArgs e)
        {
            MenuItem menuItem = sender as MenuItem;
            string text = menuItem.Tag as string;
            if (text == "All")
            {
                foreach(var v in menuItems)
                {
                    v.IsChecked = true;
                    visibleEntries[(string)v.Header] = true;
                }
            }
            else if (text == "None")
            {
                foreach (var v in menuItems)
                {
                    v.IsChecked = false;
                    visibleEntries[(string)v.Header] = false;
                }
            }
            else
            {
                menuItem.IsChecked = !menuItem.IsChecked;
                visibleEntries[(string)menuItem.Header] = menuItem.IsChecked;
            }
        }

    }
}
