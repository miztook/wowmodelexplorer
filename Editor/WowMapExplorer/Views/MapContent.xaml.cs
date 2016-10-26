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
using System.Windows.Navigation;
using System.Windows.Shapes;
using WowMapExplorer.Controls;
using WowMapExplorer.Services;
using WowMapExplorer.Data;
using mywowNet;
using mywowNet.Data;
using AvalonDock;

namespace WowMapExplorer.Views
{
    /// <summary>
    /// Interaction logic for MapContent.xaml
    /// </summary>
    public partial class MapContent : DockableContent
    {
        public MapContent()
        {
            InitializeComponent();

            this.PreviewKeyDown += (sender, e) => { if (e.Key == Key.W) e.Handled = true; };
        }

        public void Initialize()
        {
            _mapSelect.Initialize();

            _mapSelect.FillData();

            _mapSelect.Adt_Selected += new Adt_SelectedEventHandler(_mapSelect_Adt_Selected);
        }

        void _mapSelect_Adt_Selected(object sender, Adt adt)
        {
            FileWDT wdt = MapSceneService.Instance.CurrentWDT;
            if (wdt != null && adt != null)
            {
                MapSceneService.Instance.SetMainMapTileSceneNode(wdt, (int)adt.Row, (int)adt.Col);
            }
              
        }

    }
}
