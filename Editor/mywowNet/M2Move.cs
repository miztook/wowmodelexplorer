using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class M2Move : INative
    {
        public M2Move(IntPtr raw) : base(raw) { }

        public M2Move() {}

        public vector3df Dir
        {
            get 
            {
                vector3df v = new vector3df();
                m2Move_getDir(pointer, out v);
                return v;
            }
            set
            {
                m2Move_setDir(pointer, ref value);
            }
        }

        public vector3df Pos
        {
            get
            {
                vector3df v = new vector3df();
                m2Move_getPos(pointer, out v);
                return v;
            }
            set
            {
                m2Move_setPos(pointer, ref value);
            }
        }

        public vector3df Scale
        {
            get
            {
                vector3df v = new vector3df();
                m2Move_getScale(pointer, out v);
                return v;
            }
            set
            {
                m2Move_setScale(pointer, ref value);
            }
        }
    }

}