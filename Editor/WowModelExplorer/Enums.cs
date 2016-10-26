using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WowModelExplorer
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
        Character,
        Npc,
        Ridable,
        SpellVisualEffect,
        MapModel,
        Wmo,
        WorldM2,
        Clothes,
        Weapon,
        Set,
        Animation,
        MapModelName,
        Property,
        M2State,
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

    public enum EditorSettings
    {
        LineZTest,
    }

    public enum BackMusicFiles
    {
        None,
        MainTitle,
        WotLK,
        Cataclysm,
        Pandaria,
        Draenor,
        HighBorne,
        Stormwind,
        Darnassus,
        Ironforge,
        Orgrimmar,
        Thunderbluff,
        Undercity,
        Silvermoon, 
        Garrosh,
    }

    public enum CameraModes
    {
        FPS,
        Maya,
    }

    public enum NumSetOptions
    {
        Off,
        Medium,
        Low,
        Lowest,
        High,
        Highest
    }
}
