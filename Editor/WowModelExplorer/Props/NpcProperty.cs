using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using mywowNet;
using System.ComponentModel;

namespace WowModelExplorer.Props
{
    public class NpcProperty : INotifyPropertyChanged
    {

        #region display name
        public const string cateMainModel = "主模型";

        public const string propM2File = "M2名";
        public const string propM2LongFile = "M2文件名";
        public const string propM2Textures = "M2纹理";
        public const string propNpc1Texture = "纹理1";
        public const string propNpc2Texture = "纹理2";
        public const string propNpc3Texture = "纹理3";

        #endregion

        private string m2FileName;
        private string m2LongFileName;
        private string[] m2TextureFileNames;
        private string texNpc1FileName;
        private string texNpc2FileName;
        private string texNpc3FileName;

        public void FromM2SceneNode(M2SceneNode node)
        {
            SFileM2 filem2 = node.FileM2;
            M2FileName = filem2.name;
            M2LongFileName = filem2.longname;
            M2TextureNames = node.TextureNames;
            TexNpc1FileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_NPC1);
            TexNpc2FileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_NPC2);
            TexNpc3FileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_NPC3);
        }

        #region MainModel Properties

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string M2FileName
        {
            get { return m2FileName; }
            set
            {
                if (m2FileName == value) return;
                m2FileName = value;
                OnPropertyChanged("M2FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string M2LongFileName
        {
            get { return m2LongFileName; }
            set
            {
                if (m2LongFileName == value) return;
                m2LongFileName = value;
                OnPropertyChanged("M2LongFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2Textures)]
        public string[] M2TextureNames
        {
            get { return m2TextureFileNames; }
            set
            {
                if (m2TextureFileNames == value) return;
                m2TextureFileNames = value;
                OnPropertyChanged("M2TextureFileNames");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propNpc1Texture)]
        public string TexNpc1FileName
        {
            get { return texNpc1FileName; }
            set
            {
                if (texNpc1FileName == value) return;
                texNpc1FileName = value;
                OnPropertyChanged("TexNpc1FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propNpc2Texture)]
        public string TexNpc2FileName
        {
            get { return texNpc2FileName; }
            set
            {
                if (texNpc2FileName == value) return;
                texNpc2FileName = value;
                OnPropertyChanged("TexNpc2FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propNpc3Texture)]
        public string TexNpc3FileName
        {
            get { return texNpc3FileName; }
            set
            {
                if (texNpc3FileName == value) return;
                texNpc3FileName = value;
                OnPropertyChanged("TexNpc3FileName");
            }
        }
        #endregion

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion

    }

    public class NpcPropertyEnglish : INotifyPropertyChanged
    {
        #region display name
        public const string cateMainModel = "Main Model";

        public const string propM2File = "M2 Name";
        public const string propM2LongFile = "M2 FileName";
        public const string propM2Textures = "M2 Textures";
        public const string propNpc1Texture = "Npc Texture1";
        public const string propNpc2Texture = "Npc Texture2";
        public const string propNpc3Texture = "Npc Texture3";

        #endregion

        private string m2FileName;
        private string m2LongFileName;
        private string[] m2TextureFileNames;
        private string texNpc1FileName;
        private string texNpc2FileName;
        private string texNpc3FileName;

        public void FromM2SceneNode(M2SceneNode node)
        {
            SFileM2 filem2 = node.FileM2;
            M2FileName = filem2.name;
            M2LongFileName = filem2.longname;
            M2TextureNames = node.TextureNames;
            TexNpc1FileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_NPC1);
            TexNpc2FileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_NPC2);
            TexNpc3FileName = node.GetReplaceTextureFileName(E_MODEL_REPLACE_TEXTURE.EMRT_NPC3);
        }

        #region MainModel Properties

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2File)]
        public string M2FileName
        {
            get { return m2FileName; }
            set
            {
                if (m2FileName == value) return;
                m2FileName = value;
                OnPropertyChanged("M2FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2LongFile)]
        public string M2LongFileName
        {
            get { return m2LongFileName; }
            set
            {
                if (m2LongFileName == value) return;
                m2LongFileName = value;
                OnPropertyChanged("M2LongFileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propM2Textures)]
        public string[] M2TextureNames
        {
            get { return m2TextureFileNames; }
            set
            {
                if (m2TextureFileNames == value) return;
                m2TextureFileNames = value;
                OnPropertyChanged("M2TextureFileNames");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propNpc1Texture)]
        public string TexNpc1FileName
        {
            get { return texNpc1FileName; }
            set
            {
                if (texNpc1FileName == value) return;
                texNpc1FileName = value;
                OnPropertyChanged("TexNpc1FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propNpc2Texture)]
        public string TexNpc2FileName
        {
            get { return texNpc2FileName; }
            set
            {
                if (texNpc2FileName == value) return;
                texNpc2FileName = value;
                OnPropertyChanged("TexNpc2FileName");
            }
        }

        [Category(cateMainModel)]
        [ReadOnly(true)]
        [DisplayName(propNpc3Texture)]
        public string TexNpc3FileName
        {
            get { return texNpc3FileName; }
            set
            {
                if (texNpc3FileName == value) return;
                texNpc3FileName = value;
                OnPropertyChanged("TexNpc3FileName");
            }
        }
        #endregion

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion
    }
}
