using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Camera : INative
    {
        public Camera(IntPtr raw) : base(raw) { }

        public Camera() { }

        public vector3df Position
        {
            set 
            {
                Camera_setPosition(pointer, value);
            }
            get
            {
                vector3df v = new vector3df();
                Camera_getPosition(pointer, out v);
                return v;
            }
        }

        public vector3df Dir
        {
            set
            {
                Camera_setDir(pointer, value);
            }
            get
            {
                vector3df v = new vector3df();
                Camera_getDir(pointer, out v);
                return v;
            }
        }

        public vector3df Up
        {
            get
            {
                vector3df v = new vector3df();
                Camera_getUp(pointer, out v);
                return v;
            }
        }

        public vector3df LookAt
        {
            set
            {
                Camera_setLookat(pointer, value);
            }
            get
            {
                vector3df v = new vector3df();
                Camera_getLookat(pointer, out v);
                return v;
            }
        }

        public void OnKeyMove(float speed, SKeyControl keycontrol)
        {
            Camera_onKeyMove(pointer, speed, keycontrol);
        }

        public void RecalculateAll()
        {
            Camera_recalculateAll(pointer);
        }

        public void PitchYawMaya(float pitchDegree, float yawDegree)
        {
            Camera_pitch_yaw_Maya(pointer, pitchDegree, yawDegree);
        }

        public void MoveOffsetMaya(float xOffset, float yOffset)
        {
            Camera_move_offset_Maya(pointer, xOffset, yOffset);
        }

        public void PitchYawFPS(float pitchDegree, float yawDegree)
        {
            Camera_pitch_yaw_FPS(pointer, pitchDegree, yawDegree);
        }
    }
}
