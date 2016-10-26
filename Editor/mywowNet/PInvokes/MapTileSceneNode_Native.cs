using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Text;

namespace mywowNet
{
    public partial class MapTileSceneNode
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "MapTileSceneNode_getCenter")]
        public static extern void MapTileSceneNode_getCenter(
            IntPtr node,
            out vector3df pos);
    }
}
