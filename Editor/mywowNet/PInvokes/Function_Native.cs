using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Function
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, EntryPoint = "Function_getFileDirA", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Function_getFileDirA(
            [MarshalAs(UnmanagedType.LPStr)]string filename,
            StringBuilder outfilename,
            uint size);

        [DllImport(DllName, EntryPoint = "Function_getFileNameA", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Function_getFileNameA(
            [MarshalAs(UnmanagedType.LPStr)]string filename,
            StringBuilder outfilename,
            uint size);

        [DllImport(DllName, EntryPoint = "Function_getFileNameNoExtensionA", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Function_getFileNameNoExtensionA(
            [MarshalAs(UnmanagedType.LPStr)]string filename,
            StringBuilder outfilename,
            uint size);

        [DllImport(DllName, EntryPoint = "Function_hasFileExtensionA", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool Function_hasFileExtensionA(
            [MarshalAs(UnmanagedType.LPStr)]string filename,
            [MarshalAs(UnmanagedType.LPStr)]string ext);
    }
}
