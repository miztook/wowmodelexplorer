using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class WowDatabase
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getItemCount")]
        public static extern uint WowDatabase_getItemCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getItem")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getItem(
            uint index,
            out SItem item);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getItemById")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getItemById(
            int id,
            out SItem item);


        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_itemIsCorrectType")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_itemIsCorrectType(
            int type,
            int slot);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getNpcCount")]
        public static extern uint WowDatabase_getNpcCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getNpc")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getNpc(
            uint index,
            out SNpc npc);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getNpcById")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getNpcById(
            int id,
            out SNpc npc);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getRaceId", CharSet = CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getRaceId(
            [MarshalAs(UnmanagedType.LPStr)]string race,
            out uint id);

        [DllImport(DllName, EntryPoint = "WowDatabase_getRaceName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr WowDatabase_getRaceName(
            uint raceid);

        [DllImport(DllName, EntryPoint = "WowDatabase_getClassShortName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr WowDatabase_getClassShortName(
            uint classid);


        [DllImport(DllName, EntryPoint = "WowDatabase_getClassInfo", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getClassInfo(
            [MarshalAs(UnmanagedType.LPStr)]string shortname,
            out SEntry entry);
        
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getStartOutfitCount")]
        public static extern uint WowDatabase_getStartOutfitCount(
            uint race,
            [MarshalAsAttribute(UnmanagedType.I1)]bool female);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getStartOutfit")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getStartOutfit(
            uint race,
            [MarshalAsAttribute(UnmanagedType.I1)]bool female,
            uint index,
            out SStartOutfit entry);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getSetCount")]
        public static extern uint WowDatabase_getSetCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getSet")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getSet(
            uint index,
            out SEntry entry);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getMapCount")]
        public static extern uint WowDatabase_getMapCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getMap")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getMap(
            uint index,
            out SMap map);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getWmoCount")]
        public static extern uint WowDatabase_getWmoCount();

        [DllImport(DllName, EntryPoint = "WowDatabase_getWMOFileName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr WowDatabase_getWMOFileName(
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)]bool shortname);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern uint WowDatabase_getWorldModelCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr WowDatabase_getWorldModelFileName(
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)]bool shortname);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getTextureCount")]
        public static extern uint WowDatabase_getTextureCount();

        [DllImport(DllName, EntryPoint = "WowDatabase_getTextureFileName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr WowDatabase_getTextureFileName(
            uint index);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getSpellVisualEffectCount")]
        public static extern uint WowDatabase_getSpellVisualEffectCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getSpellVisualEffectId")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getSpellVisualEffectId(
            uint index,
            out int visualId);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getRidableCount")]
        public static extern uint WowDatabase_getRidableCount();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getRidable")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getRidable(
            uint index,
            out SRidable ridable);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_buildItems")]
        public static extern void WowDatabase_buildItems();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_buildNpcs", CharSet=CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_buildNpcs(
            [MarshalAs(UnmanagedType.LPStr)]string filename);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_buildStartOutfitClass")]
        public static extern void WowDatabase_buildStartOutfitClass();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_buildMaps")]
        public static extern void WowDatabase_buildMaps();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_buildWmos")]
        public static extern void WowDatabase_buildWmos();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void WowDatabase_buildWorldModels();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_buildTextures")]
        public static extern void WowDatabase_buildTextures();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_buildRidables", CharSet = CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_buildRidables(
            [MarshalAs(UnmanagedType.LPStr)]string filename);

        [DllImport(DllName, EntryPoint = "WowDatabase_getCharacterPath", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getCharacterPath(
            [MarshalAs(UnmanagedType.LPStr)]string race,
            [MarshalAs(UnmanagedType.I1)]bool female,
            [MarshalAs(UnmanagedType.I1)]bool isHD,
            StringBuilder path,
            uint size);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WowDatabase_getMaxCharFeature")]
        public static extern void WowDatabase_getMaxCharFeature(
             uint raceid,
             [MarshalAsAttribute(UnmanagedType.I1)]bool female,
             [MarshalAs(UnmanagedType.I1)]bool isHD,
             out SCharFeature feature);

        [DllImport(DllName, EntryPoint = "WowDatabase_getNpcPath", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getNpcPath(
             int npcid,
             [MarshalAs(UnmanagedType.I1)]bool isHD,
             StringBuilder path,
             uint size);

        [DllImport(DllName, EntryPoint = "WowDatabase_getItemVisualPath", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getItemVisualPath(
             int visualId,
             StringBuilder path,
             uint size);

        [DllImport(DllName, EntryPoint = "WowDatabase_getSpellVisualEffectName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr WowDatabase_getSpellVisualEffectName(
             int visualId);

        [DllImport(DllName, EntryPoint = "WowDatabase_getSpellVisualEffectPath", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getSpellVisualEffectPath(
             int visualId,
             StringBuilder path,
             uint size);

        [DllImport(DllName, EntryPoint = "WowDatabase_getItemPath", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WowDatabase_getItemPath(
             int itemid,
             StringBuilder modelpath,
             uint modelSize,
             StringBuilder texpath,
             uint texSize);

    }
}