using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class SceneEnvironment
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneEnvironment_setLightDir")]
        public static extern void SceneEnvironment_setLightDir(vector3df dir);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SceneEnvrionment_getLightDir")]
        public static extern void SceneEnvrionment_getLightDir(out vector3df dir);
    }
}
