using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class M2Instance
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Instance_setCharFeature")]
        public static extern void m2Instance_setCharFeature(
            IntPtr node,
            ref SCharFeature feature);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Instance_getCharFeature")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool m2Instance_getCharFeature(
            IntPtr node,
            out SCharFeature feature);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Instance_getItemSlot")]
        public static extern int m2Instance_getItemSlot(
            IntPtr node,
            int itemid);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Instance_getSlotItemId")]
        public static extern int m2Instance_getSlotItemId(
            IntPtr node,
            int slot);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Instance_slotHasModel")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool m2Instance_slotHasModel(
            IntPtr node,
            int slot);
    }
}
