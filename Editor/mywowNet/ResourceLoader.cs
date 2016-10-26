using System;
using System.Collections;
using System.Collections.Generic;

namespace mywowNet
{
    public partial class ResourceLoader : INative
    {
        public ResourceLoader(IntPtr raw) : base(raw) { }

        public FileWDT LoadWDT(string mapname, int mapid, bool simple)
        {
            IntPtr raw = ResourceLoader_loadWDT(mapname, mapid, simple);
            if (raw == IntPtr.Zero)
                return null;
            return new FileWDT(raw);
        }

        public void UnloadWDT(FileWDT wdt)
        {
            if (wdt != null)
                ResourceLoader_unloadWDT(wdt.pointer);
        }
    }
}
