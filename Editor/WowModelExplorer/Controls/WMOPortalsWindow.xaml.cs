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
    public partial class WMOPortalsWindow : Window
    {
        private ICollectionView portalsView;

        public WMOPortalsWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(WMOPortalsWindow_Loaded);
            this.Closing += (sender, e) => { e.Cancel = true; Hide(); };
        }

         void WMOPortalsWindow_Loaded(object sender, RoutedEventArgs e)
        {
            this.Loaded -= WMOPortalsWindow_Loaded;

            ModelSceneService.Instance.MainWMOSceneNodeChanged += new MainWMOSceneNode_ChangedHandler(Instance_MainWMOSceneNodeChanged);
         }

         public WmoPortalCollection Portals
         {
             get { return _mainGrid.Resources["_portals"] as WmoPortalCollection; }
         }

         public void Update()
         {
             WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
             Portals.Clear();

             if (node == null)
                 return;

             SWMOPortal[] portals = node.Portals;
             for (int i = 0; i < node.Portals.Length; ++i)
             {
                 WmoPortal g = new WmoPortal(ref portals[i], i, node.IsPortalShow(i), node.IsPortalGroupsShow(i));
                 Portals.Add(g);

                 g.PropertyChanged += new PropertyChangedEventHandler(portal_PropertyChanged);
             }

             portalsView = CollectionViewSource.GetDefaultView(Portals);
         }

         void Instance_MainWMOSceneNodeChanged(object sender, WMOSceneNode node)
         {
             Update();
         }

         void portal_PropertyChanged(object sender, PropertyChangedEventArgs e)
         {
             WmoPortal g = sender as WmoPortal;
             if (e.PropertyName == "Visible")
             {
                 WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
                 if (node != null)
                 {
                     node.ShowPortal(g.Index, g.Visible, new SColor(255, 255, 0));
                 }
             }
             else if(e.PropertyName == "GroupsVisible")
             {
                  WMOSceneNode node = ModelSceneService.Instance.MainWMOSceneNode;
                  if (node != null)
                  {
                      node.ShowPortalGroups(g.Index, g.GroupsVisible);
                  }
             }

         }

         bool indexAscending = true;
         bool frontGroupAscending = true;
         bool backGroupAscending = true;
         private void GridViewColumnHeader_Click(object sender, RoutedEventArgs e)
         {
             if (portalsView == null)
                return;

            GridViewColumnHeader header = sender as GridViewColumnHeader;

            portalsView.SortDescriptions.Clear();
            switch (header.Tag.ToString())
            {
                case "Index":
                    {
                        portalsView.SortDescriptions.Add(new SortDescription("Index",
                                                indexAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        indexAscending = !indexAscending;
                    }
                    break;
                case "FrontGroupIndex":
                    {
                        portalsView.SortDescriptions.Add(new SortDescription("FrontGroupIndex",
                                                frontGroupAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        frontGroupAscending = !frontGroupAscending;
                    }
                    break;
                case "BackGroupIndex":
                    {
                        portalsView.SortDescriptions.Add(new SortDescription("BackGroupIndex",
                                                backGroupAscending ? ListSortDirection.Ascending : ListSortDirection.Descending));
                        backGroupAscending = !backGroupAscending;
                    }
                    break;
            }
         }

         private void VisibleAllSelect_Click(object sender, RoutedEventArgs e)
         {
             foreach (var g in Portals)
             {
                 g.Visible = true;
             }
         }

         private void VisibleNoneSelect_Click(object sender, RoutedEventArgs e)
         {
             foreach (var g in Portals)
             {
                 g.Visible = false;
             }
         }

         private void GroupsVisibleAllSelect_Click(object sender, RoutedEventArgs e)
         {
             foreach (var g in Portals)
             {
                 g.GroupsVisible = true;
             }
         }

         private void GroupsVisibleNoneSelect_Click(object sender, RoutedEventArgs e)
         {
             foreach (var g in Portals)
             {
                 g.GroupsVisible = false;
             }
         }
    }
}
