using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WowMapExplorer.Services
{
    class MapEditorService
    {
        //options
        private bool isShowDebugText = true;
        private bool isShowCoord = true;
        private bool isShowLightCoord = false;

        public MapEditorService()
        {
            if (instance == null)
            {
                instance = this;
            }
        }

        private static MapEditorService instance = null;
        public static MapEditorService Instance { get { return instance; } }

        public bool IsShow(EditorShowOptions option)
        {
            switch (option)
            {
                case EditorShowOptions.DebugText:
                    return isShowDebugText;
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
