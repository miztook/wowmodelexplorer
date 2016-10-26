using System;
using AvalonDock;
using System.Windows.Input;
using WowModelExplorer.Controls;
using WowModelExplorer.Data;
using mywowNet;
using mywowNet.Data;
using WowModelExplorer.Services;

namespace WowModelExplorer.Views
{
    /// <summary>
    /// Interaction logic for MapObjectContent.xaml
    /// </summary>
    public partial class MapModelContent : DockableContent
    {
        public MapModelContent()
        {
            InitializeComponent();
        }

        void _mapModelSelect_Adt_Selected(object sender, Adt adt, FileADT fileadt)
        {
            if (fileadt != null)
            {
                ShellService.Instance.MapModelNameContent.FillADTData(fileadt);
                ShellService.Instance.MapModelNameContent.Show();
            }
        }

        public void Initialize()
        {
            _mapModelSelect.Initialize();

            _mapModelSelect.FillData();

            _mapModelSelect.Adt_Selected += new Adt_SelectedEventHandler(_mapModelSelect_Adt_Selected);
        }
    }
}
