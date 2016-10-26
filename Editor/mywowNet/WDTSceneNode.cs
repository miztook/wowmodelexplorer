using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class WDTSceneNode : SceneNode
    {
        public WDTSceneNode(IntPtr raw) : base(raw) { }

        public void SetCurrentTile(int row, int col) 
        {
            WDTSceneNode_setCurrentTile(pointer, row, col);
        }

        public vector3df GetCenter()
        {
            vector3df v = new vector3df();
            WDTSceneNode_getCenter(pointer, out v);
            return v;
        }
    }
}