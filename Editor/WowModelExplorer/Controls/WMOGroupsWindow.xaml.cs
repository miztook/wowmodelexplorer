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
    /// Interaction logic for WMOGroupsWindow.xaml
    /// </summary>
    public partial class WMOGroupsWindow : Window
    {
        private ICollectionView groupsView;

        public WMOGroupsWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(WMOGroupsWindow_Loaded);
            this.Closing += (sender, e) => { e.Cancel = true; Hide(); };
        }

        void WMOGroupsWindow_Loaded(object sender, RoutedEventArgs e)
        {
            this.Loaded -= WMOGroupsWindow_Loaded;

            ModelSceneService.Instance.MainWMOSceneNodeChanged += new MainWMOSceneNode_ChangedHandler(Instance_MainWMOSceneNodeChanged);
        }

        public WmoGroupCollection Groups
        {
            get { return _mainGrid.Resources["_groups"] as WmoGroupCollection; }
        }

        public void Update()
        {
            WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
            Groups.Clear();

            if (node == null)
                return;

            SWMOGroup[] groups = node.Groups;
            for (int i = 0; i < node.Groups.Length; ++i)
            {
                WmoGroup g = new WmoGroup(ref groups[i], i, node.IsGroupShow(i), node.IsGroupBoxShow(i));
                Groups.Add(g);

                g.PropertyChanged += new PropertyChangedEventHandler(group_PropertyChanged);
            }

            groupsView = CollectionViewSource.GetDefaultView(Groups);
        }

        void Instance_MainWMOSceneNodeChanged(object sender, WMOSceneNode node)
        {
            Update();
        }

        void group_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            WmoGroup g = sender as WmoGroup;
            if (e.PropertyName == "Visible")
            {
                WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
                if (node != null)
                    node.ShowGroup(g.Index, g.Visible);
            }
            else if(e.PropertyName == "BoxVisible")
            {
                WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
                if (node != null)
                    node.ShowGroupBox(g.Index, g.BoxVisible, new SColor(255, 0, 0));
            }
        }

        private void VisibleAllSelect_Click(object sender, RoutedEventArgs e)
        {
            foreach (var g in Groups)
            {
                g.Visible = true;
            }
        }

        private void VisibleNoneSelect_Click(object sender, RoutedEventArgs e)
        {
            foreach (var g in Groups)
            {
                g.Visible = false;
            }
        }

        private void BoxVisibleAllSelect_Click(object sender, RoutedEventArgs e)
        {
            foreach (var g in Groups)
            {
                g.BoxVisible = true;
            }
        }

        private void BoxVisibleNoneSelect_Click(object sender, RoutedEventArgs e)
        {
            foreach (var g in Groups)
            {
                g.BoxVisible = false;
            }
        }

        bool indexAscending = true;
        bool nameAscending = true;
        bool vCountAscending = true;
        bool iCountAscending = true;
        bool indoorAscending = true;
        private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (groupsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            groupsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Index":
                    {
                        groupsView.SortDescriptions.Add(new SortDescription("Index",
                                                indexAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        indexAscending = !indexAscending;
                    }
                    break;
                case "Name":
                    {
                        groupsView.SortDescriptions.Add(new SortDescription("Name",
                                              indexAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        nameAscending = !nameAscending;
                    }
                    break;
                case "VCount":
                    {
                        groupsView.SortDescriptions.Add(new SortDescription("VCount",
                                               vCountAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        vCountAscending = !vCountAscending;
                    }
                    break;
                case "ICount":
                    {
                        groupsView.SortDescriptions.Add(new SortDescription("ICount",
                                               iCountAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        iCountAscending = !iCountAscending;
                    }
                    break;
                case "InDoor":
                    {
                        groupsView.SortDescriptions.Add(new SortDescription("InDoor",
                                               indoorAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        indoorAscending = !indoorAscending;
                    }
                    break;
            }
        }


    }
}
