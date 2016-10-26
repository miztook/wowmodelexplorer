using System;
using System.Text;
using System.Collections;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace mywowNet
{
    public partial class FileADT : INative
    {
        public FileADT(IntPtr raw) : base(raw) { }

        public uint M2Count
        {
            get { return FileADT_getM2Count(pointer); }
        }

        public uint WMOCount
        {
            get { return FileADT_getWMOCount(pointer); }
        }

        public uint TextureCount
        {
            get { return FileADT_getTextureCount(pointer); }
        }

        public string GetM2FileName(uint index, bool shortname)
        {
            return Marshal.PtrToStringAnsi(FileADT_getM2FileName(pointer, index, shortname));
        }

        public string GetWMOFileName(uint index, bool shortname)
        {
            return Marshal.PtrToStringAnsi(FileADT_getWMOFileName(pointer, index, shortname));
        }

        public string GetTextureFileName(uint index, bool shortname)
        {
            return Marshal.PtrToStringAnsi(FileADT_getTextureFileName(pointer, index, shortname));
        }
    }
}
