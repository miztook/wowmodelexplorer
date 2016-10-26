using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using WowTexViewer.Data;
using WowTexViewer.Services;
using System.Diagnostics;
using mywowNet;
using mywowNet.Data;

namespace WowTexViewer.Controls
{
    public delegate void Adt_SelectedEventHandler(object sender, Adt adt, FileADT fileadt);
    public delegate void Wdt_SelectedEventHandler(object sender, FileWDT filewdt);

    /// <summary>
    /// Interaction logic for MapTexSelect.xaml
    /// </summary>
    public partial class MapTexSelect : UserControl
    {
        private Dictionary<string, bool> visibleEntries = new Dictionary<string, bool>();
        private ICollectionView mapsView;
        private ICollectionView adtsView;

        public event Wdt_SelectedEventHandler Wdt_Selected;
        public event Adt_SelectedEventHandler Adt_Selected;

        public MapTexSelect()
        {
            InitializeComponent();

            _maplistView.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(mapListViewItemDbClicked));
            _maplistView.KeyUp += new KeyEventHandler(mapListView_KeyUp);

            mapTypeContextMenu.Closed += new RoutedEventHandler(mapTypeContextMenu_Closed);

            _adtlistview.AddHandler(ListViewItem.MouseDoubleClickEvent, new MouseButtonEventHandler(adtListViewItemDbClicked));
            _adtlistview.KeyUp += new KeyEventHandler(adtListView_KeyUp);
            this.PreviewKeyDown += new KeyEventHandler(filter_PreviewKeyDown);
        }
        
        void filter_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (_mapfilter.IsKeyboardFocused || _adtfilter.IsKeyboardFocused)
                ShellService.Instance.MainWindow.IsKeyFocus = false;

