using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Text;

namespace mywowNet
{
    public partial class WMOSceneNode
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_showEditPart")]
        public static extern void WMOSceneNode_showEditPart(
            IntPtr node,
            E_WMO_EDIT_PART editpart,
            [MarshalAsAttribute(UnmanagedType.I1)]bool show);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_setWireframe")]
        public static extern void WMOSceneNode_setWireframe(
            IntPtr node,
            E_OVERRIDE_WIREFRAME wireframe);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_getFileWMO")]
        public static extern void WMOSceneNode_getFileWMO(
            IntPtr node,
            out SFileWMO filewmo);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_getGroup")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WMOSceneNode_getGroup(
            IntPtr node,
            uint index,
            out SWMOGroup group);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_showGroup")]
        public static extern void WMOSceneNode_showGroup(
            IntPtr node,
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)]bool show);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_isGroupShow")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WMOSceneNode_isGroupShow(
            IntPtr node,
            uint index);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_showGroupBox")]
        public static extern void WMOSceneNode_showGroupBox(
            IntPtr node,
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)]bool show,
            SColor color);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_isGroupBoxShow")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WMOSceneNode_isGroupBoxShow(
            IntPtr node,
            uint index);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_getPortal")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WMOSceneNode_getPortal(
            IntPtr node,
            uint index,
            out SWMOPortal portal);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_showPortal")]
        public static extern void WMOSceneNode_showPortal(
            IntPtr node,
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)]bool show,
            SColor color);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_isPortalShow")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WMOSceneNode_isPortalShow(
            IntPtr node,
            uint index);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_showPortalGroups")]
        public static extern void WMOSceneNode_showPortalGroups(
            IntPtr node,
            uint index,
            [MarshalAsAttribute(UnmanagedType.I1)]bool show);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WMOSceneNode_isPortalGroupsShow")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WMOSceneNode_isPortalGroupsShow(
            IntPtr node,
            uint index);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "WMOSceneNode_getDoodadName")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool WMOSceneNode_getDoodadName(
            IntPtr node,
            uint index,
            StringBuilder path,
            uint size);
    }
}
