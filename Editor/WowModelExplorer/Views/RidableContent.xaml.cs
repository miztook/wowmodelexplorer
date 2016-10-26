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
using WowModelExplorer.Data;
using WowModelExplorer.Controls;
using WowModelExplorer.Services;
using mywowNet;
using mywowNet.Data;

namespace WowModelExplorer.Views
{
    /// <summary>
    /// Interaction logic for RidableContent.xaml
    /// </summary>
    public partial class RidableContent : DockableContent
    {
        public RidableContent()
        {
            InitializeComponent();

            _ridableSelect.Ridable_Selected += new Ridable_SelectedEventHandler(_ridableSelect_Ridable_Selected);
        }

        void _ridableSelect_Ridable_Selected(object sender, Ridable ridable)
        {
            if (ridable == null)
                return;

            Player player = Client.Instance.GetPlayer();
            M2SceneNode node = ModelSceneService.Instance.MainM2SceneNode;
            if (player == null || node == null || node.Type != M2Type.MT_CHARACTER || node.IsNpc)
                return;

            if (ridable.NpcId == 0)
            {
                player.RideOnModel(0, E_M2_STATES.EMS_STAND);
            } 
            else
            {
                player.RideOnModel(ridable.NpcId, GetStateByMountFlag(ridable.MountFlag));
            }

        }

        public void Initialize()
        {
            _ridableSelect.FillData();
        }

        E_M2_STATES GetStateByMountFlag(uint flag)
        {
            switch(flag)
            {
                case 1:
                    return E_M2_STATES.EMS_MOUNT;
                case 2:
                    return E_M2_STATES.EMS_RECLINEDMOUNT;
                case 3:
                    return E_M2_STATES.EMS_RECLINEDMOUNTPASSENGER;
                case 4:
                    return E_M2_STATES.EMS_STEALTHSTAND;
                default:
                    return E_M2_STATES.EMS_STAND;
            }
        }
    }
}
