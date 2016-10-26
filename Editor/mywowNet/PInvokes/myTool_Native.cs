using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class mwTool
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "mwTool_create")]
        public static extern void mwTool_create();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "mwTool_destroy")]
        public static extern void mwTool_destroy();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "mwTool_openUrlToJsonFile", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool mwTool_openUrlToJsonFile(
            [MarshalAs(UnmanagedType.LPStr)] string url,
            [MarshalAs(UnmanagedType.LPStr)] string filename);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "mwTool_parseCharacterArmoryInfo", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool mwTool_parseCharacterArmoryInfo(
            [MarshalAs(UnmanagedType.LPStr)] string filename,
            out SCharacterArmoryInfo charInfo);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "mwTool_exportBlpAsTga", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool mwTool_exportBlpAsTga(
            [MarshalAs(UnmanagedType.LPStr)] string blpfilename,
            [MarshalAs(UnmanagedType.LPStr)] string tgafilename,
            [MarshalAs(UnmanagedType.I1)] bool bApha);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "mwTool_exportBlpAsTgaDir", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool mwTool_exportBlpAsTgaDir(
            [MarshalAs(UnmanagedType.LPStr)] string blpfilename,
            [MarshalAs(UnmanagedType.LPStr)] string dirname,
            [MarshalAs(UnmanagedType.I1)] bool bApha);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "mwTool_exportM2SceneNodeToOBJ", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool mwTool_exportM2SceneNodeToOBJ(
            IntPtr m2SceneNode,
            [MarshalAs(UnmanagedType.LPStr)] string dirname);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "mwTool_exportWMOSceneNodeToOBJ", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool mwTool_exportWMOSceneNodeToOBJ(
            IntPtr wmoSceneNode,
            [MarshalAs(UnmanagedType.LPStr)] string dirname);
    }
}