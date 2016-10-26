using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class M2Instance : INative
    {
        public M2Instance(IntPtr raw) : base(raw) { }

        public M2Instance() { }

        public SCharFeature CharFeature
        {
            get
            {
                SCharFeature feature = new SCharFeature();
                m2Instance_getCharFeature(pointer, out feature);
                return feature;
            }
            set { m2Instance_setCharFeature(pointer, ref value); }
        }

        public int GetItemSlot(int itemid)
        {
            return m2Instance_getItemSlot(pointer, itemid);
        }

        public int GetSlotItemId(E_CHAR_SLOTS slot)
        {
            return m2Instance_getSlotItemId(pointer, (int)slot);
        }

        public bool SlotHasModel(E_CHAR_SLOTS slot)
        {
            return m2Instance_slotHasModel(pointer, (int)slot);
        }

    }
}