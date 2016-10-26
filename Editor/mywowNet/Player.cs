using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Player : INative
    {
        public Player(IntPtr raw) : base(raw) { }

        Camera _target = new Camera();
        public Camera GetTargetCamera()
        {
            IntPtr cam = Player_getTargetCamera(pointer);
            if (cam == IntPtr.Zero)
                return null;
            _target.FromRaw(cam);
            return _target;
        }

        public void SetTargetCamera(float nearValue, float farValue, float fov)
        {
            Player_setTargetCamera(pointer, nearValue, farValue, fov);
        }

        public void SetM2AsTarget(M2SceneNode m2Node, float distance, float rad)
        {
            IntPtr node = IntPtr.Zero;
            if (m2Node != null)
            {
                node = m2Node.pointer;
            }
            Player_setM2AsTarget(pointer, node, distance, rad);
        }

        public void SetMoveSpeed(float walk, float run, float backwards, float roll)
        {
            Player_setMoveSpeed(pointer, walk, run, backwards, roll);
        }

        public void GetMoveSpeed(out float walk, out float run, out float backwards, out float roll)
        {
            Player_getMoveSpeed(pointer, out walk, out run, out backwards, out roll);
        }

        public int GetRideNpcId()
        {
            return Player_getRideNpcId(pointer);
        }

        public void RideOnModel(int npcid, E_M2_STATES state)
        {
            Player_rideOnModel(pointer, npcid, state);
        }

    }
}
