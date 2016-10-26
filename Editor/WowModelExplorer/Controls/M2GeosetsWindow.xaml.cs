using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using mywowNet;
using mywowNet.Data;
using System.ComponentModel;
using WowModelExplorer.Data;
using WowModelExplorer.Services;

namespace WowModelExplorer.Controls
{
    /// <summary>
    /// Interaction logic for GeosetsWindow.xaml
    /// </summary>
    public partial class M2GeosetsWindow : Window
    {
        private ICollectionView geosetsView;

        public M2GeosetsWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(GeosetsWindow_Loaded);
            this.Closing += (sender, e) => { e.Cancel = true; Hide(); };
        }

        void GeosetsWindow_Loaded(object sender, RoutedEventArgs e)
        {
            this.Loaded -= GeosetsWindow_Loaded;

            ModelSceneService.Instance.MainM2SceneNodeChanged += new MainM2SceneNode_ChangedHandler(Instance_MainM2SceneNodeChanged);
        }

        public M2GeosetCollection Geosets
        {
            get { return _mainGrid.Resources["_geosets"] as M2GeosetCollection; }
        }

        public void Update()
        {
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            Geosets.Clear();

            if (node == null)
                return;

            SGeoset[] geosets = node.Geosets;
            for (int i = 0; i < node.Geosets.Length; ++i)
            {
                M2Geoset g = new M2Geoset(ref geosets[i], i, node.IsGeosetShow(i));
                Geosets.Add(g);

                g.PropertyChanged += new PropertyChangedEventHandler(geoset_PropertyChanged);
            }

            geosetsView = CollectionViewSource.GetDefaultView(Geosets);
        }

        private void Instance_MainM2SceneNodeChanged(object sender, M2SceneNode node)
        {
            Update();
        }

        private void geoset_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            M2Geoset g = sender as M2Geoset;
            if (e.PropertyName == "Visible")
            {
                M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
                if (node != null)
                    node.ShowGeoset(g.Index, g.Visible);
            }
        }

        private void VisibleAllSelect_Click(object sender, RoutedEventArgs e)
        {
            foreach (var g in Geosets)
            {
                g.Visible = true;
            }
        }

        private void VisibleNoneSelect_Click(object sender, RoutedEventArgs e)
        {
            foreach (var g in Geosets)
            {
                g.Visible = false;
            }
        }

        bool indexAscending = true;
        bool geosetIdAscending = true;
        bool vCountAscending = true;
        bool iCountAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (geosetsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            geosetsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Index":
                    {
                        geosetsView.SortDescriptions.Add(new SortDescription("Index",
                                                indexAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        indexAscending = !indexAscending;
                    }
                    break;
                case "GeoSetID":
                    {
                        geosetsView.SortDescriptions.Add(new SortDescription("GeoSetID", 
                                                geosetIdAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        geosetIdAscending = !geosetIdAscending;
                    }
                    break;
                case "VCount":
                    {
                        geosetsView.SortDescriptions.Add(new SortDescription("VCount",
                                               vCountAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        vCountAscending = !vCountAscending;
                    }
                    break;
                case "ICount":
                    {
                        geosetsView.SortDescriptions.Add(new SortDescription("ICount",
                                               iCountAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        iCountAscending = !iCountAscending;
                    }
                    break;
            }
        }
    }
}
