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
using WowModelExplorer.Controls;
using WowModelExplorer.Data;
using WowModelExplorer.Services;

namespace WowModelExplorer.Views
{
    /// <summary>
    /// Interaction logic for CharacterContent.xaml
    /// </summary>
    public partial class CharacterContent : DockableContent
    {
        public CharacterContent()
        {
            InitializeComponent();
        }

        public void Initialize()
        {
            App app = Application.Current as App;
            _characterSelect.AllianceRaceInfos.GetAllianceRaceInfos(app.Language);
            _characterSelect.HordeRaceInfos.GetHordeRaceInfos(app.Language);

            app.ApplicationLanguageChanged += new Application_LanguageChangedHandler(app_ApplicationLanguageChanged);
            _characterSelect.StartOutfitUpdated += new StartOutfitUpdatedHandler(_characterSelect_StartOutfitUpdated);
        }

        public CharacterSelect CharacterSelect
        {
            get { return _characterSelect; }
        }

        void _characterSelect_StartOutfitUpdated(object sender, mywowNet.M2SceneNode node)
        {
            //update property
            ShellService.Instance.PropertyContent.UpdateCharacterNode(node);

            if (ShellService.Instance.EquipmentsWindow.IsVisible)
                ShellService.Instance.EquipmentsWindow.Update();

            if (ShellService.Instance.EquipmentSelectWindow.IsVisible)
                ShellService.Instance.EquipmentSelectWindow.Update();
        }

        void app_ApplicationLanguageChanged(object sender, Languages current)
        {
            _characterSelect.AllianceRaceInfos.GetAllianceRaceInfos(current);
            _characterSelect.HordeRaceInfos.GetHordeRaceInfos(current);
        }
    }
}
