using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Text;

namespace mywowNet
{
    public partial class MapTileSceneNode : SceneNode
    {
        public MapTileSceneNode(IntPtr raw) : base(raw)
        {
        }

        public vector3df GetCenter()
        {
            vector3df v = new vector3df();
            MapTileSceneNode_getCenter(pointer, out v);
            return v;
        }
    }
}
