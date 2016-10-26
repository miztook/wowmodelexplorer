using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WowTexViewer.Services
{
    public class ModelEditorService
    {
        public ModelEditorService()
        {
            if (instance == null)
            {
                instance = this;
            }
        }

        private static ModelEditorService instance = null;
        public static ModelEditorService Instance { get { return instance; } }

        //options
        private bool isShowDebugText = true;
        private bool isShowDebugGeosets = false;
        private bool isShowPlane = true;
        private bool isShowCoord = true;
        private bool isShowLightCoord = false;

        public bool IsShow(EditorShowOptions option)
        {
            switch (option)
            {
                case EditorShowOptions.DebugText:
                    return isShowDebugText;
                case EditorShowOptions.DebugGeosets:
                    return isShowDebugGeosets;
                case EditorShowOptions.Plane:
                    return isShowPlane;
                case EditorShowOptions.Coord:
                    return isShowCoord;
                case EditorShowOptions.LightCoord:
                    return isShowLightCoord;
            }
            return false;
        }

        public void SetShow(EditorShowOptions option, bool show)
        {
            switch (option)
            {
                case EditorShowOptions.DebugText:
                    isShowDebugText = show;
                    break;
                case EditorShowOptions.DebugGeosets:
                    isShowDebugGeosets = show;
                    break;
                case EditorShowOptions.Plane:
                    isShowPlane = show;
                    break;
                case EditorShowOptions.Coord:
                    isShowCoord = show;
                    break;
                case EditorShowOptions.LightCoord:
                    isShowLightCoord = show;
                    break;
            }
        }

    }
}
