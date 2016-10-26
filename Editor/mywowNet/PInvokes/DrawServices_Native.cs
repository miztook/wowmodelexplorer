using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class DrawServices
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "DrawServices_setLineZTestEnable")]
        public static extern void DrawServices_setLineZTestEnable(
            [MarshalAs(UnmanagedType.I1)]bool enable);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "DrawServices_getLineZTestEnable")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool DrawServices_getLineZTestEnable();
    }
}