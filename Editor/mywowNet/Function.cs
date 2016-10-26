using System;
using System.Text;

namespace mywowNet
{
    public static partial class Function
    {
        public static string GetFileDirA(string filename)
        {
            StringBuilder path = new StringBuilder(512);
            Function_getFileDirA(filename, path, (uint)path.Capacity);
            return path.ToString(0, path.Length);
        }

        public static string GetFileNameA(string filename)
        {
            StringBuilder path = new StringBuilder(512);
            Function_getFileNameA(filename, path, (uint)path.Capacity);
            return path.ToString(0, path.Length);
        }

        public static string GetFileNameNoExtensionA(string filename)
        {
            StringBuilder path = new StringBuilder(512);
            Function_getFileNameNoExtensionA(filename, path, (uint)path.Capacity);
            return path.ToString(0, path.Length);
        }

        public static bool HasFileExtensionA(string filename, string ext)
        {
            return Function_hasFileExtensionA(filename, ext);
        }

    }
}
