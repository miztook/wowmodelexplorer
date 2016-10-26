using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Collections.ObjectModel;
using mywowNet;

namespace WowModelExplorer.Controls.Settings
{

    /// <summary>
    /// Interaction logic for VideoSetting.xaml
    /// </summary>
    public partial class VideoSetting : UserControl
    {
        public VideoSetting()
        {
            InitializeComponent();

            _btApply.Click += delegate { Apply(); };
        }

        public void FillData()
        {
            //engine setting
            uint width, height;
            Engine.Instance.Driver.GetDisplayMode(out width, out height);
            SVideoSetting videoSetting = Engine.Instance.VideoSetting;

            //
            DisplayModes.Clear();
            DisplayModes.Add(new DisplayMode() { WindowMode=true });
            _cbDisplayModes.SelectedIndex = 0;
            _cbDisplayModes.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            Resolutions.Clear();   
            Resolutions.Add(new Resolution() { Width = width, Height = height });

            _cbResolutions.SelectedIndex = 0;
            _cbResolutions.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //

            RefreshRates.Default();
            _cbRefreshRates.SelectedIndex = 0;
            _cbRefreshRates.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            VideoDevices.Clear();
            VideoDevices.Add(new VideoDevice() { Index = 0 });

            _cbVideoDevices.SelectedIndex = 0;
            _cbVideoDevices.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            MultiSamples.Default();
            _cbMultisamples.SelectedItem = (from x in MultiSamples where x.Sample == videoSetting.antialias select x).ToArray()[0];
            _cbMultisamples.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            _cbVsyncs.SelectedItem = (from x in VSyncs where x.On == videoSetting.vsync select x).ToArray()[0];
            _cbVsyncs.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            MatResolutions.Clear();
            MatResolutions.Add(new OptionLevel() { Level = E_LEVEL.EL_LOW });
            MatResolutions.Add(new OptionLevel() { Level = E_LEVEL.EL_FAIR });
            MatResolutions.Add(new OptionLevel() { Level = E_LEVEL.EL_GOOD });
            MatResolutions.Add(new OptionLevel() { Level = E_LEVEL.EL_HIGH });

            _cbMatResolutions.SelectedItem = (from x in MatResolutions where x.Level == videoSetting.textureResolution select x).ToArray()[0];
            _cbMatResolutions.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            _cbMatTextureFilters.SelectedItem = (from x in MatTextureFilters where x.Filter == videoSetting.textureFiltering select x).ToArray()[0];
            _cbMatTextureFilters.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            _cbMatProjections.SelectedItem = (from x in MatProjections where x.On == false select x).ToArray()[0];
            _cbMatProjections.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            //
            ViewDistances.Default();
            _cbViewDistances.SelectedItem = (from x in ViewDistances where x.Level == videoSetting.viewDistance select x).ToArray()[0];
            _cbViewDistances.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            EnvDetails.Default();
            _cbEnvDetails.SelectedItem = (from x in EnvDetails where x.Level == videoSetting.envDetail select x).ToArray()[0];
            _cbEnvDetails.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            GroundScenes.Default();
            _cbGroundScenes.SelectedItem = (from x in GroundScenes where x.Level == videoSetting.groundClutter select x).ToArray()[0];
            _cbGroundScenes.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            ShadowQualities.Default();
            _cbShadowQuality.SelectedItem = (from x in ShadowQualities where x.Level == videoSetting.shadowQuality select x).ToArray()[0];
            _cbShadowQuality.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            LiquidDetails.Clear();
            LiquidDetails.Add(new OptionLevel() { Level = E_LEVEL.EL_LOW });
            LiquidDetails.Add(new OptionLevel() { Level = E_LEVEL.EL_FAIR });
            LiquidDetails.Add(new OptionLevel() { Level = E_LEVEL.EL_GOOD });
            LiquidDetails.Add(new OptionLevel() { Level = E_LEVEL.EL_ULTRA });
            _cbLiquidDetail.SelectedItem = (from x in LiquidDetails where x.Level == videoSetting.liquidDetail select x).ToArray()[0];
            _cbLiquidDetail.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            SunLightEffects.Clear();
            SunLightEffects.Add(new OptionLevel() { Level = E_LEVEL.EL_DISABLE });
            SunLightEffects.Add(new OptionLevel() { Level = E_LEVEL.EL_LOW });
            SunLightEffects.Add(new OptionLevel() { Level = E_LEVEL.EL_HIGH });
            _cbSunLightEffect.SelectedItem = (from x in SunLightEffects where x.Level == videoSetting.sunshafts select x).ToArray()[0];
            _cbSunLightEffect.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            ParticleDensities.Default();
            _cbParticleDensity.SelectedItem = (from x in ParticleDensities where x.Level == videoSetting.particleDensity select x).ToArray()[0];
            _cbParticleDensity.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);

