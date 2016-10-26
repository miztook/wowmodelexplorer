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
    public delegate void Texture_SelectedEventHandler(object sender, Entry tex);

    /// <summary>
    /// Interaction logic for TextureSelect.xaml
    /// </summary>
    public partial class TextureSelect : UserControl
    {
        private ICollectionView textureNamesView;

        public event Texture_SelectedEventHandler Texture_Selected;

        public TextureSelect()
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
            TextureNames.Clear();

            WowDatabase database = Engine.Instance.WowDatabase;

            for (uint i = 0; i < database.TextureCount; ++i)
            {
                TextureNames.Add(new Entry() { Name = database.GetTextureFileName(i), Id = (int)i } );
            }

            textureNamesView = CollectionViewSource.GetDefaultView(TextureNames);
        }

        public EntryCollection TextureNames
        {
            get { return _mainGrid.Resources["_textureNames"] as EntryCollection; }
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

    }
}
