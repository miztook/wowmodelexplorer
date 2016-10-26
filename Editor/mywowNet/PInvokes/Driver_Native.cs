using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Driver
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Driver_setDisplayMode")]
        public static extern void Driver_setDisplayMode(uint width, uint height);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Driver_getDisplayMode")]
        public static extern void Driver_getDisplayMode(out uint width, out uint height);


    }
}