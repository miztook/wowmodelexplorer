using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class mwTool : IDisposable
    {
        private static mwTool instance = null;

        public mwTool()
        {
            if (instance == null)
            {
                mwTool_create();

                instance = this;
            }
        }

        ~mwTool()
        {
            Dispose();
        }

        public static mwTool Instance
        {
            get { return instance; }
        }

        bool _disposed = false;
        public void Dispose()
        {
            if (!_disposed)
            {
                mwTool_destroy();
                _disposed = true;
            }
        }

        public bool OpenUrlToJsonFile(string url, string filename)
        {
            return mwTool_openUrlToJsonFile(url, filename);
        }

        public bool ParseCharacterArmoryInfo(string filename, out SCharacterArmoryInfo charInfo)
        {
            return mwTool_parseCharacterArmoryInfo(filename, out charInfo);
        }

        public bool ExportBlpAsTga(string blpfilename, string tgafilename, bool alpha)
        {
            return mwTool_exportBlpAsTga(blpfilename, tgafilename, alpha);
        }

        public bool ExportBlpAsTgaDir(string blpfilename, string dirname, bool alpha)
        {
            return mwTool_exportBlpAsTgaDir(blpfilename, dirname, alpha);
        }

        public bool ExportM2SceneNodeToOBJ(M2SceneNode node, string dirname)
        {
            if (node == null)
                return false;
            return mwTool_exportM2SceneNodeToOBJ(node.pointer, dirname);
        }

        public bool ExportWMOSceneNodeToOBJ(WMOSceneNode node, string dirname)
        {
            if (node == null)
                return false;
            return mwTool_exportWMOSceneNodeToOBJ(node.pointer, dirname);
        }
    }

}