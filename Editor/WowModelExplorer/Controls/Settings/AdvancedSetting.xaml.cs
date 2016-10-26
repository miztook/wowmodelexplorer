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
using mywowNet;
using WowModelExplorer.Services;

namespace WowModelExplorer.Controls.Settings
{
    /// <summary>
    /// Interaction logic for AdvancedSetting.xaml
    /// </summary>
    public partial class AdvancedSetting : UserControl
    {
        public AdvancedSetting()
        {
            InitializeComponent();

            _btApply.Click += delegate { Apply(); };
        }

        public void FillData()
        {
            SAdvancedSetting advancedSetting = Engine.Instance.AdvancedSetting;
            //
            _cbTripleBuffers.SelectedItem = (from x in TripleBuffers where x.On == advancedSetting.tripleBuffers select x).ToArray()[0];
            _cbTripleBuffers.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            _cbInputDelay.SelectedItem = (from x in InputDelays where x.On == advancedSetting.reduceInputLag select x).ToArray()[0];
            _cbInputDelay.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            _cbHardwarePointer.SelectedItem = (from x in HardwarePointers where x.On == advancedSetting.useHardwareCursor select x).ToArray()[0];
            _cbHardwarePointer.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            GraphicsInterfaces.Add(new GraphicsInterface() { DriverType = E_DRIVER_TYPE.Direct3D9 });
            if(Engine.Instance.IsAboveVista)
                GraphicsInterfaces.Add(new GraphicsInterface() { DriverType = E_DRIVER_TYPE.Direct3D11 });
            GraphicsInterfaces.Add(new GraphicsInterface() { DriverType = E_DRIVER_TYPE.OpenGL });

            E_DRIVER_TYPE drivertype = Engine.Instance.AdvancedSetting.driverType;
            _cbGraphicsInterface.SelectedItem = (from x in GraphicsInterfaces where x.DriverType == drivertype select x).ToArray()[0];
            _cbGraphicsInterface.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);
        }

        void _option_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            _btApply.IsEnabled = true;
        }

        private OptionBools TripleBuffers { get { return _mainGrid.Resources["_tripleBuffers"] as OptionBools; } }
        private OptionBools InputDelays { get { return _mainGrid.Resources["_inputDelays"] as OptionBools; } }
        private OptionBools HardwarePointers { get { return _mainGrid.Resources["_hardwarePointers"] as OptionBools; } }
        private GraphicsInterfaces GraphicsInterfaces { get { return _mainGrid.Resources["_graphicsInterfaces"] as GraphicsInterfaces; } }

        //result
        public bool TripleBuffer { get { return (_cbTripleBuffers.SelectedItem as OptionBool).On; } }
        public bool InputDelay { get { return (_cbInputDelay.SelectedItem as OptionBool).On; } }
        public bool HardwarePointer { get { return (_cbHardwarePointer.SelectedItem as OptionBool).On; } }
        public E_DRIVER_TYPE DriverType { get { return (_cbGraphicsInterface.SelectedItem as GraphicsInterface).DriverType; } }

        public void Apply()
        {
            if (!_btApply.IsEnabled)
                return;

            SAdvancedSetting advancedSetting = new SAdvancedSetting();
            advancedSetting.driverType = DriverType;
            advancedSetting.tripleBuffers = TripleBuffer;
            advancedSetting.reduceInputLag = InputDelay;
            advancedSetting.useHardwareCursor = HardwarePointer;
            advancedSetting.maxForegroundFPS = 120;
            advancedSetting.maxBackgroundFPS = 40;

            Engine.Instance.AdvancedSetting = advancedSetting;

            _btApply.IsEnabled = false;
        }
    }
}
