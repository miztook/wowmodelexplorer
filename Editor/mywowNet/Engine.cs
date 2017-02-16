using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Engine : IDisposable
    {
        private static Engine instance = null;

        public Engine()
        {
            if (instance == null)
            {
                instance = this;
                engine_create();
            }
        }

        ~Engine()
        {
            Dispose();
        }

        public static Engine Instance
        {
            get { return instance; }
        }

        bool _disposed = false;
        public void Dispose()
        {
            if (!_disposed)
            {
                engine_destroy();
                _disposed = true;
            }
        }

        public IntPtr HWnd
        {
            get { return engine_getHWnd(); }
        }

        public bool Init(E_DRIVER_TYPE driverType, uint adapter, bool fullscreen, bool vsync, uint antialias, bool multithread)
        {
            bool sucesss = engine_initDriver(driverType, adapter, fullscreen, vsync, antialias, multithread);
            if (!sucesss)
                return false;

            engine_initSceneManager();

            _driver = new Driver(engine_getDriver());
            _sceneManger = new SceneManager(engine_getSceneManager());
            _wowDatabase = new WowDatabase(engine_getWowDatabase());
            _wowEnvironment = new WowEnvironment(engine_getWowEnvironment());
            _manualMeshServices = new ManualMeshServices(engine_getManualMeshServices());
            _audioPlayer = null; // new AudioPlayer(engine_getAudioPlayer());
            _resourceLoader = new ResourceLoader(engine_getResourceLoader());
            _sceneEnvironment = new SceneEnvironment(engine_getSceneEnvironment());
            _drawServices = new DrawServices(engine_getDrawServices());
            _fontServices = new FontServices(engine_getFontServices());

            return true;
        }

        public SVideoSetting VideoSetting
        {
            set { engine_applyVideoSetting(value); }
            get
            {
                SVideoSetting v = new SVideoSetting();
                engine_getVideoSetting(out v);
                return v;
            }
        }

        public SAdvancedSetting AdvancedSetting
        {
            set { engine_applyAdvancedSetting(value); }
            get
            {
                SAdvancedSetting v = new SAdvancedSetting();
                engine_getAdvancedSetting(out v);
                return v;
            }
        }

        public void LoadSetting()
        {
            engine_loadSetting();
        }

        public void SaveSetting()
        {
            engine_saveSetting();
        }

        public string GetBaseDirectory()
        {
            StringBuilder path = new StringBuilder(512);
            Engine_getBaseDirectory(path, (uint)path.Capacity);
            return path.ToString(0, path.Length);
        }

        Driver _driver;
        SceneManager _sceneManger;
        WowDatabase _wowDatabase;
        WowEnvironment _wowEnvironment;
        ManualMeshServices _manualMeshServices;
        AudioPlayer _audioPlayer;
        ResourceLoader _resourceLoader;
        SceneEnvironment _sceneEnvironment;
        DrawServices _drawServices;
        FontServices _fontServices;

        public Driver Driver
        {
            get { return _driver;  }
        }

        public SceneManager SceneManager
        {
            get { return _sceneManger;  }
        }

        public WowDatabase WowDatabase
        {
            get { return _wowDatabase;  }
        }

        public WowEnvironment WowEnvironment
        {
            get { return _wowEnvironment; }
        }

        public ManualMeshServices ManualMeshServices
        {
            get { return _manualMeshServices;  }
        }

        public AudioPlayer AudioPlayer
        {
            get { return _audioPlayer; }
        }

        public ResourceLoader ResourceLoader
        {
            get { return _resourceLoader; }
        }

        public SceneEnvironment SceneEnvironment
        {
            get { return _sceneEnvironment; }
        }

        public DrawServices DrawServices
        {
            get { return _drawServices; }
        }

        public FontServices FontServices
        {
            get { return _fontServices; }
        }

        public bool IsAboveVista
        {
            get { return engine_isAboveVista(); }
        }
    }
}