            SSAOLevels.Clear();
            SSAOLevels.Add(new OptionLevel() { Level = E_LEVEL.EL_DISABLE});
            SSAOLevels.Add(new OptionLevel() { Level = E_LEVEL.EL_LOW });
            SSAOLevels.Add(new OptionLevel() { Level = E_LEVEL.EL_HIGH });
            _cbSSAO.SelectedItem = (from x in SSAOLevels where x.Level == videoSetting.SSAO select x).ToArray()[0];
            _cbSSAO.SelectionChanged += new SelectionChangedEventHandler(_option_SelectionChanged);
        }

        void _option_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            _btApply.IsEnabled = true;
        }

        private DisplayModes DisplayModes { get { return _mainGrid.Resources["_displayModes"] as DisplayModes; } }
        private Resolutions Resolutions { get { return _mainGrid.Resources["_resolutions"] as Resolutions; } }
        private RefreshRates RefreshRates { get { return _mainGrid.Resources["_refreshrates"] as RefreshRates; } }
        private VideoDevices VideoDevices { get { return _mainGrid.Resources["_videoDevices"] as VideoDevices; } }
        private MultiSamples MultiSamples { get { return _mainGrid.Resources["_multisamples"] as MultiSamples; } }
        private OptionBools VSyncs { get { return _mainGrid.Resources["_vsyncs"] as OptionBools; } }
        private OptionLevels MatResolutions { get { return _mainGrid.Resources["_matResolutions"] as OptionLevels; } }
        private TextureFilters MatTextureFilters { get { return _mainGrid.Resources["_matTextureFilters"] as TextureFilters; } }
        private OptionBools MatProjections { get { return _mainGrid.Resources["_matProjections"] as OptionBools; } }
        private OptionLevels ViewDistances { get { return _mainGrid.Resources["_viewDistances"] as OptionLevels; } }
        private OptionLevels EnvDetails { get { return _mainGrid.Resources["_envDetails"] as OptionLevels; } }
        private OptionLevels GroundScenes { get { return _mainGrid.Resources["_groundScenes"] as OptionLevels; } }
        private OptionLevels ShadowQualities { get { return _mainGrid.Resources["_shadowQuality"] as OptionLevels; } }
        private OptionLevels LiquidDetails { get { return _mainGrid.Resources["_liquidDetail"] as OptionLevels; } }
        private OptionLevels SunLightEffects { get { return _mainGrid.Resources["_sunLightEffect"] as OptionLevels; } }
        private OptionLevels ParticleDensities { get { return _mainGrid.Resources["_particleDensity"] as OptionLevels; } }
        private OptionLevels SSAOLevels { get { return _mainGrid.Resources["_ssao"] as OptionLevels; } }

        //result
        public uint AntiAlias { get { return (_cbMultisamples.SelectedItem as MultiSample).Sample; } }
        public bool VSync { get { return (_cbVsyncs.SelectedItem as OptionBool).On; } }
        public E_TEXTURE_FILTER TextureFilter { get { return (_cbMatTextureFilters.SelectedItem as TextureFilter).Filter; } }
        public E_LEVEL MatResolutionLevel { get { return (_cbMatResolutions.SelectedItem as OptionLevel).Level; } }
        public bool MatProjection { get { return (_cbMatProjections.SelectedItem as OptionBool).On; } }
        public E_LEVEL ViewDistance { get { return (_cbViewDistances.SelectedItem as OptionLevel).Level;  } }
        public E_LEVEL EnvDetail { get { return (_cbEnvDetails.SelectedItem as OptionLevel).Level; } }
        public E_LEVEL GroundScene { get { return (_cbGroundScenes.SelectedItem as OptionLevel).Level; } }
        public E_LEVEL ShadowQuality { get { return (_cbShadowQuality.SelectedItem as OptionLevel).Level; } }
        public E_LEVEL LiquidDetail { get { return (_cbLiquidDetail.SelectedItem as OptionLevel).Level; } }
        public E_LEVEL SunLightEffect { get { return (_cbSunLightEffect.SelectedItem as OptionLevel).Level; } }
        public E_LEVEL ParticleDensity { get { return (_cbParticleDensity.SelectedItem as OptionLevel).Level;  } }
        public E_LEVEL SSAO { get { return (_cbSSAO.SelectedItem as OptionLevel).Level; } }

        public void Apply()
        {
            if (!_btApply.IsEnabled)
                return;

            SVideoSetting setting = new SVideoSetting();
            setting.antialias = AntiAlias;
            setting.vsync = VSync;
            setting.textureFiltering = TextureFilter;
            setting.textureResolution = MatResolutionLevel;
            setting.textureProjection = MatProjection;
            setting.viewDistance = ViewDistance;
            setting.envDetail = EnvDetail;
            setting.groundClutter = GroundScene;
            setting.shadowQuality = ShadowQuality;
            setting.liquidDetail = LiquidDetail;
            setting.sunshafts = SunLightEffect;
            setting.particleDensity = ParticleDensity;
            setting.SSAO = SSAO;

            Engine.Instance.VideoSetting = setting;

            _btApply.IsEnabled = false;
        }
    }
}
