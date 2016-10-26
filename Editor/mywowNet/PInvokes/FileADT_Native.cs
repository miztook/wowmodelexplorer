using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class FileADT
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "FileADT_getM2Count")]
        public static extern uint FileADT_getM2Count(IntPtr adt);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "FileADT_getWMOCount")]
        public static extern uint FileADT_getWMOCount(IntPtr adt);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "FileADT_getTextureCount")]
        public static extern uint FileADT_getTextureCount(IntPtr adt);

        [DllImport(DllName, EntryPoint = "FileADT_getM2FileName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr FileADT_getM2FileName(
            IntPtr adt,
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)]bool shortname);

        [DllImport(DllName, EntryPoint = "FileADT_getWMOFileName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr FileADT_getWMOFileName(
            IntPtr adt,
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)]bool shortname);

        [DllImport(DllName, EntryPoint = "FileADT_getTextureFileName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr FileADT_getTextureFileName(
            IntPtr adt,
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)]bool shortname);
    }
}
