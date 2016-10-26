using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Engine
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_create")]
        public static extern void engine_create();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_destroy")]
        public static extern void engine_destroy();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getHWnd")]
        public static extern IntPtr engine_getHWnd();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_isAboveVista")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool engine_isAboveVista();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_initDriver")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool engine_initDriver(
            E_DRIVER_TYPE driverType,
            uint adapter,
            [MarshalAsAttribute(UnmanagedType.I1)]bool fullscreen,
            [MarshalAsAttribute(UnmanagedType.I1)]bool vsync,
            uint antialias,
            [MarshalAsAttribute(UnmanagedType.I1)]bool multithread);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_applyVideoSetting")]
        public static extern void engine_applyVideoSetting(
            SVideoSetting videosetting);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getVideoSetting")]
        public static extern void engine_getVideoSetting(
            out SVideoSetting videosetting);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_applyAdvancedSetting")]
        public static extern void engine_applyAdvancedSetting(
            SAdvancedSetting advancedsetting);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getAdvancedSetting")]
        public static extern void engine_getAdvancedSetting(
            out SAdvancedSetting advancedsetting);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_loadSetting")]
        public static extern void engine_loadSetting();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_saveSetting")]
        public static extern void engine_saveSetting();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_initSceneManager")]
        public static extern void engine_initSceneManager();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getDriver")]
        public static extern IntPtr engine_getDriver();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getSceneManager")]
        public static extern IntPtr engine_getSceneManager();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getWowDatabase")]
        public static extern IntPtr engine_getWowDatabase();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getWowEnvironment")]
        public static extern IntPtr engine_getWowEnvironment();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getManualMeshServices")]
        public static extern IntPtr engine_getManualMeshServices();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getAudioPlayer")]
        public static extern IntPtr engine_getAudioPlayer();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getResourceLoader")]
        public static extern IntPtr engine_getResourceLoader();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getSceneEnvironment")]
        public static extern IntPtr engine_getSceneEnvironment();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getDrawServices")]
        public static extern IntPtr engine_getDrawServices();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getFontServices")]
        public static extern IntPtr engine_getFontServices();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Engine_getBaseDirectory")]
        public static extern void Engine_getBaseDirectory(StringBuilder path,
             uint size);
    }
}