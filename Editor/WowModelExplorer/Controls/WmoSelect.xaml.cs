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
    /// <summary>
    /// Interaction logic for WmoSelect.xaml
    /// </summary>
    public partial class WmoSelect : UserControl
    {
        private ICollectionView wmoLocalNamesView;
        private ICollectionView m2NamesView;

        public event MapWMO_SelectedEventHandler MapLocalWMO_Selected;

        public event WorldM2_SelectedEventHandler M2Model_Selected;

        public WmoSelect()
        {
            InitializeComponent();

            _wmoLocalListView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(wmoLocalListViewItemDbClicked));
            _wmoLocalListView.KeyUp += new KeyEventHandler(wmoLocalListView_KeyUp);
            this.PreviewKeyDown += new KeyEventHandler(filter_PreviewKeyDown);

            _m2ListView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(m2ListViewItemDbClicked));
            _m2ListView.KeyUp += new KeyEventHandler(m2ListView_KeyUp);

            _m2Grid.Visibility = Visibility.Collapsed;
        }

        void filter_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (_wmoLocalfilter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (_m2filter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (!_wmoLocalfilter.IsKeyboardFocused && !_m2filter.IsKeyboardFocused && e.Key == Key.W)
                e.Handled = true;
        }

        public void FillData()
        {
            WMOLocalNames.Clear();

            WowDatabase database = Engine.Instance.WowDatabase;

            for (uint i = 0; i < database.WmoCount; ++i)
            {
                WMOLocalNames.Add(new Entry() { Name = database.GetWMOFileName(i, true), Id = (int)i });
            }

            wmoLocalNamesView = CollectionViewSource.GetDefaultView(WMOLocalNames);

            m2NamesView = CollectionViewSource.GetDefaultView(M2Names);
        }

        public EntryCollection WMOLocalNames
        {
            get { return _mainGrid.Resources["_wmoLocalNames"] as EntryCollection; }
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

                if(!bFind && name.Length > 0)
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
