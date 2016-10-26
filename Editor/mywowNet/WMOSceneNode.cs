using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class WMOSceneNode : SceneNode
    {
        public WMOSceneNode(IntPtr raw) : base(raw) {}

        public void SetWireFrame(E_OVERRIDE_WIREFRAME wireframe)
        {
            WMOSceneNode_setWireframe(pointer, wireframe);
        }

        public SFileWMO FileWMO
        {
            get
            {
                SFileWMO wmo = new SFileWMO();
                WMOSceneNode_getFileWMO(pointer, out wmo);
                return wmo;
            }
        }

        SWMOGroup[] _groups;
        public SWMOGroup[] Groups
        {
            get
            {
                uint num = FileWMO.numGroups;
                _groups = new SWMOGroup[num];
                for (uint i = 0; i < num; ++i)
                {
                    WMOSceneNode_getGroup(pointer, i, out _groups[i]);
                }
                return _groups;
            }
        }

        SWMOPortal[] _portals;
        public SWMOPortal[] Portals
        {
            get
            {
                uint num = FileWMO.numPortals;
                _portals = new SWMOPortal[num];
                for (uint i = 0; i < num; ++i)
                {
                    WMOSceneNode_getPortal(pointer, i, out _portals[i]);
                }
                return _portals;
            }
        }

        public void ShowGroup(int index, bool show)
        {
            WMOSceneNode_showGroup(pointer, (uint)index, show);
        }

        public bool IsGroupShow(int index)
        {
            return WMOSceneNode_isGroupShow(pointer, (uint)index);
        }

        public void ShowGroupBox(int index, bool show, SColor color)
        {
            WMOSceneNode_showGroupBox(pointer, (uint)index, show, color);
        }

        public bool IsGroupBoxShow(int index)
        {
            return WMOSceneNode_isGroupBoxShow(pointer, (uint)index);
        }

        public void ShowPortal(int index, bool show, SColor color)
        {
            WMOSceneNode_showPortal(pointer, (uint)index, show, color);
        }

        public bool IsPortalShow(int index)
        {
            return WMOSceneNode_isPortalShow(pointer, (uint)index);
        }

        public void ShowPortalGroups(int index, bool show)
        {
            WMOSceneNode_showPortalGroups(pointer, (uint)index, show);
        }

        public bool IsPortalGroupsShow(int index)
        {
            return WMOSceneNode_isPortalGroupsShow(pointer, (uint)index);
        }

        public void ShowEditPart(E_WMO_EDIT_PART editpart, bool show)
        {
            WMOSceneNode_showEditPart(pointer, editpart, show);
        }

        public string GetDoodadName(int index)
        {
            StringBuilder path = new StringBuilder(512);
            bool success = WMOSceneNode_getDoodadName(pointer, (uint)index, path, (uint)path.Capacity);
            if (success)
            {
                return path.ToString(0, path.Length);
            }
            else
            {
                return "";
            }
        }

    }
}