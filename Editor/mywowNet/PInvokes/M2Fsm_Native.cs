using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class M2Fsm
    {
        const string DllName = @"mywow_dll.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Fsm_resetState")]
        public static extern void m2Fsm_resetState(
            IntPtr fsm);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Fsm_isStateValid")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool m2Fsm_isStateValid(
            IntPtr fsm,
            E_M2_STATES state);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "m2Fsm_changeState")]
        [return: MarshalAsAttribute(UnmanagedType.I1)]
        public static extern bool m2Fsm_changeState(
            IntPtr fsm,
            E_M2_STATES state);

    }
}
