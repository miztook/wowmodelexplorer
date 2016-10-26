using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class ManualMeshServices : INative
    {
        public ManualMeshServices(IntPtr raw) : base(raw) { }

        public bool AddGridMesh(string name,  uint xzCount, float gridSize, SColor color)
        {
            return ManualMeshServices_addGridMesh(name, xzCount, gridSize, color);
        }

        public void RemoveMesh(string name)
        {
            ManualMeshServices_removeMesh(name);
        }
    }
}
