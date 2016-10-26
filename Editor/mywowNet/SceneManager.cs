using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace mywowNet
{
    public partial class SceneManager : INative
    {
        public SceneManager(IntPtr raw) : base(raw) { }

        public bool BeginFrame(bool foreground)
        {
            return SceneManager_beginFrame(foreground);
        }

        public void EndFrame()
        {
            SceneManager_endFrame();
        }

        public void DrawAll(bool foreground)
        {
            SceneManager_drawAll(foreground);
        }

        public static SColor DefaultBackgroundColor = new SColor(64, 64, 64);
        public SColor BackgroundColor
        {
            set { SceneManager_setBackgroundColor(value); }
            get
            {
                SColor c = new SColor();
                SceneManager_getBackgroundColor(out c);
                return c;
            }
        }

        public uint TimeSinceLastFrame
        {
            get 
            {
                return SceneManager_getTimeSinceLastFrame();
            }
        }

        public void ShowDebug(E_SCENE_DEBUG_PART part, bool show)
        {
            SceneManager_showDebug(part, show);
        }

        public void OnWindowSizeChanged(uint width, uint height)
        {
            SceneManager_onWindowSizeChanged(width, height);
        }

        public void SetDebugM2SceneNode(M2SceneNode node)
        {
            SceneManager_setDebugM2Node(node != null ? node.pointer : IntPtr.Zero);
        }

        public void SetDebugWMOSceneNode(WMOSceneNode node)
        {
            SceneManager_setDebugWMONode(node != null ? node.pointer : IntPtr.Zero);
        }

        public void SetDebugWDTSceneNode(WDTSceneNode node)
        {
            SceneManager_setDebugWDTNode(node != null ? node.pointer : IntPtr.Zero);
        }

        public void SetDebugMapTileSceneNode(MapTileSceneNode node)
        {
            SceneManager_setDebugMapTileNode(node != null ? node.pointer : IntPtr.Zero);
        }

        public void SetCenterTexture(string filename, float scale)
        {
            SceneManager_setCenterTexture(filename, scale);
        }

        public bool TextureOpaque
        {
            get { return SceneManager_getTextureOpaque(); }
            set { SceneManager_setTextureOpaque(value); }
        }

        Camera _activeCam = new Camera();
        public Camera ActiveCamera
        {
            get
            {
                IntPtr cam = SceneManager_getActiveCamera();
                if (cam == IntPtr.Zero)
                    return null;

                _activeCam.FromRaw(cam);
                return _activeCam;
            }
            set { SceneManager_setActiveCamera(value.pointer);  }
        }

        public void RemoveSceneNode(SceneNode node)
        {
            SceneManager_removeSceneNode(node.pointer);
        }

        public void RemoveAllSceneNodes()
        {
            SceneManager_removeAllSceneNodes();
        }

        public void RemoveAllCameras()
        {
            SceneManager_removeAllCameras();
        }

        public Camera AddCamera(vector3df position, vector3df lookat, vector3df up, float near, float far, float fov)
        {
            IntPtr raw = SceneManager_addCamera(position, lookat, up, near, far, fov);
            return new Camera(raw);
        }

        public CoordSceneNode AddCoordSceneNode()
        {
            IntPtr raw = SceneManager_addCoordSceneNode();
            if (raw == IntPtr.Zero)
                return null;
            return new CoordSceneNode(raw);
        }

        public MeshSceneNode AddMeshSceneNode(string name, SceneNode parent)
        {
            IntPtr raw = SceneManager_addMeshSceneNode(name, parent == null ? IntPtr.Zero : parent.pointer);
            if (raw == IntPtr.Zero)
                return null;
            return new MeshSceneNode(raw);
        }

        public M2SceneNode AddM2SceneNode(string meshfilename, SceneNode parent, bool npc)
        {
            IntPtr raw = SceneManager_addM2SceneNode(meshfilename, parent == null ? IntPtr.Zero : parent.pointer, npc);
            if (raw == IntPtr.Zero)
                return null;
            return new M2SceneNode(raw);
        }

        public WMOSceneNode AddWMOSceneNode(string wmofilename, SceneNode parent)
        {
            IntPtr raw = SceneManager_addWMOSceneNode(wmofilename, parent == null ? IntPtr.Zero : parent.pointer);
            if (raw == IntPtr.Zero)
                return null;
            return new WMOSceneNode(raw);
        }

        public WDTSceneNode AddWDTSceneNode(FileWDT filewdt, SceneNode parent)
        {
            IntPtr raw = SceneManager_addWDTSceneNode(filewdt.pointer, parent == null ? IntPtr.Zero : parent.pointer);
            if (raw == IntPtr.Zero)
                return null;
            return new WDTSceneNode(raw);
        }

        public MapTileSceneNode AddMapTileSceneNode(FileWDT filewdt, int row, int col, SceneNode parent)
        {
            IntPtr raw = SceneManager_addMapTileSceneNode(filewdt.pointer, row, col, parent == null ? IntPtr.Zero : parent.pointer);
            if (raw == IntPtr.Zero)
                return null;
            return new MapTileSceneNode(raw);
        }

        public string BackImageFileName
        {
            set { SceneManager_setBackImageFileName(value);  }
            get
            {
                return Marshal.PtrToStringAnsi(SceneManager_getBackImageFileName());
            }
        }
    }
}
