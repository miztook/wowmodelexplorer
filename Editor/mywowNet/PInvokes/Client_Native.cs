using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Client
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Client_create")]
        public static extern void client_create();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Client_destroy")]
        public static extern void client_destroy();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Client_getPlayer")]
        public static extern IntPtr Client_getPlayer();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Client_getWorld")]
        public static extern IntPtr Client_getWorld();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Client_tick")]
        public static extern void Client_tick(uint delta);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Client_onMouseWheel")]
        public static extern void Client_onMouseWheel(float fDelta);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Client_onMouseMove")]
        public static extern void Client_onMouseMove(float pitchDegree, float yawDegree);
    }
}