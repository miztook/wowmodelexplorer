using System;
using System.Text;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class WowDatabase : INative
    {
        public WowDatabase(IntPtr raw) : base(raw) { }

        public uint ItemCount
        {
            get { return WowDatabase_getItemCount(); }
        }

        public uint NpcCount
        {
            get { return WowDatabase_getNpcCount(); }
        }

        public uint SetCount
        {
            get { return WowDatabase_getSetCount(); }
        }

        public uint MapCount
        {
            get { return WowDatabase_getMapCount(); }
        }

        public uint SpellVisualEffectCount
        {
            get { return WowDatabase_getSpellVisualEffectCount(); }
        }

        public uint WmoCount
        {
            get { return WowDatabase_getWmoCount();  }
        }

        public uint WorldModelCount
        {
            get { return WowDatabase_getWorldModelCount(); }
        }

        public uint TextureCount
        {
            get { return WowDatabase_getTextureCount();  }
        }

        public uint RidableCount
        {
            get { return WowDatabase_getRidableCount(); }
        }

        public uint GetStartOutfitCount(uint raceid, bool female)
        {
            return WowDatabase_getStartOutfitCount(raceid, female);
        }

        public SItem? GetItem(uint index)
        {
            SItem v = new SItem();
            if (WowDatabase_getItem(index, out v))
                return v;
            return null;
        }

        public SItem? GetItemById(int id)
        {
            SItem v = new SItem();
            if (WowDatabase_getItemById(id, out v))
                return v;
            return null;
        }

        public SNpc? GetNpc(uint index)
        {
            SNpc v = new SNpc();
            if(WowDatabase_getNpc(index, out v))
                return v;
            return null;
        }

        public SNpc? GetNpcById(int id)
        {
            SNpc v = new SNpc();
            if (WowDatabase_getNpcById(id, out v))
                return v;
            return null;
        }

        public SRidable? GetRidable(uint index)
        {
            SRidable v = new SRidable();
            if (WowDatabase_getRidable(index, out v))
                return v;
            return null;
        }

        public SStartOutfit? GetStartOutfit(uint raceid, bool female, uint index)
        {
            SStartOutfit v = new SStartOutfit();
            if (WowDatabase_getStartOutfit(raceid, female, index, out v))
                return v;
            return null;
        }

        public SEntry? GetSet(uint index)
        {
            SEntry v = new SEntry();
            if (WowDatabase_getSet(index, out v))
                return v;
            return null;
        }

        public SMap? GetMap(uint index)
        {
            SMap v = new SMap();
            if (WowDatabase_getMap(index, out v))
                return v;
            return null;
        }

        public int? GetSpellVisualEffectId(uint index)
        {
            int v;
            if (WowDatabase_getSpellVisualEffectId(index, out v))
                return v;
            return null;
        }

        public string GetWMOFileName(uint index, bool shortname)
        {
            return Marshal.PtrToStringAnsi(WowDatabase_getWMOFileName(index, shortname));
        }

        public string GetWorldModelName(uint index, bool shorname)
        {
            return Marshal.PtrToStringAnsi(WowDatabase_getWorldModelFileName(index, shorname));
        }

        public string GetTextureFileName(uint index)
        {
            return Marshal.PtrToStringAnsi(WowDatabase_getTextureFileName(index));
        }

        public SCharFeature GetMaxCharFeature(uint raceid, bool female, bool isHD)
        {
            SCharFeature v = new SCharFeature();
            WowDatabase_getMaxCharFeature(raceid, female, isHD, out v);
            return v;
        }

        public uint? GetRaceId(string racename)
        {
            uint id;
            if (WowDatabase_getRaceId(racename, out id))
                return id;
            return null;
        }

        public string GetRaceName(uint raceid)
        {
            return Marshal.PtrToStringAnsi(WowDatabase_getRaceName(raceid));
        }

        public string GetClassShortName(uint classid)
        {
            return Marshal.PtrToStringAnsi(WowDatabase_getClassShortName(classid));
        }

        public string GetClassName(string shortname)
        {
            SEntry entry;
            bool success = WowDatabase_getClassInfo(shortname, out entry);
            if (success)
            {
                return entry.name;
            }
            else
            {
                return "";
            }
        }

        public void BuildItems()
        {
            WowDatabase_buildItems();
        }

        public bool BuildNpcs(string filename)
        {
            return WowDatabase_buildNpcs(filename);
        }

        public void BuildStartOutfit()
        {
            WowDatabase_buildStartOutfitClass();
        }

        public void BuildMaps()
        {
            WowDatabase_buildMaps();
        }

        public void BuildWmos()
        {
            WowDatabase_buildWmos();
        }

        public void BuildWorldModels()
        {
            WowDatabase_buildWorldModels();
        }

        public void BuildTextures()
        {
            WowDatabase_buildTextures();
        }

        public void BuildRidables(string filename)
        {
            WowDatabase_buildRidables(filename);
        }

        public string GetCharacterPath(string race, bool female, bool isHD)
        {
            StringBuilder path = new StringBuilder(256);
            bool success = WowDatabase_getCharacterPath(race, female, isHD, path, (uint)path.Capacity);
            if (success)
            {
                return path.ToString(0, path.Length);
            }
            else
            {
                return "";
            }
        }

        public string GetNpcPath(int npcid, bool isHD)
        {
            StringBuilder path = new StringBuilder(256);
            bool success = WowDatabase_getNpcPath(npcid, isHD, path, (uint)path.Capacity);
            if (success)
            {
                return path.ToString(0, path.Length);
            }
            else
            {
                return "";
            }
        }

        public string GetItemVisualPath(int visualId)
        {
            StringBuilder path = new StringBuilder(256);
            bool success = WowDatabase_getItemVisualPath(visualId, path, (uint)path.Capacity);
            if (success)
            {
                return path.ToString(0, path.Length);
            }
            else
            {
                return "";
            }
        }

        public string GetSpellVisualEffectName(int visualId)
        {
            return Marshal.PtrToStringAnsi(WowDatabase_getSpellVisualEffectName(visualId));
        }

        public string GetSpellVisualEffectPath(int visualId)
        {
            StringBuilder path = new StringBuilder(256);
            bool success = WowDatabase_getSpellVisualEffectPath(visualId, path, (uint)path.Capacity);
            if (success)
            {
                return path.ToString(0, path.Length);
            }
            else
            {
                return "";
            }
        }

        public string GetItemPath(int itemid, out string texturePath)
        {
            StringBuilder modelpath = new StringBuilder(256);
            StringBuilder texpath = new StringBuilder(256);

            bool success = WowDatabase_getItemPath(itemid, modelpath, 256, texpath, 256);
            if (success)
            {
                texturePath = texpath.ToString(0, texpath.Length);
                return modelpath.ToString(0, modelpath.Length);
            }
            else
            {
                texturePath = "";
                return "";
            }
        }

    }
}
