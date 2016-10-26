using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Collections.ObjectModel;
using mywowNet;

namespace WowModelExplorer.Controls.Settings
{
    public class OptionBool
    {
        public bool On { get; set; }
        public override string ToString()
        {
            if (On)
                return Application.Current.FindResource("on") as string;
            else
                return Application.Current.FindResource("off") as string;
        }
    }
    public class OptionBools : ObservableCollection<OptionBool>
    {
        public OptionBools()
        {
            Add(new OptionBool() { On = false });
            Add(new OptionBool() { On = true });
        }
    }

    public class OptionLevel
    {
        public E_LEVEL Level { get; set; }
        public override string ToString()
        {
            switch (Level)
            {
                case E_LEVEL.EL_DISABLE:
                    return Application.Current.FindResource("off") as string;
                case E_LEVEL.EL_LOW:
                    return Application.Current.FindResource("level1") as string;
                case E_LEVEL.EL_FAIR:
                    return Application.Current.FindResource("level2") as string;
                case E_LEVEL.EL_GOOD:
                    return Application.Current.FindResource("level3") as string;
                case E_LEVEL.EL_HIGH:
                    return Application.Current.FindResource("level4") as string;
                case E_LEVEL.EL_ULTRA:
                    return Application.Current.FindResource("level5") as string;
                default:
                    return base.ToString();
            }
        }
    }

    public class OptionLevels : ObservableCollection<OptionLevel>
    {
        public void Default()
        {
            Clear();
            Add(new OptionLevel() { Level = E_LEVEL.EL_LOW });
            Add(new OptionLevel() { Level = E_LEVEL.EL_FAIR });
            Add(new OptionLevel() { Level = E_LEVEL.EL_GOOD });
            Add(new OptionLevel() { Level = E_LEVEL.EL_HIGH });
            Add(new OptionLevel() { Level = E_LEVEL.EL_ULTRA });
        }
    }

    public class DisplayMode
    {
        public bool WindowMode { get; set; }

        public override string ToString()
        {
            if (WindowMode)
                return Application.Current.FindResource("windowMode") as string;
            else
                return Application.Current.FindResource("fullscreenMode") as string;
        }
    }
    public class DisplayModes : ObservableCollection<DisplayMode>
    {
    }

    public class Resolution
    {
        public uint Width { get; set; }
        public uint Height { get; set; }

        public override string ToString() { return String.Format("{0}X{1}", Width, Height); }
    }
    public class Resolutions : ObservableCollection<Resolution>
    {
    }

    public class RefreshRate
    {
        public int Rate { get; set; }

        public override string ToString() { return String.Format("{0}Hz", Rate); }
    }
    public class RefreshRates : ObservableCollection<RefreshRate>
    {
        public void Default()
        {
            Clear();
            Add(new RefreshRate() { Rate = 60 });
        }
    }

    public class VideoDevice
    {
        public int Index { get; set; }
        public override string ToString()
        {
            if (Index == 0)
                return Application.Current.FindResource("mainMonitor") as string;
            else
                return String.Format("{0}{1}", Application.Current.FindResource("monitor"), Index);

        }
    }

    public class VideoDevices : ObservableCollection<VideoDevice>
    {
    }

    public class MultiSample
    {
        public uint Sample { get; set; }

        public override string ToString() { return String.Format("{0}x", Sample); }
    }

    public class MultiSamples : ObservableCollection<MultiSample>
    {
        public void Default()
        {
            Clear();
            Add(new MultiSample() { Sample = 1 });
            Add(new MultiSample() { Sample = 2 });
            Add(new MultiSample() { Sample = 4 });
            Add(new MultiSample() { Sample = 8 });
        }
    }

    public class TextureFilter
    {
        public E_TEXTURE_FILTER Filter { get; set; }

        public override string ToString()
        {
            switch (Filter)
            {
                case E_TEXTURE_FILTER.ETF_BILINEAR:
                    return Application.Current.FindResource("bilinear") as string;
                case E_TEXTURE_FILTER.ETF_TRILINEAR:
                    return Application.Current.FindResource("trilinear") as string;
                case E_TEXTURE_FILTER.ETF_ANISOTROPIC_X2:
                    return Application.Current.FindResource("anisotropicX2") as string;
                case E_TEXTURE_FILTER.ETF_ANISOTROPIC_X4:
                    return Application.Current.FindResource("anisotropicX4") as string;
                case E_TEXTURE_FILTER.ETF_ANISOTROPIC_X8:
                    return Application.Current.FindResource("anisotropicX8") as string;
                case E_TEXTURE_FILTER.ETF_ANISOTROPIC_X16:
                    return Application.Current.FindResource("anisotropicX16") as string;
                default:
                    return base.ToString();
            }
        }
    }

    public class TextureFilters : ObservableCollection<TextureFilter>
    {
        public TextureFilters()
        {
            Add(new TextureFilter() { Filter = E_TEXTURE_FILTER.ETF_BILINEAR });
            Add(new TextureFilter() { Filter = E_TEXTURE_FILTER.ETF_TRILINEAR });
            Add(new TextureFilter() { Filter = E_TEXTURE_FILTER.ETF_ANISOTROPIC_X2 });
            Add(new TextureFilter() { Filter = E_TEXTURE_FILTER.ETF_ANISOTROPIC_X4 });
            Add(new TextureFilter() { Filter = E_TEXTURE_FILTER.ETF_ANISOTROPIC_X8 });
            Add(new TextureFilter() { Filter = E_TEXTURE_FILTER.ETF_ANISOTROPIC_X16 });
        }
    }

    public class GraphicsInterface
    {
        public E_DRIVER_TYPE DriverType { get; set; }

        public override string ToString()
        {
            switch (DriverType)
            {
                case E_DRIVER_TYPE.Direct3D9:
                    return "DirectX 9";
                case E_DRIVER_TYPE.Direct3D11:
                    return "DirectX 11";
                case E_DRIVER_TYPE.OpenGL:
                    return "OpenGL";
                case E_DRIVER_TYPE.GLES2:
                    return "GLES2";
                default:
                    return base.ToString();
            }
        }
    }

     public class GraphicsInterfaces : ObservableCollection<GraphicsInterface>
    {
     }
}
