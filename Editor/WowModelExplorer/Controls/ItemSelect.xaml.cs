using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using WowModelExplorer.Data;
using WowModelExplorer.Services;
using mywowNet;
using mywowNet.Data;
using ItemCollection = mywowNet.Data.ItemCollection;

namespace WowModelExplorer.Controls
{
    public delegate void Item_SelectedEventHandler(object sender, Item item);

    /// <summary>
    /// Interaction logic for ItemSelect.xaml
    /// </summary>
    public partial class ItemSelect : UserControl
    {
        private Dictionary<string, bool> visibleEntries = new Dictionary<string, bool>();
        private ICollectionView itemsView;

        public ItemSelect()
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

        public void FillData(E_ITEM_TYPE type)
        {
            App app = Application.Current as App;
            Items.GetItemsByType(type);
            itemsView = CollectionViewSource.GetDefaultView(Items);

            visibleEntries.Clear();
            foreach (Item v in Items)
            {
                if (!visibleEntries.ContainsKey(v.SubClassName))
                {
                    visibleEntries[v.SubClassName] = true;
                }
            }
            BuildTypeContextMenu();
        }

        public void FillData(E_CHAR_SLOTS slot)
        {
            App app = Application.Current as App;
            Items.GetItemsBySlot(slot);
            itemsView = CollectionViewSource.GetDefaultView(Items);

            visibleEntries.Clear();
            foreach (Item v in Items)
            {
                if (!visibleEntries.ContainsKey(v.SubClassName))
                {
                    visibleEntries[v.SubClassName] = true;
                }
            }
            BuildTypeContextMenu();
        }

        public ItemCollection Items
        {
            get { return _mainGrid.Resources["_items"] as ItemCollection; }
        }

        private bool IsMatch(object item)
        {
            Item i = item as Item;
            if (!visibleEntries[i.SubClassName])
                return false;

            return -1 != i.Name.IndexOf(filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            FilterItem();
        }

        private void _typeContextMenu_Closed(object sender, RoutedEventArgs e)
        {
            FilterItem();
        }

        private void FilterItem()
        {
            if (itemsView == null)
                return;

            itemsView.Filter = new System.Predicate<object>(IsMatch);
        }

        bool nameAscending = true;
        bool idAscending = true;
        bool subclassAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (itemsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            itemsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Name":
                    {
                        itemsView.SortDescriptions.Add(new SortDescription("Name",
                                                nameAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        nameAscending = !nameAscending;
                    }
                    break;
                case "Id":
                    {
                        itemsView.SortDescriptions.Add(new SortDescription("Id",
                                               idAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        idAscending = !idAscending;
                    }
                    break;
                case "SubClassName":
                    {
                        itemsView.SortDescriptions.Add(new SortDescription("SubClassName",
                                               subclassAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        subclassAscending = !subclassAscending;
                    }
                    break;
            }
        }

        public event Item_SelectedEventHandler ItemSelected;

        private void listViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadItem();
        }

        private void listView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadItem();
        }

        private void LoadItem()
        {
            Item item = _listView.SelectedItem as Item;
            if (item != null)
            {
                if (ItemSelected != null)
                    ItemSelected.Invoke(this, item);
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
                foreach (var v in menuItems)
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
