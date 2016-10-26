using System;
using System.Windows;

namespace WowModelExplorer.PropEditors
{
    public static class EditorKeys
    {
        private static ComponentResourceKey textureCollectionKey =
            new ComponentResourceKey(typeof(EditorKeys), "TextureCollectionKey");

        public static ComponentResourceKey TextureCollectionKey
        {
            get { return textureCollectionKey; }
        }
    }
}
