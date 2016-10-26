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
    public delegate void WorldM2_SelectedEventHandler(object sender, string m2name);

    /// <summary>
    /// Interaction logic for WorldM2Select.xaml
    /// </summary>
    public partial class WorldM2Select : UserControl
    {
        private ICollectionView m2NamesView;

        public event WorldM2_SelectedEventHandler M2Model_Selected;

        public WorldM2Select()
        {
            InitializeComponent();

            _m2ListView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(m2ListViewItemDbClicked));
            _m2ListView.KeyUp += new KeyEventHandler(m2ListView_KeyUp);
            this.PreviewKeyDown += new KeyEventHandler(filter_PreviewKeyDown);
        }

        void filter_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (_m2filter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (!_m2filter.IsKeyboardFocused && e.Key == Key.W)
                e.Handled = true;
        }

        public void FillData()
        {
            M2Names.Clear();

            WowDatabase database = Engine.Instance.WowDatabase;

            for (uint i = 0; i < database.WorldModelCount; ++i)
            {
                M2Names.Add(new Entry() { Name = database.GetWorldModelName(i, false), Id = (int)i });
            }

            m2NamesView = CollectionViewSource.GetDefaultView(M2Names);
        }

        public EntryCollection M2Names
        {
            get { return _mainGrid.Resources["_m2Names"] as EntryCollection; }
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
