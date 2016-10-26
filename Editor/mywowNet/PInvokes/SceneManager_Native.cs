using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace mywowNet
{
    public partial class SceneManager
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_beginFrame")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool SceneManager_beginFrame([MarshalAsAttribute(UnmanagedType.I1)]bool foreground);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_endFrame")]
        public static extern void SceneManager_endFrame();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_drawAll")]
        public static extern void SceneManager_drawAll([MarshalAsAttribute(UnmanagedType.I1)]bool foreground);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_setBackgroundColor")]
        public static extern void SceneManager_setBackgroundColor(
            SColor color);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_getBackgroundColor")]
        public static extern void SceneManager_getBackgroundColor(
            out SColor color);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_getTimeSinceLastFrame")]
        public static extern uint SceneManager_getTimeSinceLastFrame();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_showDebug")]
        public static extern void SceneManager_showDebug(
         E_SCENE_DEBUG_PART part, 
         [MarshalAsAttribute(UnmanagedType.I1)]bool show);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_onWindowSizeChanged")]
        public static extern void SceneManager_onWindowSizeChanged(uint width, uint height);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_removeSceneNode")]
        public static extern void SceneManager_removeSceneNode(
            IntPtr sceneNode);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_removeAllCameras")]
        public static extern void SceneManager_removeAllCameras();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_removeAllSceneNodes")]
        public static extern void SceneManager_removeAllSceneNodes();


        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_setDebugM2Node")]
        public static extern void SceneManager_setDebugM2Node(
            IntPtr sceneNode);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_setDebugWMONode")]
        public static extern void SceneManager_setDebugWMONode(
            IntPtr sceneNode);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_setDebugWDTNode")]
        public static extern void SceneManager_setDebugWDTNode(
            IntPtr sceneNode);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_setDebugMapTileNode")]
        public static extern void SceneManager_setDebugMapTileNode(
            IntPtr sceneNode);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SceneManager_setCenterTexture(
            [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)]string filename, float scale);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SceneManager_setTextureOpaque([MarshalAsAttribute(UnmanagedType.I1)]bool opaque);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool SceneManager_getTextureOpaque();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_setActiveCamera")]
        public static extern void SceneManager_setActiveCamera(
            IntPtr cam);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_getActiveCamera")]
        public static extern IntPtr SceneManager_getActiveCamera();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_addCamera")]
        public static extern IntPtr SceneManager_addCamera(
            vector3df position,
            vector3df lookat,
            vector3df up,
            float near,
            float far,
            float fov);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_addCoordSceneNode")]
        public static extern IntPtr SceneManager_addCoordSceneNode( );

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_addMeshSceneNode")]
        public static extern IntPtr SceneManager_addMeshSceneNode(
            [MarshalAs(UnmanagedType.LPStr, SizeConst = 64)]string name,
            IntPtr parent);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_addM2SceneNode")]
        public static extern IntPtr SceneManager_addM2SceneNode(
            [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)]string meshfilename,
            IntPtr parent,
            [MarshalAsAttribute(UnmanagedType.I1)]bool npc);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_addWMOSceneNode")]
        public static extern IntPtr SceneManager_addWMOSceneNode(
            [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)]string wmofilename,
            IntPtr parent);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_addWDTSceneNode")]
        public static extern IntPtr SceneManager_addWDTSceneNode(
            IntPtr filewdt,
            IntPtr parent);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_addMapTileSceneNode")]
        public static extern IntPtr SceneManager_addMapTileSceneNode(
            IntPtr filewdt,
            int row, 
            int col,
            IntPtr parent);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneManager_setBackImageFileName")]
        public static extern IntPtr SceneManager_setBackImageFileName(
            [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)]string filename);

        [DllImport(DllName, EntryPoint = "SceneManager_getBackImageFileName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr SceneManager_getBackImageFileName();
    }
}