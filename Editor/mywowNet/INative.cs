using System.Diagnostics;
using System;

namespace mywowNet
{
    public class INative
    {
        internal IntPtr pointer = IntPtr.Zero;

        public INative(IntPtr raw)
        {
            FromRaw(raw);
        }

        public INative() { }

        public void FromRaw(IntPtr raw)
        {
            Debug.Assert(raw != IntPtr.Zero);

            pointer = raw;
        }

        public static bool operator ==(INative a, INative b)
        {
            if (System.Object.ReferenceEquals(a, b))
            {
                return true;
            }

            // If one is null, but not both, return false.
            if (((object)a == null) || ((object)b == null))
            {
                return false;
            }

            return a.pointer == b.pointer;
        }

        public static bool operator !=(INative a, INative b)
        {
            if (System.Object.ReferenceEquals(a, b))
            {
                return false;
            }

            // If one is null, but not both, return true.
            if (((object)a == null) || ((object)b == null))
            {
                return true;
            }

            return a.pointer != b.pointer;
        }

        public override bool Equals(object obj)
        {
            INative b = obj as INative;
            return this == b;
        }

        public override int GetHashCode()
        {
            return pointer.GetHashCode();
        }

    }
}
