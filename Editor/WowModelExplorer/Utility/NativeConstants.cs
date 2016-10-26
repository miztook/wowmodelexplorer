using System;
using System.Windows.Interop;
using System.Runtime.InteropServices;

namespace WowModelExplorer.Utility
{
    public class NativeConstants
    {
        public const int WM_SIZE = 5;
        public const int WM_MOUSEMOVE = 512;
        public const int WM_LBUTTONDOWN = 513;
        public const int WM_LBUTTONUP = 514;
        public const int WM_MOUSEWHEEL = 522;
        public const int WM_RBUTTONDOWN = 0x204;
        public const int WM_RBUTTONUP = 0x205;
        public const int WM_CAPTURECHANGED = 0x215;

        public const int GWL_STYLE = -16;
        public const int WS_VISIBLE = 0x10000000;
        public const int WS_CHILD = 0x40000000;
        public const int WM_DESTROY = 0x0002;

        public const int VK_SPACE = 32;
    }
}
