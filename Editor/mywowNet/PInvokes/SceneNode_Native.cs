using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class SceneNode
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneNode_setVisible")]
        public static extern void SceneNode_setVisible(
            IntPtr node,
            [MarshalAs(UnmanagedType.I1)]bool visible);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneNode_getVisible")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool SceneNode_getVisible(
            IntPtr node);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneNode_setRelativeTransformation")]
        public static extern void SceneNode_setRelativeTransformation(
            IntPtr node,
            ref matrix4 mat);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneNode_getRelativeTransformation")]
        public static extern void SceneNode_getRelativeTransformation(
            IntPtr node,
            out matrix4 mat);


        [System.Runtime.InteropServices.DllImportAttribute(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneNode_update")]
        public static extern void SceneNode_update(IntPtr sceneNode, [MarshalAs(UnmanagedType.I1)] bool includeChildren);
    }
}