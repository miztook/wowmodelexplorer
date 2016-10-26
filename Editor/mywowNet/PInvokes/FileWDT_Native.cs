using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class FileWDT
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "FileWDT_getTileCount")]
        public static extern uint FileWDT_getTileCount(IntPtr wdt);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "FileWDT_getTile")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool FileWDT_getTile(IntPtr wdt, uint index, out uint row, out uint col);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "FileWDT_loadADT")]
        public static extern IntPtr FileWDT_loadADT(IntPtr wdt, uint row, uint col, [MarshalAs(UnmanagedType.I1)] bool simple);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "FileWDT_loadADTTextures")]
        public static extern IntPtr FileWDT_loadADTTextures(IntPtr wdt, uint row, uint col);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "FileWDT_unloadADT")]
        public static extern void FileWDT_unloadADT(IntPtr wdt, IntPtr adt);
    }
}
