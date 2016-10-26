using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Camera
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_setPosition")]
        public static extern void Camera_setPosition(
            IntPtr cam,
            vector3df pos);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_getPosition")]
        public static extern void Camera_getPosition(
            IntPtr cam,
            out vector3df pos);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_setDir")]
        public static extern void Camera_setDir(
            IntPtr cam,
            vector3df dir);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_getDir")]
        public static extern void Camera_getDir(
            IntPtr cam,
            out vector3df dir);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_setLookat")]
        public static extern void Camera_setLookat(
            IntPtr cam,
            vector3df lookat);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_getLookat")]
        public static extern void Camera_getLookat(
            IntPtr cam,
            out vector3df lookat);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_getUp")]
        public static extern void Camera_getUp(
            IntPtr cam,
            out vector3df up);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_onKeyMove")]
        public static extern void Camera_onKeyMove(
            IntPtr cam,
            float speed,
            SKeyControl keycontrol);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_recalculateAll")]
        public static extern void Camera_recalculateAll(
            IntPtr cam);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_pitch_yaw_Maya")]
        public static extern void Camera_pitch_yaw_Maya(
            IntPtr cam,
            float pitchDegree,
            float yawDegree);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_move_offset_Maya")]
        public static extern void Camera_move_offset_Maya(
            IntPtr cam,
            float xOffset,
            float yOffset);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Camera_pitch_yaw_FPS")]
        public static extern void Camera_pitch_yaw_FPS(
            IntPtr cam,
            float pitchDegree,
            float yawDegree);

        [StructLayoutAttribute(LayoutKind.Sequential)]
        public struct SKeyControl
        {

            /// boolean
            [MarshalAsAttribute(UnmanagedType.I1)]
            public bool front;

            /// boolean
            [MarshalAsAttribute(UnmanagedType.I1)]
            public bool back;

            /// boolean
            [MarshalAsAttribute(UnmanagedType.I1)]
            public bool left;

            /// boolean
            [MarshalAsAttribute(UnmanagedType.I1)]
            public bool right;

            /// boolean
            [MarshalAsAttribute(UnmanagedType.I1)]
            public bool up;

            /// boolean
            [MarshalAsAttribute(UnmanagedType.I1)]
            public bool down;
        }
    }
}