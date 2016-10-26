using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class DrawServices : INative
    {
        public DrawServices(IntPtr raw) : base(raw) { }

        public bool LineZTestEnable
        {
            get { return DrawServices_getLineZTestEnable();  }
            set { DrawServices_setLineZTestEnable(value);  }
        }
    }
}