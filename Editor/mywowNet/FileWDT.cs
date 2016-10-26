using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;

namespace mywowNet
{
    public partial class FileWDT : INative
    {
        public FileWDT(IntPtr raw) : base(raw) { }

        public uint TileCount
        {
            get { return FileWDT_getTileCount(pointer);  }
        }

        public bool GetTile(uint index, out uint row, out uint col)
        {
            return FileWDT_getTile(pointer, index, out row, out col);
        }

        public FileADT LoadADT(uint row, uint col, bool simple)
        {
            IntPtr raw = FileWDT_loadADT(pointer, row, col, simple);
            if (raw == IntPtr.Zero)
                return null;
            return new FileADT(raw);
        }

        public FileADT LoadADTTextures(uint row, uint col)
        {
            IntPtr raw = FileWDT_loadADTTextures(pointer, row, col);
            if (raw == IntPtr.Zero)
                return null;
            return new FileADT(raw);
        }

        public void UnloadADT(FileADT adt)
        {
            if(adt != null)
                FileWDT_unloadADT(pointer, adt.pointer);
        }
    }
}
