using System;
using System.Linq;
using System.Collections;
using System.Collections.Generic;
using System.Windows;
using mywowNet;
using mywowNet.Data;

namespace WowTexViewer.Data
{
    public static class DataExtension
    {
        public static void GetAllMaps(this MapCollection maps, Languages language)
        {
            WowDatabase _wowDatabase = Engine.Instance.WowDatabase;

            uint mapCount = _wowDatabase.MapCount;
            maps.Clear();

            for (uint i = 0; i < mapCount; ++i)
            {
                SMap? m = _wowDatabase.GetMap(i);
                if (m != null)
                {
                    if (m.Value.name.IndexOf("test", StringComparison.CurrentCultureIgnoreCase) != -1 ||
                        m.Value.name.IndexOf("transport", StringComparison.CurrentCultureIgnoreCase) != -1 ||
                        m.Value.name.IndexOf("QA_DVD", StringComparison.CurrentCultureIgnoreCase) != -1 ||
                        m.Value.name.IndexOf("development", StringComparison.CurrentCultureIgnoreCase) != -1)
                        continue;

                    maps.Add(new Map() { Id = m.Value.id, Name = m.Value.name, Type = GetMapTypeName(m.Value.type, language) });
                }
            }
        }

        public static string GetMapTypeName(int type, Languages language)
        {
            switch (language)
            {
                case Languages.Chinese:
                    {
                        switch (type)
                        {
                            case 1:
                                return "地下城";
                            case 2:
                                return "团队";
                            case 3:
                                return "PVP区域";
                            case 4:
                                return "竞技场";
                            default:
                                return "普通";
                        }
                    }
                default:
                    {
                        switch (type)
                        {
                            case 1:
                                return "Dungeon";
                            case 2:
                                return "Raid";
                            case 3:
                                return "PVPZone";
                            case 4:
                                return "Arena";
                            default:
                                return "Normal";
                        }
                    }
            }
        }

        public static void GetAllAdts(this AdtCollection adts, FileWDT wdt)
        {
            adts.Clear();
            for (uint i = 0; i < wdt.TileCount; ++i)
            {
                uint row, col;
                if (!wdt.GetTile(i, out row, out col))
                    continue;
                string name = String.Format("{0},{1}", row, col);

                adts.Add(new Adt() { Name = name, Id = (int)i, Row = row, Col = col });
            }
        }
    }
}
