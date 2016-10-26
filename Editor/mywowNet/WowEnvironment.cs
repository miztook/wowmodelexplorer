using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public struct FileDirIInfo
    {
        public string Name;
        public string FullName;
    }

    public partial class WowEnvironment : INative
    {
        public WowEnvironment(IntPtr raw) : base(raw) { }

        public bool IsFileExist(string filename)
        {
            return WowEnvironment_Exists(filename);
        }

        public uint CascFileCount
        {
            get { return WowEnvironment_GetCascFileCount(); }
        }

        public string GetCascFile(int index)
        {
            return Marshal.PtrToStringAnsi(WowEnvironment_GetCascFile(index));
        }

        public void ClearOwnCascFiles()
        {
            WowEnvironment_ClearOwnCascFiles();
        }

        public void AddOwnCascFile(string filename)
        {
            WowEnvironment_AddOwnCascFile(filename);
        }

        public void FinishOwnCascFiles()
        {
            WowEnvironment_FinishOwnCascFiles();
        }

        public int GetFilesCountInDir(string baseDir, string ext, bool useOwn)
        {
            return WowEnvironment_GetFileCount_InDir(baseDir, ext, useOwn);
        }

        public string GetFileInDir(bool fullname, int index)
        {
            StringBuilder path = new StringBuilder(256);
            WowEnvironment_GetFile_InDir(fullname, index, path, (uint)path.Capacity);
            return path.ToString(0, path.Length);
        }

        public int GetDirectoryCountInDir(string baseDir, bool useOwn)
        {
            return WowEnvironment_GetDirectoryCount_InDir(baseDir, useOwn);
        }

        public string GetDirectoryInDir(bool fullname, int index)
        {
            StringBuilder path = new StringBuilder(256);
            WowEnvironment_GetDirectory_InDir(fullname, index, path, (uint)path.Capacity);
            return path.ToString(0, path.Length);
        }

        public FileDirIInfo[] GetFilesInDir(string baseDir, string ext, bool useOwn)
        {
            int count = GetFilesCountInDir(baseDir, ext, useOwn);
            FileDirIInfo[] infos = new FileDirIInfo[count];
            for (int i = 0; i < count; ++i)
            {
                infos[i].FullName = GetFileInDir(true, i);
                infos[i].Name = GetFileInDir(false, i);
            }
            return infos;
        }

        public FileDirIInfo[] GetDirectoriesInDir(string baseDir, bool useOwn)
        {
            int count = GetDirectoryCountInDir(baseDir, useOwn);
            FileDirIInfo[] infos = new FileDirIInfo[count];
            for (int i = 0; i < count; ++i)
            {
                infos[i].FullName = GetDirectoryInDir(true, i);
                infos[i].Name = GetDirectoryInDir(false, i);
            }
            return infos;
        }

        public string GetLocale()
        {
            return Marshal.PtrToStringAnsi(WowEnvironment_GetLocale());
        }
    }
}
