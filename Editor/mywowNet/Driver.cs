using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Driver : INative
    {
        public Driver(IntPtr raw) : base(raw) { }

        public void SetDisplayMode(uint width, uint height)
        {
            Driver_setDisplayMode(width, height);
        }

        public void GetDisplayMode(out uint width, out uint height)
        {
            Driver_getDisplayMode(out width, out height);
        }

    }
}
