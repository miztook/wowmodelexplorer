using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class M2Appearance : INative
    {
        public M2Appearance(IntPtr raw) : base(raw) { }

        public M2Appearance() { }

        public bool TakeItem(int itemId)
        {
            return m2Appearance_takeItem(pointer, itemId);
        }

        public void LoadStartOutfitId(int startid, bool deathknight)
        {
            m2Appearance_loadStartOutfit(pointer, startid, deathknight);
        }

        public void LoadSet(int setid)
        {
            m2Appearance_loadSet(pointer, setid);
        }

        public bool SheathLeftHand
        {
            get { return m2Appearance_isLeftWeaponSheathed(pointer); }
            set
            {
                m2Appearance_sheathLeftWeapon(pointer, value);
            }
        }

        public bool SheathRightHand
        {
            get { return m2Appearance_isRightWeaponSheathed(pointer); }
            set
            {
                m2Appearance_sheathRightWeapon(pointer, value);
            }
        }

    }

}