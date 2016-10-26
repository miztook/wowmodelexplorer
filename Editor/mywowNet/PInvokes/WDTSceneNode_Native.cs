using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Text;

namespace mywowNet
{
    public partial class WDTSceneNode
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WDTSceneNode_setCurrentTile")]
        public static extern void WDTSceneNode_setCurrentTile(
            IntPtr node,
            int row,
            int col);


        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "WDTSceneNode_getCenter")]
        public static extern void WDTSceneNode_getCenter(
            IntPtr node,
            out vector3df pos);
    }
}