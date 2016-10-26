using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WowTexViewer
{
    public enum Languages
    {
        Chinese,
        English,
    }

    public enum OpenTypes
    {
        LoginScreen,
    }

    public enum ModelViewTypes
    {
        AllTexture,
        MapTexture,
        MapTextureName,
    }

    public enum EditorShowOptions
    {
        DebugText,
        Plane,
        DebugGeosets,
        Coord,
        LightCoord,
    }

    public enum EditorSetOptions
    {
        Default,
        Set,
        None,
    }
}