            if (!_mapfilter.IsKeyboardFocused && !_adtfilter.IsKeyboardFocused && e.Key == Key.W)
                e.Handled = true;
        }

        public void Initialize()
        {
            App app = Application.Current as App;
            app.ApplicationLanguageChanged += new Application_LanguageChangedHandler(app_ApplicationLanguageChanged);
        }

        void app_ApplicationLanguageChanged(object sender, Languages current)
        {
            if (mapsView != null)
                mapsView.Filter = null;

            Maps.GetAllMaps(current);
            mapsView = CollectionViewSource.GetDefaultView(Maps);
            adtsView = CollectionViewSource.GetDefaultView(Adts);

            visibleEntries.Clear();
            foreach (Map map in Maps)
            {
                if (!visibleEntries.ContainsKey(map.Type))
                {
                    visibleEntries[map.Type] = true;
                }
            }

            FilterMap();
        }

        public void FillData()
        {
            App app = Application.Current as App;
            Maps.GetAllMaps(app.Language);
            mapsView = CollectionViewSource.GetDefaultView(Maps);
            adtsView = CollectionViewSource.GetDefaultView(Adts);

            visibleEntries.Clear();
            foreach (Map map in Maps)
            {
                if (!visibleEntries.ContainsKey(map.Type))
                {
                    visibleEntries[map.Type] = true;
                }
            }

            BuildTypeContextMenu();
        }

        public MapCollection Maps
        {
            get { return _mapGrid.Resources["_maps"] as MapCollection; }
        }

        public AdtCollection Adts
        {
            get { return _adtGrid.Resources["_adts"] as AdtCollection; }
        }

        private bool IsMapMatch(object map)
        {
            Map i = map as Map;
            if (!visibleEntries[i.Type])
                return false;
            return -1 != i.Name.IndexOf(_mapfilter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void mapfilter_TextChanged(object sender, TextChangedEventArgs e)
        {
            FilterMap();
        }

        private void mapTypeContextMenu_Closed(object sender, RoutedEventArgs e)
        {
            FilterMap();
        }

        private void FilterMap()
        {
            if (mapsView == null)
                return;

            mapsView.Filter = new System.Predicate<object>(IsMapMatch);
        }

        bool nameAscending = true;
        bool idAscending = true;
        bool typeAscending = true;
        private void mapGridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (mapsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            mapsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Name":
                    {
                        mapsView.SortDescriptions.Add(new SortDescription("Name",
                                                nameAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        nameAscending = !nameAscending;
                    }
                    break;
                case "Id":
                    {
                        mapsView.SortDescriptions.Add(new SortDescription("Id",
                                               idAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        idAscending = !idAscending;
                    }
                    break;
                case "Type":
                    {
                        mapsView.SortDescriptions.Add(new SortDescription("Type",
                                               typeAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        typeAscending = !typeAscending;
                    }
                    break;
            }

        }

        private void mapListViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadMap();
        }

        private void mapListView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadMap();
        }

        private void LoadMap()
        {
            Map map = _maplistView.SelectedItem as Map;
            if (map == null)
                return;

            FileWDT wdt = ModelSceneService.Instance.LoadWDT(map.Name, map.Id);
            if (wdt != null)
            {
                if (Wdt_Selected != null)
                    Wdt_Selected.Invoke(this, wdt);

                Adts.GetAllAdts(wdt);
            }
            else
            {
                Adts.Clear();
            }
        }

        List<MenuItem> menuItems = new List<MenuItem>();
        public void BuildTypeContextMenu()
        {
            mapTypeContextMenu.Items.Clear();

            //
            {
                MenuItem menuItemAll = new MenuItem() { Tag = "All" };
                menuItemAll.SetResourceReference(MenuItem.HeaderProperty, "allSelect");
                menuItemAll.Click += new RoutedEventHandler(menuItem_Click);
                mapTypeContextMenu.Items.Add(menuItemAll);

                MenuItem menuItemNone = new MenuItem() { Tag = "None" };
                menuItemNone.SetResourceReference(MenuItem.HeaderProperty, "noneSelect");
                menuItemNone.Click += new RoutedEventHandler(menuItem_Click);
                mapTypeContextMenu.Items.Add(menuItemNone);
            }

            foreach (var en in visibleEntries)
            {
                MenuItem menuItem = new MenuItem() { Header = en.Key, IsChecked = en.Value };
                menuItem.Click += new RoutedEventHandler(menuItem_Click);
                mapTypeContextMenu.Items.Add(menuItem);

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

        private void adtfilter_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (adtsView == null)
                return;

            adtsView.Filter = new System.Predicate<object>(IsAdtMatch);
        }

        private bool IsAdtMatch(object adt)
        {
            Adt i = adt as Adt;
            return -1 != i.Name.IndexOf(_adtfilter.Text, StringComparison.CurrentCultureIgnoreCase);
        }

        private void adtListViewItemDbClicked(object sender, MouseButtonEventArgs e)
        {
            LoadADT();
        }

        private void adtListView_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                LoadADT();
        }

        private void LoadADT()
        {
            Adt i = _adtlistview.SelectedItem as Adt;
            if (i == null)
                return;

            FileADT adt = ModelSceneService.Instance.LoadADTTextures(i.Row, i.Col);
            Debug.Assert(adt != null);
            if (adt != null)
            {
                if (Adt_Selected != null)
                    Adt_Selected.Invoke(this, i, adt);
            }
        }

        private void menuShowAllTexturesWDT_Click(object sender, RoutedEventArgs e)
        {
            /*
            Map map = _maplistView.SelectedItem as Map;
            if (map == null)
                return;

            FileWDT wdt = ModelSceneService.Instance.LoadWDT(map.Name, map.Id);
            if (wdt != null)
            {
                AdtCollection adtCollection = new AdtCollection();
                adtCollection.GetAllAdts(wdt);

                List<Entry> listEntries = new List<Entry>();
                for (int i = 0; i < adtCollection.Count; ++i)
                {
                    Adt adt = adtCollection[i];
                    FileADT fileAdt = ModelSceneService.Instance.LoadADTTextures(adt.Row, adt.Col);
                    for (uint t = 0; t < fileAdt.TextureCount; ++t)
                    {
                        Entry entry = new Entry() { Id = listEntries.Count, Name = fileAdt.GetTextureFileName(t, false) };
                        listEntries.Add(entry);
                    }
                    Debug.WriteLine(i);
                }
                ShellService.Instance.MapTexNameContent.FillMapTexNames(listEntries.ToArray());
            }
             */
        }
    }
}
