using System;
using mywowNet;
using WowMapExplorer.Controls;

namespace WowMapExplorer.Services
{
    public delegate void MainWDTSceneNode_ChangedHandler(object sender, WDTSceneNode node);
    public delegate void MainMapTileSceneNode_ChangedHandler(object sender, MapTileSceneNode node);

    class MapSceneService
    {
        private CoordSceneNode coordSceneNode;
        private CoordSceneNode lightCoordSceneNode;
        private MapTileSceneNode mapTileSceneNode;
        private FileWDT wdt;

        public MapSceneService()
        {
            if (instance == null)
            {
                instance = this;
            }
         }
        private static MapSceneService instance = null;
        public static MapSceneService Instance { get { return instance; } }

        public CoordSceneNode CoordSceneNode { get { return coordSceneNode; } }
        public CoordSceneNode LightCoordSceneNode { get { return lightCoordSceneNode; } }
        public FileWDT CurrentWDT { get { return wdt; } }
        public MapTileSceneNode MapTileSceneNode { get { return mapTileSceneNode; } }

        public void Initialize()
        {
            coordSceneNode = Engine.Instance.SceneManager.AddCoordSceneNode();
            coordSceneNode.SetPosition2D(E_POSITION_2D.EP2D_BOTTOMLEFT, 18);
            coordSceneNode.SetAxisText("x", "y", "z");

            lightCoordSceneNode = Engine.Instance.SceneManager.AddCoordSceneNode();
            lightCoordSceneNode.Visible = false;
            lightCoordSceneNode.SetPosition2D(E_POSITION_2D.EP2D_TOPCENTER, 18);
            lightCoordSceneNode.SetAxisVisible(false, false, true);
            lightCoordSceneNode.SetAxisColor(new SColor(), new SColor(), new SColor(255, 255, 255));
            lightCoordSceneNode.SetAxisText("", "", "light");
            lightCoordSceneNode.SetAxisDir(E_AXIS.ESA_Z, Engine.Instance.SceneEnvironment.LightDir);
        }

        public void Shutdown()
        {
            Engine.Instance.SceneManager.RemoveAllSceneNodes();
            Engine.Instance.SceneManager.RemoveAllCameras();

            if (wdt != null)
            {
                Engine.Instance.ResourceLoader.UnloadWDT(wdt);
                wdt = null;
            }      
        }

        public FileWDT LoadWDT(string mapname, int mapid)
        {
            if (wdt != null)
            {
                Engine.Instance.ResourceLoader.UnloadWDT(wdt);
                wdt = null;
            }
            wdt = Engine.Instance.ResourceLoader.LoadWDT(mapname, mapid, false);
            return wdt;
        }

        public event MainMapTileSceneNode_ChangedHandler MainMapTileSceneNodeChanged;

        private void RemoveMainSceneNode()
        {
            if (mapTileSceneNode != null)
            {
                Engine.Instance.SceneManager.RemoveSceneNode(mapTileSceneNode);
                mapTileSceneNode = null;
            }
        }

        public MapTileSceneNode SetMainMapTileSceneNode(FileWDT wdt, int row, int col)
        {
            RemoveMainSceneNode();

            MapTileSceneNode node = Engine.Instance.SceneManager.AddMapTileSceneNode(wdt, row, col, null);

            if (node != null)
            {
                vector3df pos = node.GetCenter();

                Camera cam  = EngineService.Instance.MainCamera;
                if (cam != null)
                {
                    cam.Position = pos + new vector3df(0, 10, 20);
                    cam.Dir = new vector3df(0, -5, 10);
                    cam.RecalculateAll();

                    cam.LookAt = pos;
                }
            }

            Engine.Instance.SceneManager.SetDebugMapTileSceneNode(node);

            mapTileSceneNode = node;

            RaiseMainMapTileSceneNodeChanged();

            return node;
        }

        private void RaiseMainMapTileSceneNodeChanged()
        {
            if (MainMapTileSceneNodeChanged != null)
            {
                MainMapTileSceneNodeChanged.Invoke(this, mapTileSceneNode);
            }
        }

       
    }
}
