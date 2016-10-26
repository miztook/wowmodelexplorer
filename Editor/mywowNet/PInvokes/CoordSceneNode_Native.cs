using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Text;

namespace mywowNet
{
    public partial class CoordSceneNode
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CoordSceneNode_setPosition2D")]
        public static extern void CoordSceneNode_setPosition2D(IntPtr node, E_POSITION_2D position, float distance);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CoordSceneNode_setAxisColor")]
        public static extern void CoordSceneNode_setAxisColor(IntPtr node, SColor colorX, SColor colorY, SColor colorZ);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CoordSceneNode_setAxisText")]
        public static extern void CoordSceneNode_setAxisText(IntPtr node, 
            [In] [MarshalAs(UnmanagedType.LPStr)] string textX, 
            [In] [MarshalAs(UnmanagedType.LPStr)] string textY, 
            [In] [MarshalAs(UnmanagedType.LPStr)] string textZ);


        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CoordSceneNode_setAxisVisible")]
        public static extern void CoordSceneNode_setAxisVisible(IntPtr node,
            [MarshalAs(UnmanagedType.I1)] bool xVisible,
            [MarshalAs(UnmanagedType.I1)] bool yVisible, 
            [MarshalAs(UnmanagedType.I1)] bool zVisible);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CoordSceneNode_setDir")]
        public static extern void CoordSceneNode_setDir(IntPtr node,
            E_AXIS axis,
            vector3df dir);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CoordSceneNode_getDir")]
        public static extern void CoordSceneNode_getDir(IntPtr node,
            E_AXIS axis,
            out vector3df dir);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "CoordSceneNode_pitch_yaw_FPS")]
        public static extern void CoordSceneNode_pitch_yaw_FPS(
            IntPtr node,
            float pitchDegree,
            float yawDegree);
    }
}
