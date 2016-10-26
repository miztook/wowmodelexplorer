using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class M2Fsm : INative
    {
        public M2Fsm(IntPtr raw) : base(raw) { }

        public M2Fsm() { }

        public void ResetState()
        {
            m2Fsm_resetState(pointer);
        }

        public bool IsStateValid(E_M2_STATES state)
        {
            return m2Fsm_isStateValid(pointer, state);
        }

        public bool ChangeState(E_M2_STATES state)
        {
            return m2Fsm_changeState(pointer, state);
        }

    }
}
