using System;
using mywowNet;
using WowModelExplorer.Controls;

namespace WowModelExplorer.Services
{
    public delegate void MainM2SceneNode_ChangedHandler(object sender, M2SceneNode node);
    public delegate void MainWMOSceneNode_ChangedHandler(object sender, WMOSceneNode node);

    class ModelSceneService
    {
        private CoordSceneNode coordSceneNode;
        private CoordSceneNode lightCoordSceneNode;
        private MeshSceneNode gridSceneNode;
        private M2SceneNode mainM2SceneNode;
        private WMOSceneNode mainWMOSceneNode;
        private FileWDT wdt;
        private FileADT adt;

        public ModelSceneService()
        {
            if (instance == null)
            {
                instance = this;
            }
         }
        private static ModelSceneService instance = null;
        public static ModelSceneService Instance { get { return instance; } }

        public CoordSceneNode CoordSceneNode { get { return coordSceneNode; } }
        public CoordSceneNode LightCoordSceneNode { get { return lightCoordSceneNode; } }
        public MeshSceneNode GridSceneNode { get { return gridSceneNode; } }
        public M2SceneNode MainM2SceneNode { get { return mainM2SceneNode; } }
        public WMOSceneNode MainWMOSceneNode { get { return mainWMOSceneNode; } }
        public FileWDT CurrentWDT { get { return wdt; } }
        public FileADT CurrentADT { get { return adt; } }

        public event MainM2SceneNode_ChangedHandler MainM2SceneNodeChanged;

        private void RemoveMainSceneNode()
        {
            if (mainM2SceneNode != null)
            {
                Engine.Instance.SceneManager.RemoveSceneNode(mainM2SceneNode);
                mainM2SceneNode = null;
            }

            if (mainWMOSceneNode != null)
            {
                Engine.Instance.SceneManager.RemoveSceneNode(mainWMOSceneNode);
                mainWMOSceneNode = null;
            }
        }

        public M2SceneNode SetMainM2SceneNode(string filename, bool npc)
        {
            if (Client.Instance.GetPlayer().GetRideNpcId() != 0)
            {
                Client.Instance.GetPlayer().RideOnModel(0, E_M2_STATES.EMS_STAND);
            }

            RemoveMainSceneNode();

            M2SceneNode node = Engine.Instance.SceneManager.AddM2SceneNode(filename, null, npc);
           
            if (node != null)
            {
                switch(node.Type)
                {
                    case M2Type.MT_INTERFACE:
                        {
                            if (node.M2Fsm != null)
                            {
                                node.M2Fsm.ResetState();
                            }
                            node.PlayAnimation("Stand", 0, true, 0);
                            node.SetCamera(0);
                        }
                        break;
                    default:
                        {
                            M2Move move =  node.M2Move;
                            if (move != null)
                            {
                                move.Dir = new vector3df(0, 0, -1);
                            }
                            else
                            {
                                matrix4 mat = new matrix4(true);
                                mat.RotationDegrees = new vector3df(0, 90, 0);
                                node.SetRelativeTransformation(ref mat);
                            }

                            if (node.M2Fsm != null)
                            {
                                node.M2Fsm.ResetState();
                            }
                            node.PlayAnimation("Stand", 0, true, 0);
                        }
                        break;
                }
            }

            Engine.Instance.SceneManager.SetDebugM2SceneNode(node);

            mainM2SceneNode = node;

            RaiseMainM2SceneNodeChanged();

            return node;
        }

        private void RaiseMainM2SceneNodeChanged()
        {
            if (MainM2SceneNodeChanged != null)
            {
                MainM2SceneNodeChanged.Invoke(this, mainM2SceneNode);
            }
        }

        public event MainWMOSceneNode_ChangedHandler MainWMOSceneNodeChanged;

        public WMOSceneNode SetMainWMOSceneNode(string filename)
        {
            if (Client.Instance.GetPlayer().GetRideNpcId() != 0)
            {
                Client.Instance.GetPlayer().RideOnModel(0, E_M2_STATES.EMS_STAND);
            }

            RemoveMainSceneNode();

            WMOSceneNode node = Engine.Instance.SceneManager.AddWMOSceneNode(filename, null);
            if (node != null)
            {
                float scale = 1.0f;
                matrix4 mat = new matrix4(true);
                node.GetRelativeTransformation(out mat);

                matrix4 newMat = new matrix4(true);
                newMat.RotationRadians = mat.RotationRadians;
                newMat.Scale = new vector3df(scale, scale, scale);
                newMat.Translation = mat.Translation;
                node.SetRelativeTransformation(ref newMat);
                node.Update(true);
            }
           
            Engine.Instance.SceneManager.SetDebugWMOSceneNode(node);

            mainWMOSceneNode = node;

            RaiseMainWMOSceneNodeChanged();

             Player player = Client.Instance.GetPlayer();
             player.SetM2AsTarget(null, 15, (float)(Math.PI / 6.0f));

            return node;
        }

        private void RaiseMainWMOSceneNodeChanged()
        {
            if (MainWMOSceneNodeChanged != null)
            {
                MainWMOSceneNodeChanged.Invoke(this, mainWMOSceneNode);
            }
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

        public FileADT LoadADT(uint row, uint col, bool simple)
        {
            if (adt != null)
            {
                wdt.UnloadADT(adt);
                adt = null;
            }
            adt = wdt.LoadADT(row, col, simple);
            return adt;
        }

        public void Initialize()
        {
            Engine.Instance.ManualMeshServices.AddGridMesh("$grid20", 20, 1, new SColor(128, 128, 128));

            gridSceneNode = Engine.Instance.SceneManager.AddMeshSceneNode("$grid20", null);

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
                wdt.UnloadADT(adt);
                adt = null;
                Engine.Instance.ResourceLoader.UnloadWDT(wdt);
                wdt = null;
            }      
        }

    }
}
