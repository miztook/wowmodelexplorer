using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class FontServices
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "FontServices_onWindowSizeChanged")]
        public static extern void FontServices_onWindowSizeChanged(uint width, uint height);
    }
}