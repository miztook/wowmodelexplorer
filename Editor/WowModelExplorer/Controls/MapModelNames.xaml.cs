using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using WowModelExplorer.Data;
using mywowNet;
using mywowNet.Data;
using WowModelExplorer.Services;

namespace WowModelExplorer.Controls
{
    public delegate void MapM2_SelectedEventHandler(object sender, Entry entry);
    public delegate void MapWMO_SelectedEventHandler(object sender, Entry entry);

    /// <summary>
    /// Interaction logic for MapModelNames.xaml
    /// </summary>
    public partial class MapModelNames : UserControl
    {
        private ICollectionView m2NamesView;
        private ICollectionView wmoLocalNamesView;

        public event MapM2_SelectedEventHandler MapM2_Selected;
        public event MapWMO_SelectedEventHandler MapLocalWMO_Selected;

        public MapModelNames()
        {
            InitializeComponent();

            _m2listView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(m2ListViewItemDbClicked));
            _m2listView.KeyUp += new KeyEventHandler(m2ListView_KeyUp);

            _wmoLocalListView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(wmoLocalListViewItemDbClicked));
            _wmoLocalListView.KeyUp += new KeyEventHandler(wmoLocalListView_KeyUp);
            this.PreviewKeyDown += new KeyEventHandler(filter_PreviewKeyDown);
        }

        void filter_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (_m2filter.IsKeyboardFocused || _wmoLocalfilter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (!_m2filter.IsKeyboardFocused && !_wmoLocalfilter.IsKeyboardFocused && e.Key == Key.W)
                e.Handled = true;
        }

        public void FillADTData(FileADT fileadt)
        {
            M2Names.Clear();
            for (uint i = 0; i < fileadt.M2Count; ++i)
            {
                M2Names.Add(new Entry() { Name = fileadt.GetM2FileName(i, true), Id = (int)i });
            }

            WMOLocalNames.Clear();
            for (uint i = 0; i < fileadt.WMOCount; ++i)
            {
                WMOLocalNames.Add(new Entry() { Name = fileadt.GetWMOFileName(i, true), Id = (int)i });
            }

            m2NamesView = CollectionViewSource.GetDefaultView(M2Names);
            wmoLocalNamesView = CollectionViewSource.GetDefaultView(WMOLocalNames);
        }

        public EntryCollection M2Names
        {
            get { return _m2Grid.Resources["_m2Names"] as EntryCollection; }
        }

        public EntryCollection WMOLocalNames
        {
            get { return _wmoLocalGrid.Resources["_wmoLocalNames"] as EntryCollection; }
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
            LoadMapM2();
        }

        private void m2ListView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadMapM2();
        }

        private void LoadMapM2()
        {
            Entry entry = _m2listView.SelectedItem as Entry;
            if (entry != null)
            {
                if (MapM2_Selected != null)
                    MapM2_Selected.Invoke(this, entry);
            }
        }

        private void wmoLocalfilter_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (wmoLocalNamesView != null)
                wmoLocalNamesView.Filter = new System.Predicate<object>(IsLocalWMOMatch);
        }

        private bool IsLocalWMOMatch(object entry)
        {
            Entry i = entry as Entry;
            return -1 != i.Name.IndexOf(_wmoLocalfilter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void wmoLocalListViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadLocalMapWMO();
        }

        private void wmoLocalListView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadLocalMapWMO();
        }

        private void LoadLocalMapWMO()
        {
            Entry entry = _wmoLocalListView.SelectedItem as Entry;
            if (entry != null)
            {
                if (MapLocalWMO_Selected != null)
                    MapLocalWMO_Selected.Invoke(this, entry);
            }
        }

    }
}
