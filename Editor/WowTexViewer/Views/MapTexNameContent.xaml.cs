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
using AvalonDock;
using WowTexViewer.Controls;
using WowTexViewer.Services;
using mywowNet.Data;

namespace WowTexViewer.Views
{
    /// <summary>
    /// Interaction logic for MapTexNames.xaml
    /// </summary>
    public partial class MapTexNameContent : DockableContent
    {
        public MapTexNameContent()
        {
            InitializeComponent();
            _mapTexNames.Texture_Selected += new Texture_SelectedEventHandler(_mapTexNames_Texture_Selected);
        }

        void _mapTexNames_Texture_Selected(object sender, Entry tex)
        {
            ModelSceneService.Instance.SetCurrentTextureName(tex.Name, 1.0f);
        }

        public void FillMapTexNames(Entry[] entries)
        {
            _mapTexNames.FillTexNames(entries);
        }
    }
}
