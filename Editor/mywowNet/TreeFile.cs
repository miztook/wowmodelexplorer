using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mywowNet
{
    public class TreeFileInfo
    {
        public string Name;
        public string FullName;

        public TreeFileInfo(string filename)
        {
            FullName = filename;
            Name = Function.GetFileNameA(FullName);
        }
    }

    public class TreeDirectoryInfo
    {
        public string Name;
        public string FullName;

        public TreeDirectoryInfo(string filename)
        {
            FullName = Function.GetFileDirA(filename);
            Name = Function.GetFileNameA(FullName);
        }
    }

}
