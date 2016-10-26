using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class WowEnvironment
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowEnvironment_Exists")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool WowEnvironment_Exists(
            [MarshalAs(UnmanagedType.LPStr)] string filename);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowEnvironment_GetCascFileCount")]
        public static extern uint WowEnvironment_GetCascFileCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowEnvironment_GetCascFile")]
        public static extern IntPtr WowEnvironment_GetCascFile(int index);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void WowEnvironment_ClearOwnCascFiles();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void WowEnvironment_AddOwnCascFile([MarshalAs(UnmanagedType.LPStr)]string filename);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void WowEnvironment_FinishOwnCascFiles();


        [DllImport(DllName, EntryPoint = "WowEnvironment_GetFileCount_InDir", CallingConvention = CallingConvention.Cdecl)]
        public static extern int WowEnvironment_GetFileCount_InDir(
            [MarshalAs(UnmanagedType.LPStr)]string baseDir,
            [MarshalAs(UnmanagedType.LPStr)]string ext,
            [MarshalAs(UnmanagedType.I1)]bool useOwn);

        [DllImport(DllName, EntryPoint = "WowEnvironment_GetFile_InDir", CallingConvention = CallingConvention.Cdecl)]
        public static extern void WowEnvironment_GetFile_InDir(
            [MarshalAs(UnmanagedType.I1)]bool fullname,
            int index,
            StringBuilder path,
            uint size);

        [DllImport(DllName, EntryPoint = "WowEnvironment_GetDirectoryCount_InDir", CallingConvention = CallingConvention.Cdecl)]
        public static extern int WowEnvironment_GetDirectoryCount_InDir(
            [MarshalAs(UnmanagedType.LPStr)]string baseDir,
            [MarshalAs(UnmanagedType.I1)]bool useOwn);

        [DllImport(DllName, EntryPoint = "WowEnvironment_GetDirectory_InDir", CallingConvention = CallingConvention.Cdecl)]
        public static extern void WowEnvironment_GetDirectory_InDir(
            [MarshalAs(UnmanagedType.I1)]bool fullname,
            int index,
            StringBuilder path,
            uint size);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowEnvironment_GetLocale")]
        public static extern IntPtr WowEnvironment_GetLocale();
    }
}
