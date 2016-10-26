using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Text;

namespace mywowNet
{
    public partial class CoordSceneNode : SceneNode
    {
         public CoordSceneNode(IntPtr raw) : base(raw) { }

        public void SetPosition2D(E_POSITION_2D position, float distance)
        {
            CoordSceneNode_setPosition2D(pointer, position, distance);
        }

        public void SetAxisColor(SColor colorX, SColor colorY, SColor colorZ)
        {
            CoordSceneNode_setAxisColor(pointer, colorX, colorY, colorZ);
        }

        public void SetAxisText(string textX, string textY, string textZ)
        {
            CoordSceneNode_setAxisText(pointer, textX, textY, textZ);
        }

        public void SetAxisVisible(bool xVisible, bool yVisible, bool zVisible)
        {
            CoordSceneNode_setAxisVisible(pointer, xVisible, yVisible, zVisible);
        }

        public void SetAxisDir(E_AXIS axis, vector3df dir)
        {
            CoordSceneNode_setDir(pointer, axis, dir);
        }

        public vector3df GetAxisDir(E_AXIS axis)
        {
            vector3df v = new vector3df();
            CoordSceneNode_getDir(pointer, axis, out v);
            return v;
        }

        public void PitchYawFPS(float pitchDegree, float yawDegree)
        {
            CoordSceneNode_pitch_yaw_FPS(pointer, pitchDegree, yawDegree);
        }
    }
}
