using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Windows.Media;
using mywowNet;

namespace WowTexViewer.Utility
{
    static class EngineExtension
    {
        public static SColor ToSColor(this Color color)
        {
            uint c = (uint)(((color.A & 0xff) << 24) |
                      ((color.R & 0xff) << 16) |
                      ((color.G & 0xff) << 8) |
                       (color.B & 0xff));

            return new SColor { color = c };
        }
    }
}
