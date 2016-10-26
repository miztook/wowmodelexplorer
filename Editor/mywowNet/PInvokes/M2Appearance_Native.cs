using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class M2Appearance
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Appearance_takeItem")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool m2Appearance_takeItem(
            IntPtr node,
            int itemid);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Appearance_loadStartOutfit")]
        public static extern void m2Appearance_loadStartOutfit(
            IntPtr node,
            int startId,
            [MarshalAsAttribute(UnmanagedType.I1)]bool deathknight);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Appearance_loadSet")]
        public static extern void m2Appearance_loadSet(
            IntPtr node,
            int setid);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Appearance_sheathLeftWeapon")]
        public static extern void m2Appearance_sheathLeftWeapon(
            IntPtr node,
            [MarshalAsAttribute(UnmanagedType.I1)]bool sheath);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Appearance_sheathRightWeapon")]
        public static extern void m2Appearance_sheathRightWeapon(
            IntPtr node,
            [MarshalAsAttribute(UnmanagedType.I1)]bool sheath);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Appearance_isLeftWeaponSheathed")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool m2Appearance_isLeftWeaponSheathed(
            IntPtr node);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Appearance_isRightWeaponSheathed")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool m2Appearance_isRightWeaponSheathed(
            IntPtr node);


    }

}