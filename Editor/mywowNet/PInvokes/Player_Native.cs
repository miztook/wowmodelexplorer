using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Player
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Player_getTargetCamera")]
        public static extern IntPtr Player_getTargetCamera(IntPtr player);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Player_setTargetCamera")]
        public static extern void Player_setTargetCamera(IntPtr player, float nearValue, float farValue, float fov);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Player_setM2AsTarget")]
        public static extern void Player_setM2AsTarget(IntPtr player, IntPtr m2SceneNode, float distance, float rad);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Player_setMoveSpeed")]
        public static extern void Player_setMoveSpeed(IntPtr player, float walk, float run, float backwards, float roll);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Player_getMoveSpeed")]
        public static extern void Player_getMoveSpeed(IntPtr player, out float walk, out float run, out float backwards, out float roll);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Player_rideOnModel")]
        public static extern void Player_rideOnModel(IntPtr player, int npcid, E_M2_STATES state);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Player_getRideNpcId")]
        public static extern int Player_getRideNpcId(IntPtr player);
    }
}