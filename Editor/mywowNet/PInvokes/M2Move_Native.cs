using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class M2Move
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Move_getDir")]
        public static extern void m2Move_getDir(
            IntPtr m2move,
            out vector3df dir);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Move_setDir")]
        public static extern void m2Move_setDir(
            IntPtr m2move,
            ref vector3df dir);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Move_getPos")]
        public static extern void m2Move_getPos(
            IntPtr m2move,
            out vector3df pos);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Move_setPos")]
        public static extern void m2Move_setPos(
            IntPtr m2move,
            ref vector3df pos);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Move_getScale")]
        public static extern void m2Move_getScale(
            IntPtr m2move,
            out vector3df scale);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Move_setScale")]
        public static extern void m2Move_setScale(
            IntPtr m2move,
            ref vector3df scale);
    }

}