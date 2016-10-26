using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public static class Extension
    {
        public static bool IsMatchSlot(this SItem item, E_CHAR_SLOTS slot)
        {
            return WowDatabase.WowDatabase_itemIsCorrectType(item.type, (int)slot);
        }
    }
}
