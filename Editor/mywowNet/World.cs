using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class World : INative
    {
        public World(IntPtr raw) : base(raw) { }

    }
}