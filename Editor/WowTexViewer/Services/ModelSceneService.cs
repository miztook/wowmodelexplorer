using System;
using mywowNet;
using WowTexViewer.Controls;

namespace WowTexViewer.Services
{
    public delegate void MainTexture_ChangedHandler(object sender, string textureFileName);

    public class ModelSceneService
    {
        private MeshSceneNode gridSceneNode;
        private FileWDT wdt;
        private FileADT adt;
        private string currentTextureName = "";

        public event MainTexture_ChangedHandler MainTexture_Changed;

        public ModelSceneService()
        {
            if (instance == null)
            {
                instance = this;
            }
        }
        private static ModelSceneService instance = null;
        public static ModelSceneService Instance { get { return instance; } }

        public MeshSceneNode GridSceneNode { get { return gridSceneNode; } }
        public FileWDT CurrentWDT { get { return wdt; } }
        public FileADT CurrentADT { get { return adt; } }
        public string CurrentTextureName { get { return currentTextureName; } }

        public void Initialize()
        {
            Engine.Instance.ManualMeshServices.AddGridMesh("$grid20", 20, 1, new SColor(128, 128, 128));

            gridSceneNode = Engine.Instance.SceneManager.AddMeshSceneNode("$grid20", null);
        }

        public FileWDT LoadWDT(string mapname, int mapid)
        {
            if (wdt != null)
            {
                wdt.UnloadADT(adt);
                adt = null;
                Engine.Instance.ResourceLoader.UnloadWDT(wdt);
                wdt = null;
            }
            wdt = Engine.Instance.ResourceLoader.LoadWDT(mapname, mapid, true);
            return wdt;
        }

        public FileADT LoadADTTextures(uint row, uint col)
        {
            if (adt != null)
            {
                wdt.UnloadADT(adt);
                adt = null;
            }
            adt = wdt.LoadADTTextures(row, col);
            return adt;
        }

        public void Shutdown()
        {
            Engine.Instance.SceneManager.RemoveAllSceneNodes();
            Engine.Instance.SceneManager.RemoveAllCameras();

            if (wdt != null)
            {
                wdt.UnloadADT(adt);
                adt = null;
                Engine.Instance.ResourceLoader.UnloadWDT(wdt);
                wdt = null;
            }  
        }

        public void SetCurrentTextureName(string filename, float scale)
        {
            Engine.Instance.SceneManager.SetCenterTexture(filename, scale);            

            currentTextureName = filename;

            if (MainTexture_Changed != null)
                MainTexture_Changed.Invoke(this, currentTextureName);
        }
    }
}
