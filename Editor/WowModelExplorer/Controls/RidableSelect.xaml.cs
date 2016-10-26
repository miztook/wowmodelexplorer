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
    public delegate void Ridable_SelectedEventHandler(object sender, Ridable ridable);

    /// <summary>
    /// Interaction logic for RidableSelect.xaml
    /// </summary>
    public partial class RidableSelect : UserControl
    {
        private ICollectionView ridablesView;

        public RidableSelect()
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

        public RidableCollection Ridables
        {
            get { return _mainGrid.Resources["_ridables"] as RidableCollection; }
        }

        public void FillData()
        {
            Ridables.GetAllRidables();
            ridablesView = CollectionViewSource.GetDefaultView(Ridables);
        }

        private bool IsMatch(object ride)
        {
            Ridable i = ride as Ridable;
            return -1 != i.Name.IndexOf(filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            FilterRidable();
        }

        private void FilterRidable()
        {
            if (ridablesView == null)
                return;

            ridablesView.Filter = new System.Predicate<object>(IsMatch);
        }

        public event Ridable_SelectedEventHandler Ridable_Selected;

        private void listViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadRidable();
        }

        private void listView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                LoadRidable();
            }
        }

        private void LoadRidable()
        {
            Ridable ridable = _listView.SelectedItem as Ridable;
            if (ridable != null)
            {
                if (Ridable_Selected != null)
                    Ridable_Selected.Invoke(this, ridable);
            }
        }

        bool nameAscending = true;
        bool idAscending = true;
        bool modelIdAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (ridablesView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            ridablesView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Name":
                    {
                        ridablesView.SortDescriptions.Add(new SortDescription("Name",
                                                nameAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        nameAscending = !nameAscending;
                    }
                    break;
                case "ModelDisplayId":
                    {
                        ridablesView.SortDescriptions.Add(new SortDescription("ModelDisplayId",
                                               idAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        idAscending = !idAscending;
                    }
                    break;
                case "ModelId":
                    {
                        ridablesView.SortDescriptions.Add(new SortDescription("ModelId",
                                               modelIdAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        modelIdAscending = !modelIdAscending;
                    }
                    break;
            }

        }
    }
}
