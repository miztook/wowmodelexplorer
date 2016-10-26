using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using WowModelExplorer.Data;
using mywowNet.Data;
using WowModelExplorer.Services;

namespace WowModelExplorer.Controls
{
    public delegate void Set_SelectedEventHandler(object sender, Set set);

    /// <summary>
    /// Interaction logic for SetSelect.xaml
    /// </summary>
    public partial class SetSelect : UserControl
    {
        private ICollectionView setsView;

        public SetSelect()
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
            Sets.GetAllSets();
            setsView = CollectionViewSource.GetDefaultView(Sets);
        }

        public SetCollection Sets
        {
            get { return _mainGrid.Resources["_sets"] as SetCollection; }
        }

        private bool IsMatch(object set)
        {
            Set i = set as Set;

            return -1 != i.Name.IndexOf(filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            FilterSet();
        }

        private void FilterSet()
        {
            if (setsView == null)
                return;

            setsView.Filter = new System.Predicate<object>(IsMatch);
        }

        bool nameAscending = true;
        bool idAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (setsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            setsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Name":
                    {
                        setsView.SortDescriptions.Add(new SortDescription("Name",
                                                nameAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        nameAscending = !nameAscending;
                    }
                    break;
                case "Id":
                    {
                        setsView.SortDescriptions.Add(new SortDescription("Id",
                                               idAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        idAscending = !idAscending;
                    }
                    break;
            }
        }

        public event Set_SelectedEventHandler Set_Selected;

        private void listViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadSet();
        }

        private void listView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadSet();
        }

        private void LoadSet()
        {
            Set set = _listView.SelectedItem as Set;
            if (set != null)
            {
                if (Set_Selected != null)
                    Set_Selected.Invoke(this, set);
            }
        }
    }
}
