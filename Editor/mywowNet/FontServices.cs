using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class FontServices : INative
    {
        public FontServices(IntPtr raw) : base(raw) { }

        public void OnWindowSizeChanged(uint width, uint height)
        {
            FontServices_onWindowSizeChanged(width, height);
        }
    }
}