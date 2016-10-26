using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using WowTexViewer.Data;
using WowTexViewer.Services;
using mywowNet;
using mywowNet.Data;

namespace WowTexViewer.Controls
{
    /// <summary>
    /// Interaction logic for MapTexNames.xaml
    /// </summary>
    public partial class MapTexNames : UserControl
    {
        private ICollectionView textureNamesView;

        public event Texture_SelectedEventHandler Texture_Selected;

        public MapTexNames()
        {
            InitializeComponent();

            _listView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(listViewItemDbClicked));
            _listView.KeyUp += new KeyEventHandler(listView_KeyUp);

            this.PreviewKeyDown += new KeyEventHandler(filter_PreviewKeyDown);
        }

        public void FillTexNames(Entry[] entries)
        {
            TextureNames.Clear();
            for (uint i = 0; i < entries.Length; ++i)
            {
                TextureNames.Add(new Entry() { Name = entries[i].Name, Id = entries[i].Id});
            }

            textureNamesView = CollectionViewSource.GetDefaultView(TextureNames);
        }

        public EntryCollection TextureNames
        {
            get { return _texGrid.Resources["_textureNames"] as EntryCollection; }
        }

        void filter_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (filter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (!filter.IsKeyboardFocused && e.Key == Key.W)
                e.Handled = true;
        }

        private void filter_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (textureNamesView != null)
                textureNamesView.Filter = new System.Predicate<object>(IsTextureNameMatch);
        }

        private bool IsTextureNameMatch(object entry)
        {
            Entry i = entry as Entry;
            return -1 != i.Name.IndexOf(filter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void listViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadTexture();
        }

        private void listView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadTexture();
        }

        private void LoadTexture()
        {
            Entry entry = _listView.SelectedItem as Entry;
            if (entry != null)
            {
                if (Texture_Selected != null)
                    Texture_Selected.Invoke(this, entry);
            }
        }

        private void btExport_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
