using System;
using AvalonDock;
using System.Windows.Input;
using WowTexViewer.Controls;
using WowTexViewer.Data;
using mywowNet;
using mywowNet.Data;
using WowTexViewer.Services;

namespace WowTexViewer.Views
{
    /// <summary>
    /// Interaction logic for MapTexContent.xaml
    /// </summary>
    public partial class MapTexContent : DockableContent
    {
        public MapTexContent()
        {
            InitializeComponent();
        }

        public void Initialize()
        {
            _mapTexSelect.Initialize();

            _mapTexSelect.FillData();

            _mapTexSelect.Adt_Selected += new Adt_SelectedEventHandler(_mapModelSelect_Adt_Selected);
        }

        void _mapModelSelect_Adt_Selected(object sender, Adt adt, FileADT fileadt)
        {
            if (fileadt != null)
            {
                Entry[] entries = new Entry[fileadt.TextureCount];
                for (uint i = 0; i < fileadt.TextureCount; ++i)
                {
                    entries[i] = new Entry() { Id = (int)i, Name = fileadt.GetTextureFileName(i, false) };
                }
                ShellService.Instance.MapTexNameContent.FillMapTexNames(entries);
            }
        }
    }
}
